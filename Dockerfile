FROM ubuntu:18.04

MAINTAINER tunm<tunmxy@163.com>

WORKDIR /work
COPY . /work
COPY 3rdparty_hyper_inspire_op/cmake-3.24.3.tar.gz /root/

# timezone
RUN ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
RUN echo 'Asia/Shanghai' >/etc/timezone


RUN ls 3rdparty_hyper_inspire_op

#    && tar -zxvf cmake-3.24.3.tar.gz \
#    && cd cmake-3.24.3 \
#    && ./bootstrap \
#    && make -j$(nproc) \
#    && sudo make install \
#    && cmake --version