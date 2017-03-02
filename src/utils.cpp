//
// Created by Thulio Ferraz Assis on 2017-02-27.
//

#include "utils.h"

CryptoPP::RDRAND prng;

std::string pseudoRandomStr(const size_t size, const std::string &dictionary) {
    CryptoPP::SecByteBlock randomBytes(size);
    prng.GenerateBlock(randomBytes, randomBytes.size());
    auto dictionarySize = dictionary.size();
    std::string result;
    result.reserve(size);
    for (CryptoPP::SecByteBlock::iterator it = randomBytes.begin(); it != randomBytes.end(); it++) result += dictionary[*it % dictionarySize];
    return result;
}

std::string convertToDictionary(std::string &mac, const std::string &dictionary) {
    auto dictionarySize = dictionary.size();
    std::string result;
    result.reserve(mac.length());
    for (std::string::iterator it = mac.begin(); it != mac.end(); it++) result += dictionary[*it % dictionarySize];
    return result;
}

std::string signStr(const byte *key, const size_t keyLength, const char *str) {
    std::string mac;
    CryptoPP::HMAC<CryptoPP::SHA512> hmac(key, keyLength);
    CryptoPP::StringSource(str, true, new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(mac)));
    return convertToDictionary(mac);
}

const char *parseAndValidateToken(RedisModuleCtx *ctx, const byte *signKey, const size_t signKeyLen, const char *token, const size_t tokenLen, RedisModuleString **sessionIdRedisString) {
    std::vector<std::string> parts(3);
    const auto tokenStr = std::string(token, tokenLen);
    boost::split(parts, tokenStr, boost::is_any_of("."));
    if (parts.size() < 3) return "the token format is invalid";
    for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); it++) {
        if (*it == "") return "the token format is invalid";
    }
    auto tokenVersion = parts[0];
    auto sessionId = parts[1];
    auto signature = parts[2];

    // Generate the token signature from the sessionId to compare to the provided signature
    std::string compareSignature;
    try {
        compareSignature = signStr(signKey, signKeyLen, sessionId.c_str());
    } catch (const CryptoPP::Exception &e) {
        return "error while signing the token";
    }
    if (signature != compareSignature) return "invalid token: the signatures do not match";

    // Check if the token still exists
    *sessionIdRedisString = RedisModule_CreateStringPrintf(ctx, "session:%s", sessionId.c_str());
    auto existsRep = RedisModule_Call(ctx, "EXISTS", "s", *sessionIdRedisString);
    if (RedisModule_CallReplyType(existsRep) != REDISMODULE_REPLY_INTEGER) {
        return "error while checking the session ID";
    }
    bool exists = (bool) RedisModule_CallReplyInteger(existsRep);
    if (!exists) return "the token is expired";

    // Check if the stored signature matches the provided signature
    auto sessionIdSignatureRedisString = RedisModule_CreateStringPrintf(ctx, "signature:%s", sessionId.c_str());
    auto hgetSignatureRep = RedisModule_Call(ctx, "HGET", "ss", *sessionIdRedisString, sessionIdSignatureRedisString);
    if (RedisModule_CallReplyType(hgetSignatureRep) != REDISMODULE_REPLY_STRING) {
        return "error while getting the stored session signature";
    }
    size_t storedSignatureLen;
    auto storedSignatureChar = RedisModule_CallReplyStringPtr(hgetSignatureRep, &storedSignatureLen);
    auto storedSignature = std::string(storedSignatureChar, storedSignatureLen);
    if (signature != storedSignature) return "invalid token: this token was signed using other <sign_key>";

    // Check if the token versions match
    if (tokenVersion != TOKEN_VERSION) return "invalid token: wrong token version";

    return NULL;
}