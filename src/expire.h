//
// Created by Thulio Ferraz Assis on 2017-03-01.
//

#ifndef SESSION_GATE_EXPIRE_H
#define SESSION_GATE_EXPIRE_H


#include "../thirdparty/redismodule.h"

#include "utils.h"

/*
 * sessiongate.expire <sign_key> <token> <ttl>
 * Sets the TTL of a session.
 * Returns OK.
 */
int ExpireCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSION_GATE_EXPIRE_H
