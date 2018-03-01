package(default_visibility = ["//visibility:private"])

cc_library(
    name = "sessiongate",
    srcs = glob(["src/*.c"]),
    hdrs = glob(["src/*.h"]),
    deps = [
        "@libsodium//:sodium",
        "@redis_modules_sdk//:redis_modules_sdk",
    ],
    copts = ["-std=c11"],
    alwayslink = 1,
)

cc_binary(
    name = "sessiongate.so",
    deps = [
        ":sessiongate",
    ],
    linkshared = 1,
)

load("@test_deps//:requirements.bzl", "requirement")

py_test(
    name = "sessiongate_test",
    srcs = [
        "tests/all.py",
    ],
    main = "tests/all.py",
    data = [
        ":sessiongate.so",
    ],
    deps = [
        requirement("rmtest"),
    ],
    size = "small",
)
