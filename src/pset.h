//
// Created by Thulio Ferraz Assis on 2017-02-28.
//

#ifndef SESSIONGATE_PSET_H
#define SESSIONGATE_PSET_H


#include "../thirdparty/redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.pset <sign_key> <token> <payload_name> <payload_data>
 * Sets a payload of a session.
 * Returns OK.
 */
int PSetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSIONGATE_PSET_H
