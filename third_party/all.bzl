load("//third_party:repos/boost.bzl", "boost_repository")
load("//third_party:repos/grpc.bzl", "grpc_rules_repository")
load("//third_party:repos/gtest.bzl", "gtest_repository")

def dependencies(excludes = []):
    ignores = native.existing_rules().keys() + excludes

    if "com_github_nelhage_rules_boost" not in ignores:
        boost_repository()

    if "rules_proto_grpc" not in ignores:
        grpc_rules_repository()

    if "gtest" not in ignores:
        gtest_repository(name = "gtest")
