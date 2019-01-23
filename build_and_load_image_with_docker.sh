#!/bin/sh

project_mount="/home/sessiongate"

# Enables Docker in Docker and mounts the project inside the container.
docker run -it \
  -v /var/run/docker.sock:/var/run/docker.sock \
  -v $(git rev-parse --show-toplevel):"${project_mount}" \
  thulioassis/sessiongate-ci \
  /bin/sh -c "cd ${project_mount}; bazel run //:sessiongate_image"
