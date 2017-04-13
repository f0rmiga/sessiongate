#pragma once

#include <string.h>

#include "redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.pdel <sign_key> <token> <payload_name>
 * Deletes a payload of a session.
 * Returns OK.
 */
int PDelCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
