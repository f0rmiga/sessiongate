package(default_visibility = ["//visibility:public"])

load("@bazel_gazelle//:def.bzl", "gazelle")
load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_bundle",
    "container_image",
    "container_push",
)

gazelle(
    name = "gazelle",
    prefix = "github.com/f0rmiga/sessiongate",
)

cc_library(
    name = "sessiongate",
    srcs = [
        "src/end.c",
        "src/expire.c",
        "src/module.c",
        "src/pdel.c",
        "src/pget.c",
        "src/pset.c",
        "src/start.c",
        "src/utils.c",
    ],
    hdrs = [
        "src/consts.h",
        "src/end.h",
        "src/expire.h",
        "src/pdel.h",
        "src/pget.h",
        "src/pset.h",
        "src/start.h",
        "src/utils.h",
    ],
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
        "--loadmodule",
        "/sessiongate.so",
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
