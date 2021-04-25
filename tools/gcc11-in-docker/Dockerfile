FROM ubuntu:21.04

ARG USER_ID=61000
ARG GROUP_ID=61000
ARG USERNAME=gcc

RUN apt-get update  -y \
 && apt-get install -y g++-11

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