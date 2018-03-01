#include "redismodule.h"
#include "rmutil/util.h"

#include "end.h"
#include "expire.h"
#include "pdel.h"
#include "pget.h"
#include "pset.h"
#include "start.h"
#include "utils.h"

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
    __attribute__((visibility("default")));
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv,
                       int argc) {
  REDISMODULE_NOT_USED(argv);
  REDISMODULE_NOT_USED(argc);

  // Register the module.
  if (RedisModule_Init(ctx, "sessiongate", 1, REDISMODULE_APIVER_1) ==
      REDISMODULE_ERR)
    return REDISMODULE_ERR;

  // Register functions.
  RMUtil_RegisterWriteCmd(ctx, "sessiongate.start", StartCommand);
  RMUtil_RegisterWriteCmd(ctx, "sessiongate.end", EndCommand);
  RMUtil_RegisterWriteCmd(ctx, "sessiongate.pset", PSetCommand);
  RMUtil_RegisterWriteCmd(ctx, "sessiongate.pget", PGetCommand);
  RMUtil_RegisterWriteCmd(ctx, "sessiongate.pdel", PDelCommand);
  RMUtil_RegisterWriteCmd(ctx, "sessiongate.expire", ExpireCommand);

  return REDISMODULE_OK;
}
