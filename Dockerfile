FROM mkaichen/bazel_cpp:latest

ENV APP_DIR /usr/src/cppkg

RUN mkdir -p $APP_DIR
WORKDIR $APP_DIR

COPY . $APP_DIR
RUN apt-get update && apt-get install -y curl

ENTRYPOINT [ "./scripts/tests.sh", "." ]
