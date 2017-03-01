//
// Created by Thulio Ferraz Assis on 2017-02-28.
//

#ifndef SESSION_GATE_PSET_H
#define SESSION_GATE_PSET_H


#include "../thirdparty/redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.pset <sign_key> <token> <payload_name> <payload>
 * Sets a payload of a session.
 * Returns OK.
 */
int PSetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSION_GATE_PSET_H
