//
// Created by Thulio Ferraz Assis on 2017-03-01.
//

#ifndef SESSIONGATE_EXPIRE_H
#define SESSIONGATE_EXPIRE_H


#include "../thirdparty/redismodule.h"

#include "utils.h"

/*
 * sessiongate.expire <sign_key> <token> <ttl>
 * Sets the TTL of a session.
 * Returns OK.
 */
int ExpireCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSIONGATE_EXPIRE_H
