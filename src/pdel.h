//
// Created by Thulio Ferraz Assis on 2017-02-28.
//

#ifndef SESSIONGATE_PDEL_H
#define SESSIONGATE_PDEL_H


#include "../thirdparty/redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.pdel <sign_key> <token> <payload_name>
 * Deletes a payload of a session.
 * Returns OK.
 */
int PDelCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSIONGATE_PDEL_H
