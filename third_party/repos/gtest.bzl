load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def gtest_repository(name):
    git_repository(
        name = name,
        remote = "https://github.com/google/googletest",
        commit = "ca4b7c9ff4d8a5c37ac51795b03ffe934958aeff",
    )
