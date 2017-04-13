#pragma once

#include <string.h>

#include "redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.expire <sign_key> <token> <ttl>
 * Sets the TTL of a session.
 * Returns OK.
 */
int ExpireCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
