# cmakels

A language server for CMake.

## Why?

From the first time I heard about language servers I was very excited by the idea. At the same time I had to work a lot with CMake and was quite disappointed by the existing editing support in IDEs (typically syntax highlighting, auto-completion for keywords and built-in functions, but no features requiring semantics). So I decided to play with a language server for CMake. My idea was to implement the language server in C++ to be able to easily access CMake information by interfacing with CMake's implementation.
Since I couldn't find an existing [LSP]() implementation in C++ which was easy to re-use and I found it very interesting to implement a language server from scratch, I started this project as 2 components: [lscpp](../lscpp/README.md), LSP implementation in C++ (similar to [lsp4j](https://github.com/eclipse/lsp4j) for Java) and cmakels building building on top of it.

## Should I use it?

At this point, cmakels is still in a prototype phase. It provides only minimal features and is very unstable. However, having cmakels enabled in your IDE will usually not harm your developing experience as either cmakels will just not give language support or will even crash (and therefore stop providing language support). I have it enabled for all my development work...

## Features (to be continued...)

- Go to definition for `add_subdirectory()` will jump to the first line in the `CMakeLists.txt` of the referenced folder.
- Hovering a variable will show the value of the variable (has some known issues, e.g. related to issue #32).

## Contributing

Contributions are welcome. Please follow the style of existing code and apply clang-format.

## License

cmakels is published under the BSD-3-clause license (SPDX short identifier: `BSD-3-Clause`)
