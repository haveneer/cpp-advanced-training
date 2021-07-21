# Inspired from https://github.com/JetBrains/clion-remote
# and https://www.jetbrains.com/help/clion/clion-toolchains-in-docker.html

# CLion remote docker environment (How to build docker container, run and stop it)
#
# Build and run:
#   docker build -t clion/remote-cpp-env .
#   docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 --name clion_remote_env clion/remote-cpp-env
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:2222"
#
# stop:
#   docker stop clion_remote_env
# 
# ssh credentials (test user):
#   user@password 

FROM ubuntu:21.04

## preesed tzdata, update package index, upgrade packages and install needed software
RUN export DEBIAN_FRONTEND=noninteractive; \
    export DEBCONF_NONINTERACTIVE_SEEN=true; \
    echo 'tzdata tzdata/Areas select Europe' | debconf-set-selections; \
    echo 'tzdata tzdata/Zones/Europe select Paris' | debconf-set-selections; \ 
    apt-get update && apt-get install -y tzdata
    
RUN apt-get install -y ssh \
      build-essential \
      gcc-11 g++-11 gdb \
      clang-12 lldb-12 libc++-12-dev libc++abi-12-dev \
      clangd-12 clang-tools-12 \
      cmake ninja-build \
      rsync tar \
      git \
      python3 python3-pip \
      software-properties-common \
      libpthread-stubs0-dev libtbb-dev \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*
  
RUN pip3 install conan

RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \
  ) > /etc/ssh/sshd_config_test_clion \
  && mkdir /run/sshd

RUN useradd -m user \
  && yes password | passwd user

RUN usermod -s /bin/bash user

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]