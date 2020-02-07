FROM gitpod/workspace-full

USER root

COPY --from=havogt/clangd-indexer:v10.0.0-rc1 /build/llvm-project/llvm/build/bin/clangd /usr/bin/clangd-10-rc1
COPY --from=havogt/clangd-indexer:v10.0.0-rc1 /build/llvm-project/llvm/build/bin/clangd-indexer /usr/bin/clangd-indexer-10-rc1

USER gitpod
