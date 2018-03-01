package(default_visibility = ["//visibility:private"])

cc_library(
    name = "sessiongate",
    srcs = glob(["src/*.c"]),
    hdrs = glob(["src/*.h"]),
    copts = ["-std=c11"],
    deps = [
        "@libsodium//:sodium",
        "@redis_modules_sdk//:redis_modules_sdk",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "sessiongate.so",
    linkshared = 1,
    deps = [
        ":sessiongate",
    ],
)

load("@test_deps//:requirements.bzl", "requirement")

py_test(
    name = "sessiongate_test",
    size = "small",
    srcs = [
        "tests/all.py",
    ],
    data = [
        ":sessiongate.so",
    ],
    main = "tests/all.py",
    deps = [
        requirement("rmtest"),
    ],
)
