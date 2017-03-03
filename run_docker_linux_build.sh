#!/bin/sh

docker build -f docker/compile-linux/ubuntu-16.10/Dockerfile -t libsessiongate .
docker run --rm -w /sessiongate -v $(pwd):/sessiongate -i -t libsessiongate
