FROM gitpod/workspace-full

USER root
RUN apt-get install -yq clang-tools \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/*
