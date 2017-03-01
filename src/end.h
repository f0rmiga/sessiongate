//
// Created by Thulio Ferraz Assis on 2017-02-27.
//

#ifndef SESSIONGATE_END_H
#define SESSIONGATE_END_H


#include "../thirdparty/redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.end <sign_key> <token>
 * Ends a session.
 * Returns OK.
 */
int EndCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSIONGATE_END_H
