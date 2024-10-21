FROM ubuntu:18.04

MAINTAINER tunm<tunmxy@163.com>

# timezone
RUN ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
RUN echo 'Asia/Shanghai' >/etc/timezone


RUN apt-get update
RUN apt-get install cmake -y
RUN apt-get install build-essential -y
