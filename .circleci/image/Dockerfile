FROM opensuse/leap

RUN zypper update
RUN zypper dup -y
RUN zypper install -y --type pattern devel_basis
RUN zypper install -y \
  curl \
  git \
  java-11-openjdk-devel \
  libstdc++-devel \
  python \
  python-pip \
  unzip \
  which

ADD install_bazel.sh /opt/bin/install_bazel.sh
ADD install_redis.sh /opt/bin/install_redis.sh
RUN /opt/bin/install_bazel.sh 0.19.2
