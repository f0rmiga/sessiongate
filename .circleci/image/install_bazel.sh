#!/bin/sh

set -o errexit

version="$1"

printf "Downloading Bazel ${version}..."
tmp_installer=$(mktemp -t bazel.XXXXXXXX)
curl \
  -o $tmp_installer \
  -L https://github.com/bazelbuild/bazel/releases/download/${version}/bazel-${version}-installer-linux-x86_64.sh \
  2> /dev/null
printf " ok\n"

printf "Installing Bazel ${version}..."
sh $tmp_installer > /dev/null 2>&1
printf " ok\n"
