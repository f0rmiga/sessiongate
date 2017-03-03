FROM ubuntu:16.10

RUN apt-get update -qq -y
RUN apt-get upgrade -qq -y
RUN apt-get install -qq libboost-dev gcc g++ cmake -y
RUN apt-get install -qq wget -y

RUN wget https://github.com/weidai11/cryptopp/archive/CRYPTOPP_5_6_5.tar.gz && tar zxf CRYPTOPP_5_6_5.tar.gz
RUN rm CRYPTOPP_5_6_5.tar.gz
WORKDIR cryptopp-CRYPTOPP_5_6_5
RUN make && make install

CMD ["/bin/sh", "/sessiongate/docker/compile-linux/ubuntu-16.10/run_build.sh"]
