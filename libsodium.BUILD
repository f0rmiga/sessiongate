package(default_visibility = ["//visibility:public"])

genrule(
    name = "configure_sh",
    outs = [
        "configure.sh",
    ],
    cmd = """
cat > $@ <<"EOF"
#! /bin/sh
./external/libsodium/configure >> /dev/null
EOF"""
)

filegroup(
    name = "all",
    srcs = glob(["**/*"])
)

VERSION_H_PATH = "src/libsodium/include/sodium/version.h"
genrule(
    name = "version_h",
    srcs = [
        ":all",
    ],
    outs = [
        VERSION_H_PATH,
    ],
    cmd = "$(location :configure_sh) && cat {} > $(@)".format(VERSION_H_PATH),
    tools = [":configure_sh"],
)

filegroup(
    name = "headers",
    srcs = glob(
        ["**/*.h"],
        exclude = ["test/**/*"],
    ),
)

filegroup(
    name = "srcs",
    srcs = glob(
        ["**/*.c"],
        exclude = ["test/**/*"],
    ),
)

cc_library(
    name = "sodium",
    hdrs = [
        ":headers",
        ":version_h",
    ],
    srcs = [
        ":srcs",
    ],
    includes = [
        "src/libsodium/include",
        "src/libsodium/include/sodium",
    ],
    copts = [
        "-Wno-cpp",
        "-Wno-unknown-pragmas",
        "-Wno-unknown-warning-option",
        "-Wno-unused-function",
        "-Wno-unused-value",
        "-Wno-unused-variable",
        "-Wno-#warnings",
    ],
)

cc_binary(
    name = "libsodium",
    deps = [":sodium"],
    linkstatic = 1,
)
