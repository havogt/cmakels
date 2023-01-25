/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <memory>
#include <string>

namespace lscpp {

/**
 * Transporter concept
 * A transporter needs to provide the following member functions
 * - read_line(): reads a line, terminated by \r\n.
 *   (\r\n is not part of the return string)
 * - read_message(std::size_t length): reads a string of size `length`.
 *   As the size of the message is known the read can be implemented more
 *   efficiently than read_line()
 * - write_message(std::string const&, bool newline): writes the string,
     adds line ending (\r\n) if `newline == true`.
 *
 * Transporters are responsible for dealing with line endings (which might be
 * platform specific).
 */
class transporter {
  struct iface {
    virtual ~iface(){};
    virtual std::string read_line() = 0;
    virtual std::string read_message(std::size_t length) = 0;
    virtual void write_message(std::string const &str, bool newline) = 0;
  };

  template <class Transporter> struct impl : iface {
    Transporter t_;
    impl(Transporter t) : t_(std::move(t)) {}
    std::string read_line() override { return t_.read_line(); };
    std::string read_message(std::size_t length) override {
      return t_.read_message(length);
    };
    void write_message(std::string const &str, bool newline) override {
      t_.write_message(str, newline);
    };
  };

  std::unique_ptr<iface> impl_;

public:
  template <class Transporter>
  transporter(Transporter t) : impl_(new impl<Transporter>(std::move(t))) {}
  std::string read_line() { return impl_->read_line(); }
  std::string read_message(std::size_t length) {
    return impl_->read_message(length);
  }
  void write_message(std::string const &str, bool newline = false) {
    return impl_->write_message(str, newline);
  }
};

} // namespace lscpp
