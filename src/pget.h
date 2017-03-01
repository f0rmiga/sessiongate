//
// Created by Thulio Ferraz Assis on 2017-02-28.
//

#ifndef SESSION_GATE_PGET_H
#define SESSION_GATE_PGET_H


#include <string>

#include "../thirdparty/redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.pget <sign_key> <token> <payload_name>
 * Gets a payload of a session.
 * Returns the payload data.
 */
int PGetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSION_GATE_PGET_H
