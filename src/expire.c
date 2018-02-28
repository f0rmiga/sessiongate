#include "expire.h"

int ExpireCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  if (argc != 4)
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

  // Extract <ttl> and validate it.
  long long ttl;
  if (RedisModule_StringToLongLong(argv[3], &ttl) != REDISMODULE_OK)
    return RedisModule_ReplyWithError(
        ctx, "<ttl> must be a valid integer that represents seconds");
  if (ttl < 0)
    return RedisModule_ReplyWithError(
        ctx, "<ttl> must be a valid integer that represents seconds");

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

  // Set the TTL for the signature key.
  RedisModule_SetExpire(redisKey, ttl * 1000);
  RedisModule_CloseKey(redisKey);

  // Set the TTL for the payloads key if it exists.
  RedisModuleString *sessionPayloadsKeyStr =
      RedisModule_CreateStringPrintf(ctx, "sg-session:%s:payloads", sessionId);
  redisKey = RedisModule_OpenKey(ctx, sessionPayloadsKeyStr, REDISMODULE_WRITE);
  if (RedisModule_KeyType(redisKey) == REDISMODULE_KEYTYPE_HASH)
    RedisModule_SetExpire(redisKey, ttl * 1000);

  RedisModule_CloseKey(redisKey);

  RedisModule_ReplyWithSimpleString(ctx, "OK");
  return REDISMODULE_OK;
}
