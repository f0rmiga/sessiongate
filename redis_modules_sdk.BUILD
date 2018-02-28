package(default_visibility = ["//visibility:public"])

filegroup(
    name = "headers",
    srcs = [
        "redismodule.h",
        "rmutil/util.h",
    ],
)

cc_library(
    name = "redis_modules_sdk",
    hdrs = [
        ":headers",
    ],
    includes = [
        ".",
    ],
)
