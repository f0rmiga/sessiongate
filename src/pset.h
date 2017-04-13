#pragma once

#include <string.h>

#include "redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.pset <sign_key> <token> <payload_name> <payload_data>
 * Sets a payload of a session.
 * Returns OK.
 */
int PSetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
