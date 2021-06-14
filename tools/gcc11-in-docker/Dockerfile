FROM ubuntu:21.04

ARG USER_ID=61000
ARG GROUP_ID=61000
ARG USERNAME=gcc

RUN export DEBIAN_FRONTEND=noninteractive; \
    export DEBCONF_NONINTERACTIVE_SEEN=true; \
    echo 'tzdata tzdata/Areas select Europe' | debconf-set-selections; \
    echo 'tzdata tzdata/Zones/Europe select Paris' | debconf-set-selections; \ 
    apt-get update && apt-get install -y tzdata;


RUN apt-get install -y build-essential wget m4 flex bison git \
    && apt install -y libgmp-dev libmpfr-dev libmpc-dev 
RUN git clone git://gcc.gnu.org/git/gcc.git
RUN cd gcc && git checkout -b gcc-11 remotes/origin/releases/gcc-11
RUN mkdir build && cd build && ../configure --enable-languages=c,c++ --disable-multilib --prefix=/usr/local --program-suffix=-11 && make -j 8 && make install


COPY entrypoint.sh /

RUN if [ ${USER_ID:-0} -ne 0 ] && [ ${GROUP_ID:-0} -ne 0 ]; then \
      groupadd -g ${GROUP_ID} gcc \
      && useradd -g ${GROUP_ID} -l -M \
         -s /sbin/nologin -u ${USER_ID} gcc \
    ;fi

USER ${USERNAME}

WORKDIR /

ENTRYPOINT ["/entrypoint.sh"]
CMD ["g++","--help"]