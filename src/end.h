//
// Created by Thulio Ferraz Assis on 2017-02-27.
//

#ifndef SESSION_GATE_END_H
#define SESSION_GATE_END_H


#include "../thirdparty/redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.end <sign_key> <token>
 * Ends a session.
 * Returns OK.
 */
int EndCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSION_GATE_END_H
