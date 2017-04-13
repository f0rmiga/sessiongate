#pragma once

#include <string.h>

#include "redismodule.h"

#include "consts.h"
#include "utils.h"

/*
 * sessiongate.start <sign_key> <ttl>
 * Starts a new session that will live for <ttl> seconds.
 * Returns a token containing the token version, the session id and the signature.
 */
int StartCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
