#pragma once

#include <string.h>

#include "redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.end <sign_key> <token>
 * Ends a session.
 * Returns OK.
 */
int EndCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
