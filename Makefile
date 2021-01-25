
######## COVERAGES ########

TMP_COVFILE := /tmp/coverage.info
COVERAGE_INFO_FILE := bazel-out/_coverage/_coverage_report.dat
CCOVER := bazel coverage --config asan --action_env="ASAN_OPTIONS=detect_leaks=0" --config gtest --config cc_coverage
COVERAGE_CTX := tmp/cppkg_coverage
CONVERSION_CSV := tmp/cppkg_conversions.csv

.PHONY: cov_clean
cov_clean:
	rm *.info
	rm -Rf html

# coverage helper
.PHONY: cov_init
cov_init:
	rm -Rf tmp
	mkdir -p $(COVERAGE_CTX)
	find . -maxdepth 1 | grep -E -v 'tmp|\.git|bazel-' | tail -n +2 | xargs -i cp -r {} $(COVERAGE_CTX)
	find $(COVERAGE_CTX) | grep -E '\.cpp|\.hpp' | python3 scripts/label_uniquify.py $(COVERAGE_CTX) > $(CONVERSION_CSV)
	find $(COVERAGE_CTX) | grep -E '\.yml' | python3 scripts/yaml_replace.py $(CONVERSION_CSV)

.PHONY: cov_copyout
cov_copyout:
	python3 scripts/label_replace.py $(COVERAGE_CTX)/$(COVFILE) $(CONVERSION_CSV) > $(COVFILE)

.PHONY: cov_genhtml
cov_genhtml: cov_copyout
	genhtml -o html $(COVFILE)

.PHONY: clean_test_coverage
clean_test_coverage: ${COVERAGE_INFO_FILE}
	lcov --remove ${COVERAGE_INFO_FILE} '**/test/*' '**/mock/*' '**/*.pb.*' -o ${TMP_COVFILE}

.PHONY: coverage
coverage:
	$(CCOVER) //...
	@make clean_test_coverage
	lcov --extract ${TMP_COVFILE} 'diff/*' 'egrpc/*' 'error/*' 'estd/*' 'exam/*' 'flag/*' 'fmts/*' 'jobs/*' 'logs/*' 'types/*' -o coverage.info

###### INDIVIDUAL COVERAGES ######

.PHONY: cover_modules
cover_modules: cover_diff cover_egrpc cover_error cover_estd cover_exam cover_flag cover_fmts cover_jobs cover_logs
	lcov \
		-a diff_coverage.info \
		-a egrpc_coverage.info \
		-a error_coverage.info \
		-a estd_coverage.info \
		-a exam_coverage.info \
		-a flag_coverage.info \
		-a fmts_coverage.info \
		-a jobs_coverage.info \
		-a logs_coverage.info \
		-o modules_coverage.info

.PHONY: cover_diff
cover_diff:
	${CCOVER} --instrumentation_filter 'diff/*' //diff:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o diff_coverage.info

.PHONY: cover_egrpc
cover_egrpc:
	${CCOVER} --instrumentation_filter 'egrpc/*' //egrpc:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o egrpc_coverage.info

.PHONY: cover_error
cover_error:
	${CCOVER} --instrumentation_filter 'error/*' //error:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o error_coverage.info

.PHONY: cover_estd
cover_estd:
	${CCOVER} --instrumentation_filter 'estd/*' //estd:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o estd_coverage.info

.PHONY: cover_exam
cover_exam:
	$(CCOVER) --instrumentation_filter 'exam/*' //exam:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o exam_coverage.info

.PHONY: cover_flag
cover_flag:
	$(CCOVER) --instrumentation_filter 'flag/*' //flag:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o flag_coverage.info

.PHONY: cover_fmts
cover_fmts:
	$(CCOVER) --instrumentation_filter 'fmts/*' //fmts:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o fmts_coverage.info

.PHONY: cover_jobs
cover_jobs:
	$(CCOVER) --instrumentation_filter 'jobs/*' //jobs:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o jobs_coverage.info

.PHONY: cover_logs
cover_logs:
	$(CCOVER) --instrumentation_filter 'logs/*' //logs:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o logs_coverage.info

VERSION=$(./get_version.sh)

.PHONY: conan_remote
conan_remote:
	conan remote add inexorgame "https://api.bintray.com/conan/inexorgame/inexor-conan"
	conan remote add mingkaic-co "https://gitlab.com/api/v4/projects/23299689/packages/conan"

build/conanbuildinfo.cmake:
	conan install -if build .

.PHONY: conan_install
conan_install: build/conanbuildinfo.cmake

.PHONY: conan_build
conan_build: build/conanbuildinfo.cmake
	conan build -bf build .

.PHONY: conan_create
conan_create:
	conan create . mingkaic-co/test

.PHONY: conan_upload
conan_upload:
	conan upload cppkg/${VERSION}@mingkaic-co/test --all --remote mingkaic-co

.PHONY: conan_create_n_upload
conan_create_n_upload: conan_create conan_upload
