#!/bin/sh

image_name="thulioassis/sessiongate-ci:0.1.0"

docker build -t $image_name .
