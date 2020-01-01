#pragma once

#include <memory>
#include <string>

namespace lscpp {

class transporter {
  struct iface {
    virtual ~iface(){};
    [[deprecated("use read_line() instead")]] virtual char read_char() = 0;
    virtual std::string read_line() = 0;
    virtual std::string read_message(std::size_t length) = 0;
    virtual void write_message(std::string str) = 0;
  };

  template <class Transporter> struct impl : iface {
    Transporter t_;
    impl(Transporter t) : t_(std::move(t)) {}
    char read_char() override { return t_.read_char(); };
    std::string read_line() override { return t_.read_line(); };
    std::string read_message(std::size_t length) override {
      return t_.read_message(length);
    };
    void write_message(std::string str) override { t_.write_message(str); };
  };

  std::unique_ptr<iface> impl_;

public:
  template <class Transporter>
  transporter(Transporter t) : impl_(new impl<Transporter>(std::move(t))) {}
  char read_char() { return impl_->read_char(); }
  std::string read_line() { return impl_->read_line(); }
  std::string read_message(std::size_t length) {
    return impl_->read_message(length);
  }
  void write_message(std::string str) { return impl_->write_message(str); }
};

} // namespace lscpp
