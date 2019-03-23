#pragma once

#include <optional>
#include <string>
#include <variant>

#include "../external/json.hpp"

// TODO respect ODR

namespace message {

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
  // TODO params
};

void to_json(nlohmann::json& j, const RequestMessage& m) {
  j = static_cast<const Message&>(m);
  j.emplace("id", m.id);
  j.emplace("method", m.method);
}

void from_json(const nlohmann::json& j, RequestMessage& m) {
  from_json(j, static_cast<Message&>(m));
  j.at("id").get_to(m.id);
  j.at("method").get_to(m.method);
}

template <typename D>
struct ResponseError {
  int code;
  std::string message;
  std::optional<D> data;
};

struct ResponseMessage : public Message {
  std::string id;
  std::optional<std::variant<std::string, int, bool>> result;
  std::optional<ResponseError<int>> error;  // TODO int?
};

void to_json(nlohmann::json& j, const ResponseMessage& m) {
  j = static_cast<const Message&>(m);
  if (m.result) {
    if (std::holds_alternative<int>(*(m.result)))
      j.emplace("result", std::get<int>(*(m.result)));
    // TODO error
  }
}

void from_json(const nlohmann::json& j, ResponseMessage& m) {
  from_json(j, static_cast<Message&>(m));
  if (j.contains("result")) m.result = 0;  // j.at("result").get();
}

struct NotificationMessage : public Message {
  std::string method;
  // TODO params
};

void to_json(nlohmann::json& j, const NotificationMessage& m) {
  j = static_cast<const Message&>(m);
  j.emplace("method", m.method);
}

void from_json(const nlohmann::json& j, NotificationMessage& m) {
  from_json(j, static_cast<Message&>(m));
  j.at("method").get_to(m.method);
}

}  // namespace message
