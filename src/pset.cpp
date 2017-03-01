//
// Created by Thulio Ferraz Assis on 2017-02-28.
//

#include "pset.h"

int PSetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 5) return RedisModule_WrongArity(ctx);
    RedisModule_AutoMemory(ctx);

    // Parse and validate the token
    size_t signKeyLen;
    auto signKey = (const byte *) RedisModule_StringPtrLen(argv[1], &signKeyLen);
    size_t tokenLen;
    auto token = RedisModule_StringPtrLen(argv[2], &tokenLen);
    RedisModuleString *sessionIdRedisString;
    auto err = parseAndValidateToken(ctx, signKey, signKeyLen, token, tokenLen, &sessionIdRedisString);
    if (err != NULL) return RedisModule_ReplyWithError(ctx, err);

    // Set the payload
    auto payloadName = argv[3];
    auto payload = argv[4];
    auto hsetRep = RedisModule_Call(ctx, "HSET", "sss", sessionIdRedisString, payloadName, payload);
    if (RedisModule_CallReplyType(hsetRep) == REDISMODULE_REPLY_ERROR) {
        return RedisModule_ReplyWithError(ctx, "error while setting the session payload");
    }

    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}