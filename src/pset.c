#include "pset.h"

int PSetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  if (argc != 5)
    return RedisModule_WrongArity(ctx);
  RedisModule_AutoMemory(ctx);

  // Extract <sign_key> and validate it.
  size_t signKeyLen;
  const char *signKey = RedisModule_StringPtrLen(argv[1], &signKeyLen);
  if (signKeyLen == 0)
    return RedisModule_ReplyWithError(
        ctx, "<sign_key> must have at least one character");

  // Extract <token> and validate it.
  size_t tokenLen;
  const char *token = RedisModule_StringPtrLen(argv[2], &tokenLen);
  if (tokenLen == 0)
    return RedisModule_ReplyWithError(
        ctx, "<token> must have at least one character");
  else if (tokenLen != TOKEN_STRLEN)
    return RedisModule_ReplyWithError(ctx, "<token> format is invalid");

  // Parse the token.
  char tokenVersion[TOKEN_VERSION_STRLEN + 1];
  char sessionId[SESSION_ID_STRLEN + 1];
  char signature[SIGNATURE_STRLEN + 1];
  parseToken(token, tokenVersion, sessionId, signature);

  // Recreate the signature of the session id and compare with the signature
  // contained in the token.
  char signatureCheck[SIGNATURE_STRLEN + 1];
  signData((const unsigned char *)signKey, signKeyLen,
           (const unsigned char *)sessionId, SESSION_ID_STRLEN, signatureCheck);
  if (strncmp(signature, signatureCheck, SIGNATURE_STRLEN) != 0)
    return RedisModule_ReplyWithError(
        ctx, "the signature contained in <token> is invalid");

  // Check if the signature is the same stored in the session.
  RedisModuleString *sessionSignatureKeyStr =
      RedisModule_CreateStringPrintf(ctx, "sg-session:%s:signature", sessionId);
  RedisModuleKey *redisKey = RedisModule_OpenKey(
      ctx, sessionSignatureKeyStr, REDISMODULE_READ | REDISMODULE_WRITE);
  if (RedisModule_KeyType(redisKey) != REDISMODULE_KEYTYPE_STRING) {
    RedisModule_CloseKey(redisKey);
    return RedisModule_ReplyWithError(
        ctx, "the session id contained in <token> does not exist");
  }
  size_t signatureStoredLen;
  char *signatureStored =
      RedisModule_StringDMA(redisKey, &signatureStoredLen, REDISMODULE_READ);
  if (strncmp(signature, signatureStored, SIGNATURE_STRLEN) != 0) {
    RedisModule_CloseKey(redisKey);
    return RedisModule_ReplyWithError(
        ctx, "the signature contained in <token> seems to be valid, but is "
             "different from the stored signature in the session");
  }

  // Get the TTL of the session to assign it to the payload.
  mstime_t ttl = RedisModule_GetExpire(redisKey);

  RedisModule_CloseKey(redisKey);

  // Extract <payload_name> and validate it.
  const RedisModuleString *payloadName = argv[3];
  size_t payloadNameLen;
  RedisModule_StringPtrLen(payloadName, &payloadNameLen);
  if (payloadNameLen == 0)
    return RedisModule_ReplyWithError(
        ctx, "<payload_name> must have at least one character");
  else if (payloadNameLen > PAYLOAD_NAME_MAX_STRLEN) {
    char msg[128];
    sprintf(msg,
            "<payload_name> length exceeds the maximum value allowed of %zu",
            PAYLOAD_NAME_MAX_STRLEN);
    return RedisModule_ReplyWithError(ctx, msg);
  }

  // Extract <payload_data> and validate it.
  const RedisModuleString *payloadData = argv[4];
  size_t payloadDataLen;
  RedisModule_StringPtrLen(payloadData, &payloadDataLen);
  if (payloadDataLen == 0)
    return RedisModule_ReplyWithError(
        ctx, "<payload_data> must have at least one character");
  else if (payloadDataLen > PAYLOAD_DATA_MAX_STRLEN) {
    char msg[128];
    sprintf(msg,
            "<payload_data> length exceeds the maximum value allowed of %zu",
            PAYLOAD_DATA_MAX_STRLEN);
    return RedisModule_ReplyWithError(ctx, msg);
  }

  // Set the payload.
  RedisModuleString *sessionPayloadsKeyStr =
      RedisModule_CreateStringPrintf(ctx, "sg-session:%s:payloads", sessionId);
  redisKey = RedisModule_OpenKey(ctx, sessionPayloadsKeyStr, REDISMODULE_WRITE);
  RedisModule_HashSet(redisKey, REDISMODULE_HASH_NONE, payloadName, payloadData,
                      NULL);

  // Set the TTL.
  if (ttl > 0)
    RedisModule_SetExpire(redisKey, ttl);

  RedisModule_CloseKey(redisKey);

  RedisModule_ReplyWithSimpleString(ctx, "OK");
  return REDISMODULE_OK;
}
