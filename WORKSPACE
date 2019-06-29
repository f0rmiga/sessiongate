workspace(name = "sessiongate")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib",
    sha256 = "2ef429f5d7ce7111263289644d233707dba35e39696377ebab8b0bc701f7818e",
    urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/0.8.0/bazel-skylib.0.8.0.tar.gz"],
)

# Python rules.
http_archive(
    name = "io_bazel_rules_python",
    sha256 = "9a3d71e348da504a9c4c5e8abd4cb822f7afb32c613dc6ee8b8535333a81a938",
    strip_prefix = "rules_python-fdbb17a4118a1728d19e638a5291b4c4266ea5b8",
    url = "https://github.com/bazelbuild/rules_python/archive/fdbb17a4118a1728d19e638a5291b4c4266ea5b8.tar.gz",
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
    shallow_since = "1503497108 +0300",
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
    sha256 = "87fc6a2b128147a0a3039a2fd0b53cc1f2ed5adb8716f50756544a572999ae9a",
    strip_prefix = "rules_docker-0.8.1",
    urls = ["https://github.com/bazelbuild/rules_docker/archive/v0.8.1.tar.gz"],
)

load(
    "@io_bazel_rules_docker//repositories:repositories.bzl",
    container_repositories = "repositories",
)

container_repositories()

load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_pull",
)

container_pull(
    name = "redis",
    registry = "index.docker.io",
    repository = "library/redis",
    tag = "5.0.5-stretch",
    digest = "sha256:adcf62f378efe1187d2f72c6f0ecdf86ab2173a9e1c3c9f4fe4bb89060f5362f",
)
