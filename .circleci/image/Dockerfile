FROM opensuse/leap

ADD install_dependencies.sh /opt/bin/install_dependencies.sh
RUN /opt/bin/install_dependencies.sh

ADD install_bazel.sh /opt/bin/install_bazel.sh
ADD versions/bazel /var/versions/bazel
RUN cat /var/versions/bazel | xargs /opt/bin/install_bazel.sh

ADD install_redis.sh /opt/bin/install_redis.sh
ADD versions/redis_4 /var/versions/redis_4
ADD versions/redis_5 /var/versions/redis_5
