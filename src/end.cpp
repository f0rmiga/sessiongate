//
// Created by Thulio Ferraz Assis on 2017-02-27.
//

#include "end.h"

int EndCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 3) return RedisModule_WrongArity(ctx);
    RedisModule_AutoMemory(ctx);

    // Parse and validate the token
    size_t signKeyLen;
    auto signKey = (const byte *) RedisModule_StringPtrLen(argv[1], &signKeyLen);
    size_t tokenLen;
    auto token = RedisModule_StringPtrLen(argv[2], &tokenLen);
    RedisModuleString *sessionIdRedisString;
    auto err = parseAndValidateToken(ctx, signKey, signKeyLen, token, tokenLen, &sessionIdRedisString);
    if (err != NULL) return RedisModule_ReplyWithError(ctx, err);

    // Remove the keys associated to the session
    auto delRep = RedisModule_Call(ctx, "DEL", "s", sessionIdRedisString);
    if (RedisModule_CallReplyType(delRep) == REDISMODULE_REPLY_ERROR) {
        return RedisModule_ReplyWithError(ctx, "error while deleting the session");
    }

    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}