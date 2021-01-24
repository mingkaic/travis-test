load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def grpc_rules_repository():
    http_archive(
        name = "rules_proto_grpc",
        urls = ["https://github.com/rules-proto-grpc/rules_proto_grpc/archive/2.0.0.tar.gz"],
        sha256 = "d771584bbff98698e7cb3cb31c132ee206a972569f4dc8b65acbdd934d156b33",
        strip_prefix = "rules_proto_grpc-2.0.0",
    )
