FROM gitpod/workspace-full

USER root

COPY --from=havogt/clangd-indexer:latest /build/llvm-project/llvm/build/bin/clangd /usr/bin/clangd-10-rc1
COPY --from=havogt/clangd-indexer:latest /build/llvm-project/llvm/build/bin/clangd-indexer /usr/bin/clangd-indexer-10-rc1

USER gitpod
