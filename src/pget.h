#pragma once

#include <string.h>

#include "redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.pget <sign_key> <token> <payload_name>
 * Gets a payload of a session.
 * Returns <payload_data>.
 */
int PGetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
