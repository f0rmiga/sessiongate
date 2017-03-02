//
// Created by Thulio Ferraz Assis on 2017-02-27.
//

#include "start.h"

int StartCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 3) return RedisModule_WrongArity(ctx);
    RedisModule_AutoMemory(ctx);

    size_t keyLen;
    auto keyStr = RedisModule_StringPtrLen(argv[1], &keyLen);
    if (std::string(keyStr, keyLen) == "") {
        return RedisModule_ReplyWithError(ctx, "<sign_key> must have at least one character");
    }

    int64_t ttl;
    if (RedisModule_StringToLongLong(argv[2], &ttl) != REDISMODULE_OK) {
        return RedisModule_ReplyWithError(ctx, "<ttl> must be a valid integer that represents seconds");
    } else if (ttl < 0) {
        return RedisModule_ReplyWithError(ctx, "<ttl> must be a valid integer that represents seconds");
    }

    std::string sessionId;
    RedisModuleString *sessionIdRedisString;
    // A security measure to ensure no collisions will happen to existing session IDs
    while (true) {
        // Generate a session ID
        sessionId = pseudoRandomStr(32);
        sessionIdRedisString = RedisModule_CreateStringPrintf(ctx, "session:%s", sessionId.c_str());

        // Verify if the session ID already exists
        auto existsRep = RedisModule_Call(ctx, "EXISTS", "s", sessionIdRedisString);
        if (RedisModule_CallReplyType(existsRep) != REDISMODULE_REPLY_INTEGER) {
            return RedisModule_ReplyWithError(ctx, "error while checking the session ID");
        }
        bool exists = (bool) RedisModule_CallReplyInteger(existsRep);
        if (exists) continue;

        break;
    }

    // Generate the token signature
    std::string signature;
    try {
        auto key = (const byte *) keyStr;
        signature = signStr(key, keyLen, sessionId.c_str());
    } catch (const CryptoPP::Exception &e) {
        return RedisModule_ReplyWithError(ctx, "error while signing the token");
    }

    // Set a key containing the signature
    auto sessionIdSignatureRedisString = RedisModule_CreateStringPrintf(ctx, "signature:%s", sessionId.c_str());
    auto hsetRep = RedisModule_Call(ctx, "HSETNX", "ssc", sessionIdRedisString, sessionIdSignatureRedisString, signature.c_str());
    if (RedisModule_CallReplyType(hsetRep) == REDISMODULE_REPLY_ERROR) {
        return RedisModule_ReplyWithError(ctx, "error while creating the session");
    }

    // If <ttl> is set to more than 0, fire the Redis command to expire it
    if (ttl > 0) {
        auto expireRep = RedisModule_Call(ctx, "EXPIRE", "sl", sessionIdRedisString, ttl);
        if (RedisModule_CallReplyType(expireRep) != REDISMODULE_REPLY_INTEGER) {
            return RedisModule_ReplyWithError(ctx, "error while setting the session TTL");
        }
    }

    // Return the token in the format <token version>.<session id>.<token signature>
    return RedisModule_ReplyWithString(ctx, RedisModule_CreateStringPrintf(ctx, "%s.%s.%s", TOKEN_VERSION, sessionId.c_str(), signature.c_str()));
}
