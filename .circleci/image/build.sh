#!/bin/sh

image_name="thulioassis/sessiongate-ci:latest"

docker build -t $image_name .
