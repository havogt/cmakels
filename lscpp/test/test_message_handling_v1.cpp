#include <gtest/gtest.h>
#include "../external/json.hpp"

struct Params {
  virtual nlohmann::json json() = 0;
  virtual ~Params(){};
};

struct InitializeParams : public Params {
  InitializeParams(int processId) : processId(processId) {}
  InitializeParams(nlohmann::json const& j) { from_json(j, *this); }
  int processId;

  nlohmann::json json() {
    nlohmann::json j;
    to_json(j, *this);
    return j;
  }

  friend void to_json(nlohmann::json& j, const InitializeParams& m) {
    j.emplace("processId", m.processId);
  }
  friend void from_json(const nlohmann::json& j, InitializeParams& m) {
    j.at("processId").get_to(m.processId);
  }
};

struct Message {
  std::string jsonrpc;
};

void to_json(nlohmann::json& j, const Message& m) {
  j = nlohmann::json{{"jsonrpc", m.jsonrpc}};
}

void from_json(const nlohmann::json& j, Message& m) {
  j.at("jsonrpc").get_to(m.jsonrpc);
}

struct RequestMessage : public Message {
  int id;
  std::string method;
  std::unique_ptr<Params> params;
};

void to_json(nlohmann::json& j, const RequestMessage& m) {
  j = static_cast<const Message&>(m);
  j.emplace("id", m.id);
  j.emplace("method", m.method);
  nlohmann::json params = m.params->json();
  j.emplace("params", params);
}

void from_json(const nlohmann::json& j, RequestMessage& m) {
  from_json(j, static_cast<Message&>(m));
  j.at("id").get_to(m.id);
  j.at("method").get_to(m.method);
  if (true /*method=="initialize"*/) {
    m.params.reset(new InitializeParams(j.at("params")));
  }
}

TEST(json, simple) {
  //  InitializeParams a{1};
  auto params = std::make_unique<InitializeParams>(1);
  RequestMessage msg{"", 1, "initialize", std::move(params)};
  nlohmann::json j = msg;
  std::cout << j.dump() << std::endl;

  nlohmann::json from{
      R"({"id":1,"jsonrpc":"","method":"initialize","params":{"processId:":1}})"};
  auto req = from.get<RequestMessage>();
  nlohmann::json paramsback = req.params->json();
  std::cout << paramsback.dump() << std::endl;
}
