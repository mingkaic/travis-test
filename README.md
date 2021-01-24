# C++ pkg
[![Build Status](https://travis-ci.org/mingkaic/cppkg.svg?branch=master)](https://travis-ci.org/mingkaic/cppkg)
[![codecov](https://codecov.io/gh/mingkaic/cppkg/branch/master/graph/badge.svg)](https://codecov.io/gh/mingkaic/cppkg)

Mimic Golang pkg libraries to simplify application development

# Dependencies

## DIFF, ERROR, ESTD, EXAM, FMTS, JOBS, LOGS, and TYPES

These libraries don't depend on anything

## FLAG

This library depends on boost

## EGRPC

This library depends on grpc and protobuf

# Conan installation
Before install package first add remote: `conan remote add mingkaic-co "https://gitlab.com/api/v4/projects/23299689/packages/conan"`
Add requirement `cppkg/<version>@mingkaic-co/stable`

trigger yaml
