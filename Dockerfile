FROM ubuntu:20.04

RUN apt-get update -q && apt-get -qy install    \
      build-essential                           \
      git-core                                  \
      make

RUN mkdir -p /dmtcp
RUN mkdir -p /tmp

WORKDIR /dmtcp
RUN git clone https://github.com/dmtcp/dmtcp.git /dmtcp && \
      git checkout master &&                    \
      git log -n 1

RUN ./configure --prefix=/usr && make -j 2 && make install

RUN mkdir fork

WORKDIR /fork

COPY . .

EXPOSE 80