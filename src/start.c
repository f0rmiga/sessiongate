#include "start.h"

int StartCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  if (argc != 3)
    return RedisModule_WrongArity(ctx);
  RedisModule_AutoMemory(ctx);

  // Extract <sign_key> and validate it.
  size_t signKeyLen;
  const char *signKey = RedisModule_StringPtrLen(argv[1], &signKeyLen);
  if (signKeyLen == 0)
    return RedisModule_ReplyWithError(
        ctx, "<sign_key> must have at least one character");

  // Extract <ttl> and validate it.
  long long ttl;
  if (RedisModule_StringToLongLong(argv[2], &ttl) != REDISMODULE_OK)
    return RedisModule_ReplyWithError(
        ctx, "<ttl> must be a valid integer that represents seconds");
  if (ttl < 0)
    return RedisModule_ReplyWithError(
        ctx, "<ttl> must be a valid integer that represents seconds");

  char sessionId[SESSION_ID_STRLEN + 1] = {0};
  RedisModuleString *sessionSignatureKeyStr;
  RedisModuleKey *redisKey;
  // A security measure to ensure no collisions will happen to existing session
  // IDs. Open a ticket and call me paranoid. You better like me being paranoid.
  while (1) {
    // Generate the session ID.
    generatePseudoRandomString(sessionId);
    sessionSignatureKeyStr = RedisModule_CreateStringPrintf(
        ctx, "sg-session:%s:signature", sessionId);

    // Verify if the session ID already exists.
    redisKey =
        RedisModule_OpenKey(ctx, sessionSignatureKeyStr, REDISMODULE_WRITE);
    if (RedisModule_KeyType(redisKey) == REDISMODULE_KEYTYPE_EMPTY)
      break;
  }

  char signature[SIGNATURE_STRLEN + 1];
  signData((const unsigned char *)signKey, signKeyLen,
           (const unsigned char *)sessionId, SESSION_ID_STRLEN, signature);
  RedisModule_StringSet(redisKey,
                        RedisModule_CreateStringPrintf(ctx, signature));

  // Set the session TTL.
  RedisModule_SetExpire(redisKey, ttl * 1000);

  RedisModule_CloseKey(redisKey);

  RedisModule_ReplyWithString(
      ctx, RedisModule_CreateStringPrintf(ctx, "%s.%s.%s", TOKEN_VERSION,
                                          sessionId, signature));
  return REDISMODULE_OK;
}
