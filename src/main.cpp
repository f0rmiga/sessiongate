//
// Created by Thulio Ferraz Assis on 2017-02-26.
//

#include "../thirdparty/redismodule.h"
#include "../thirdparty/rmutil/util.h"

#include "start.h"
#include "end.h"
#include "pset.h"
#include "pget.h"
#include "pdel.h"
#include "expire.h"

extern "C" int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    // Register the module itself
    if (RedisModule_Init(ctx, "sessiongate", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    // Register functions
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.start", StartCommand, "write");
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.end", EndCommand, "write");
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.pset", PSetCommand, "write");
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.pget", PGetCommand, "readonly");
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.pdel", PDelCommand, "write");
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.expire", ExpireCommand, "write");

    return REDISMODULE_OK;
}
