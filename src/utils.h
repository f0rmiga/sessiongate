//
// Created by Thulio Ferraz Assis on 2017-02-27.
//

#ifndef SESSIONGATE_UTILS_H
#define SESSIONGATE_UTILS_H


#include <boost/algorithm/string.hpp>

#include <cryptopp/filters.h>
#include <cryptopp/hmac.h>
#include <cryptopp/rdrand.h>
#include <cryptopp/secblock.h>
#include <cryptopp/sha.h>

#include "../thirdparty/redismodule.h"

#include "consts.h"

std::string pseudoRandomStr(const size_t size, const std::string &dictionary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

std::string convertToDictionary(std::string &mac, const std::string &dictionary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

std::string signStr(const byte *key, const size_t keyLength, const char *str);

const char *parseAndValidateToken(RedisModuleCtx *ctx, const byte *signKey, const size_t signKeyLen, const char *token, const size_t tokenLen, RedisModuleString **sessionIdRedisString);


#endif //SESSIONGATE_UTILS_H
