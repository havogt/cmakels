#include "../external/json.hpp"
#include <any>
#include <gtest/gtest.h>
#include <string_view>
#include <type_traits>
#include <variant>

class Object {
  struct concept_t {
    virtual ~concept_t() {}
    virtual nlohmann::json do_serialize() const = 0;
  };

  template <typename T> struct model_t : public concept_t {
    model_t() = default;
    model_t(const T &v) : m_data(v) {}
    model_t(T &&v) : m_data(std::move(v)) {}

    nlohmann::json do_serialize() const { return serialize(m_data); }

    T m_data;
  };

public:
  Object() = default;

  Object(const Object &) = delete;
  Object(Object &&) = default;

  template <typename T>
  Object(T &&impl)
      : m_impl(new model_t<std::decay_t<T>>(std::forward<T>(impl))) {}

  Object &operator=(const Object &) = delete;
  Object &operator=(Object &&) = default;

  template <typename T> Object &operator=(T &&impl) {
    m_impl.reset(new model_t<std::decay_t<T>>(std::forward<T>(impl)));
    return *this;
  }

  friend nlohmann::json serialize(Object const &o) {
    return o.m_impl->do_serialize();
  }

  template <typename T> friend T const &as(Object const &o) {
    return *dynamic_cast<T>(o.m_impl.get());
  }

private:
  std::unique_ptr<concept_t> m_impl;
};

class Message {
public:
  const std::string jsonrpc = "2.0";
};

nlohmann::json serialize(Message const &msg) {
  nlohmann::json j{};
  j.emplace("jsonrpc", msg.jsonrpc);
  return j;
};

struct InitializeParams {
  int processId;
};

nlohmann::json serialize(InitializeParams const &p) {
  nlohmann::json j{};
  j.emplace("processId", p.processId);
  return j;
}

struct OtherParams {
  std::string bla;
};

nlohmann::json serialize(OtherParams const &p) {
  nlohmann::json j{};
  j.emplace("blabal", p.bla);
  return j;
}

class RequestMessage : public Message {
public:
  RequestMessage(int id, std::string const &method, Object &&params)
      : id{id}, method{method}, params{std::move(params)} {}

  /**
   * The request id.
   */
  int id;

  /**
   * The method to be invoked.
   */
  std::string method;

  /**
   * The method's params.
   */
  Object params; // Array<any> | object
};

nlohmann::json serialize(RequestMessage const &msg) {
  nlohmann::json j = serialize(dynamic_cast<Message const &>(msg));
  j.emplace("id", msg.id);
  j.emplace("method", msg.method);
  j.emplace("params", serialize(msg.params));
  return j;
};

template <typename T>
std::enable_if_t<std::is_same_v<T, InitializeParams>, InitializeParams>
deserialize_to(nlohmann::json j) {
  return InitializeParams{j.at("processId")};
}

template <typename T>
std::enable_if_t<std::is_same_v<T, RequestMessage>, RequestMessage>
deserialize_to(nlohmann::json j) {
  return RequestMessage{j.at("id"), j.at("method"), [&]() -> Object {
                          if (j.at("method") == "initialize") {
                            return deserialize_to<InitializeParams>(
                                j.at("params"));
                          }
                          return OtherParams{"asdf"};
                        }()};
}

//   friend void to_json(nlohmann::json &j, const RequestMessage &m) {
//     j = static_cast<const Message &>(m);
//     j.emplace("id", m.id);
//     j.emplace("method", m.method);
//     nlohmann::json params = m.params->json();
//     j.emplace("params", params);
//   }

TEST(json, serializable) {
  // nlohmann::json from = {{"id", 1},
  //                        {"jsonrpc", ""},
  //                        {"method", "initialize"},
  //                        {"params", {"processId:", 1}}};
  //   std::cout << from.dump() << std::endl;

  RequestMessage msg{1, "initialize", InitializeParams{1}};

  auto j = serialize(msg);
  std::cout << j.dump() << std::endl;

  RequestMessage msg2 = deserialize_to<RequestMessage>(j);

  std::cout << serialize(msg2).dump() << std::endl;

  //   message_handler hndlr{std::make_unique<my_request_message_handler>()};
  //   hndlr.handle(from);

  //  auto req = from.get<RequestMessage>();
  //  nlohmann::json paramsback = req.params->json();
  //  std::cout << paramsback.dump() << std::endl;
}
