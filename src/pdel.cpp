//
// Created by Thulio Ferraz Assis on 2017-02-28.
//

#include "pdel.h"

int PDelCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 4) return RedisModule_WrongArity(ctx);
    RedisModule_AutoMemory(ctx);

    // Parse and validate the token
    size_t signKeyLen;
    auto signKey = (const byte *) RedisModule_StringPtrLen(argv[1], &signKeyLen);
    size_t tokenLen;
    auto token = RedisModule_StringPtrLen(argv[2], &tokenLen);
    RedisModuleString *sessionIdRedisString;
    auto err = parseAndValidateToken(ctx, signKey, signKeyLen, token, tokenLen, &sessionIdRedisString);
    if (err != NULL) return RedisModule_ReplyWithError(ctx, err);

    // Delete the payload
    auto payloadName = argv[3];
    auto hdelRep = RedisModule_Call(ctx, "HDEL", "ss", sessionIdRedisString, payloadName);
    if (RedisModule_CallReplyType(hdelRep) == REDISMODULE_REPLY_ERROR) {
        return RedisModule_ReplyWithError(ctx, "error while deleting the session payload");
    } else if (RedisModule_CallReplyInteger(hdelRep) == 0) {
        return RedisModule_ReplyWithError(ctx, "the requested payload to delete does not exist");
    }

    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}