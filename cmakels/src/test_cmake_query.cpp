#include "cmake_query.hpp"

int main(int argc, char *argv[]) {
  cmake_query::cmake_query query{"", "build"};
  query.configure();
}
