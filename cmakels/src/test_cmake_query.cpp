/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cmake_query.hpp"

int main(int argc, char *argv[]) {
  cmake_query::cmake_query query{"", "build"};
  query.configure();
}
