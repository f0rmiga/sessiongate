#!/bin/sh

set -o errexit

zypper refresh
zypper dup -y
zypper install -y --type pattern devel_basis
zypper install -y \
  curl \
  docker \
  git \
  java-11-openjdk-devel \
  libstdc++-devel \
  python \
  python-pip \
  unzip \
  which
