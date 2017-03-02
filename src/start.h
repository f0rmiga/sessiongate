//
// Created by Thulio Ferraz Assis on 2017-02-27.
//

#ifndef SESSIONGATE_START_H
#define SESSIONGATE_START_H


#include <string>

#include "../thirdparty/redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.start <sign_key> <ttl>
 * Starts a new session that will live for <ttl> seconds.
 * Returns a token containing the version, session id and signature.
 */
int StartCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


#endif //SESSIONGATE_START_H
