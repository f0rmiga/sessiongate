#include "redismodule.h"
#include "rmutil/util.h"

#include "end.h"
#include "pset.h"
#include "pget.h"
#include "start.h"
#include "utils.h"

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) __attribute__((visibility("default")));
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);

    // Register the module
    if (RedisModule_Init(ctx, "sessiongate", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    // Register functions
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.start", StartCommand, "write");
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.end", EndCommand, "write");
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.pset", PSetCommand, "write");
    RMUtil_RegisterWriteCmd(ctx, "sessiongate.pget", PGetCommand, "readonly");
    // RMUtil_RegisterWriteCmd(ctx, "sessiongate.pdel", PDelCommand, "write");
    // RMUtil_RegisterWriteCmd(ctx, "sessiongate.expire", ExpireCommand, "write");

    return REDISMODULE_OK;
}
