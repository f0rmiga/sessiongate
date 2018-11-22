workspace(name = "sessiongate")

# Python rules.
git_repository(
    name = "io_bazel_rules_python",
    commit = "115e3a0dab4291184fdcb0d4e564a0328364571a",
    remote = "https://github.com/bazelbuild/rules_python.git",
)

load(
    "@io_bazel_rules_python//python:pip.bzl",
    "pip_repositories",
    "pip_import",
)

pip_repositories()

pip_import(
    name = "test_deps",
    requirements = "//:pip_requirements.txt",
)

load("@test_deps//:requirements.bzl", "pip_install")

pip_install()

# C dependencies.
new_git_repository(
    name = "redis_modules_sdk",
    build_file = "redis_modules_sdk.BUILD",
    commit = "3e4daab6dcbc881d8c17406235d156de55be4fc7",
    remote = "https://github.com/RedisLabs/RedisModulesSDK.git",
)

libsodium_version = "1.0.16"
new_http_archive(
    name = "libsodium",
    build_file = "libsodium.BUILD",
    sha256 = "eeadc7e1e1bcef09680fb4837d448fbdf57224978f865ac1c16745868fbd0533",
    strip_prefix = "libsodium-{}".format(libsodium_version),
    urls = [
        "https://github.com/jedisct1/libsodium/releases/download/{v}/libsodium-{v}.tar.gz".format(
            v = libsodium_version,
        ),
    ],
)
