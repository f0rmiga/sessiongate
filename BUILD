package(default_visibility = ["//visibility:private"])

load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_bundle",
    "container_image",
    "container_push",
)
load("@test_deps//:requirements.bzl", "requirement")

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
    linkshared = True,
    deps = [
        ":sessiongate",
    ],
)

container_image(
    name = "_sessiongate_image",
    base = "@redis//image",
    files = [":sessiongate.so"],
    cmd = [
        "redis-server",
        "--loadmodule", "/sessiongate.so",
    ],
)

container_bundle(
    name = "sessiongate_image",
    images = {
        "thulioassis/sessiongate:latest": ":_sessiongate_image",
    },
)

container_push(
    name = "push_sessiongate_image",
    image = ":_sessiongate_image",
    format = "Docker",
    registry = "index.docker.io",
    repository = "thulioassis/sessiongate",
    tag = "latest",
)

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
