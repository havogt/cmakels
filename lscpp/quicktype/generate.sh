#!/bin/sh
quicktype lsp_specification.ts -l c++ -o lsp.hpp --source-style multi-source --code-format with-struct --const-style east-const --no-boost --namespace protocol
