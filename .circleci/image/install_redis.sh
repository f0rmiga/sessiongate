#!/bin/sh

set -o errexit

version="$1"

printf "Downloading Redis ${version}..."
tmp_redis=$(mktemp --directory -t redis.XXXXXXXX)
curl \
  -L https://github.com/antirez/redis/archive/${version}.tar.gz \
  2> /dev/null \
  | tar zx -C $tmp_redis
printf " ok\n"

printf "Compiling Redis ${version}..."
{
  cd "${tmp_redis}/redis-${version}"
  make -j4 > /dev/null 2>&1
}
printf " ok\n"

printf "Installing Redis ${version}..."
{
  cd "${tmp_redis}/redis-${version}"
  make install > /dev/null 2>&1
}
printf " ok\n"
