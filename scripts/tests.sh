#!/usr/bin/env bash

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )";
COV_DIR="$THIS_DIR";

CONTEXT=$(cd "$1" && pwd);

COVERAGE_CTX="$CONTEXT/tmp/cppkg_coverage";
CONVERSION_CSV="$CONTEXT/tmp/cppkg_conversion.csv";
TMP_COVFILE="$COV_DIR/coverage.info"
OUT_COVFILE="$COV_DIR/labelled_coverage.info";

rm -Rf "$COVERAGE_CTX";
mkdir -p "$COVERAGE_CTX";
find $CONTEXT -maxdepth 1 | grep -E -v 'tmp|\.git|bazel-' | tail -n +2 | xargs -i cp -r {} $COVERAGE_CTX;
find $COVERAGE_CTX | grep -E '\.cpp|\.hpp' | python3 "$THIS_DIR/label_uniquify.py" $COVERAGE_CTX > $CONVERSION_CSV;
find $COVERAGE_CTX | grep -E '\.yml' | python3 "$THIS_DIR/yaml_replace.py" $CONVERSION_CSV;

cd "$COVERAGE_CTX";
lcov --base-directory . --directory . --zerocounters;
set -e

echo "===== ENVIRONMENT =====";
if [ -x "$(command -v free)" ]; then
	free -m;
fi

# ===== Run Gtest =====
echo "===== TESTS =====";

source "$THIS_DIR/coverage.sh";

bzl_coverage //...

python3 "$THIS_DIR/label_replace.py" $TMP_COVFILE $CONVERSION_CSV > $OUT_COVFILE;
cd "$CONTEXT";

echo "";
echo "============ CPPKG TEST SUCCESSFUL ============";
