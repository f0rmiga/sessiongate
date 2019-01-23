workspace(name = "sessiongate")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

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
    build_file = "//:redis_modules_sdk.BUILD",
    commit = "3e4daab6dcbc881d8c17406235d156de55be4fc7",
    remote = "https://github.com/RedisLabs/RedisModulesSDK.git",
)

libsodium_version = "1.0.17"
http_archive(
    name = "libsodium",
    build_file = "//:libsodium.BUILD",
    sha256 = "0cc3dae33e642cc187b5ceb467e0ad0e1b51dcba577de1190e9ffa17766ac2b1",
    strip_prefix = "libsodium-{}".format(libsodium_version),
    urls = [
        "https://github.com/jedisct1/libsodium/releases/download/{v}/libsodium-{v}.tar.gz".format(
            v = libsodium_version,
        ),
    ],
)

# Docker rules.
http_archive(
    name = "io_bazel_rules_docker",
    sha256 = "c0e9d27e6ca307e4ac0122d3dd1df001b9824373fb6fb8627cd2371068e51fef",
    strip_prefix = "rules_docker-0.6.0",
    urls = ["https://github.com/bazelbuild/rules_docker/archive/v0.6.0.tar.gz"],
)

load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_pull",
    container_repositories = "repositories",
)

container_repositories()

container_pull(
    name = "redis",
    registry = "index.docker.io",
    repository = "library/redis",
    digest = "sha256:64cf18f3875e2a0a63e423fcfb1e082734691f404cd59f1f421575f42f48c595", # 5.0.3-stretch
)
