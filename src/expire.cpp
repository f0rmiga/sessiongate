//
// Created by Thulio Ferraz Assis on 2017-03-01.
//

#include "expire.h"

int ExpireCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 4) return RedisModule_WrongArity(ctx);
    RedisModule_AutoMemory(ctx);

    long long ttl;
    if (RedisModule_StringToLongLong(argv[3], &ttl) != REDISMODULE_OK) {
        return RedisModule_ReplyWithError(ctx, "<ttl> must be a valid integer that represents seconds");
    } else if (ttl < 0) {
        return RedisModule_ReplyWithError(ctx, "<ttl> must be a valid integer that represents seconds");
    }

    // Parse and validate the token
    size_t signKeyLen;
    auto signKey = (const byte *) RedisModule_StringPtrLen(argv[1], &signKeyLen);
    size_t tokenLen;
    auto token = RedisModule_StringPtrLen(argv[2], &tokenLen);
    RedisModuleString *sessionIdRedisString;
    auto err = parseAndValidateToken(ctx, signKey, signKeyLen, token, tokenLen, &sessionIdRedisString);
    if (err != NULL) return RedisModule_ReplyWithError(ctx, err);

    // Set the TTL
    if (ttl > 0) {
        auto expireRep = RedisModule_Call(ctx, "EXPIRE", "sl", sessionIdRedisString, ttl);
        if (RedisModule_CallReplyType(expireRep) != REDISMODULE_REPLY_INTEGER) {
            return RedisModule_ReplyWithError(ctx, "error while setting the session TTL");
        }
    } else {
        auto expireRep = RedisModule_Call(ctx, "PERSIST", "s", sessionIdRedisString);
        if (RedisModule_CallReplyType(expireRep) != REDISMODULE_REPLY_INTEGER) {
            return RedisModule_ReplyWithError(ctx, "error while setting the session TTL");
        }
    }

    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}
