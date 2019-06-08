#include <gtest/gtest.h>
#include "../external/json.hpp"

struct initialize_params {};

struct request_message_handler {
  void handle(nlohmann::json const& j) {
    std::string method = j.at("method");

    if (method.compare("initialize") == 0) {
      handle_initialize(initialize_params{});
    } else {
      std::cout << "cannot handle request message" << std::endl;
    }
  }

  virtual void handle_initialize(initialize_params const& p) = 0;
  virtual ~request_message_handler() {}
};

struct message_handler {
  std::unique_ptr<request_message_handler> req_handler_;

  void handle(nlohmann::json const& j) {
    if (j.contains("id")) {
      req_handler_->handle(j);
    } else {
      std::cout << "message ignored" << std::endl;
    }
  }
};

struct my_request_message_handler : public request_message_handler {
  void handle_initialize(initialize_params const& p) {
    std::cout << "handling initialize" << std::endl;
  }
};

TEST(json, simple) {
  //  InitializeParams a{1};
  //  auto params = std::make_unique<InitializeParams>(1);
  //  RequestMessage msg{"", 1, "initialize", std::move(params)};
  //  nlohmann::json j = msg;
  //  std::cout << j.dump() << std::endl;

  nlohmann::json from = {{"id", 1},
                         {"jsonrpc", ""},
                         {"method", "initialize"},
                         {"params", {"processId:", 1}}};
  std::cout << from.dump() << std::endl;

  message_handler hndlr{std::make_unique<my_request_message_handler>()};
  hndlr.handle(from);

  //  auto req = from.get<RequestMessage>();
  //  nlohmann::json paramsback = req.params->json();
  //  std::cout << paramsback.dump() << std::endl;
}
