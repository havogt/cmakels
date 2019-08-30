#!/bin/sh
qtscript=/home/vogtha/git/quicktype/script/quicktype
# qtscript=quicktype
${qtscript} lsp_specification.ts -l c++ -o lsp.hpp --source-style multi-source --code-format with-struct --const-style east-const --no-boost --namespace protocol
