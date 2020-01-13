# lscpp

A C++ implementation of the language server protocol intended to be consumed by language servers implemented in C++.

## Why?

The idea of lscpp is to provide an lightweight, easy to use language server protocol implementation in C++. There exist some implementations, however they are typically integrated in the respective language server they are implementing, e.g. [clangd](https://github.com/llvm/llvm-project/tree/master/clang-tools-extra/clangd).

## Should I implement my language server in C++

Most likely not. If you are starting from scratch with no dependency, you should most likely use the [TypeScript language server](https://github.com/Microsoft/vscode-languageserver-node/). However, if you have an existing codebase in C++ which implements parsing and analysis for your target language, you might want to consider using lscpp.

## State of the project

The project is in very early state. The language server protocol is very incomplete. Only messages used by [cmakels](../cmakels/README.md) are implemented.

The initial goal of this project is to bring it in a state that is easy extend and maintain.

## Contributing

Contributions are welcome. Please follow the style of existing code and apply clang-format.

## License

lscpp is published under the BSD-3-clause license (SPDX short identifier: `BSD-3-Clause`)
