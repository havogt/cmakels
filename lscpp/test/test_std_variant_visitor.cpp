#include "../external/json.hpp"
#include <any>
#include <gtest/gtest.h>
#include <variant>

class Message {};
class RequestMessage {};

void print(Message const &msg) { std::cout << "Message" << std::endl; }

void print(RequestMessage const &msg) {
  std::cout << "RequestMessage" << std::endl;
}

TEST(json, serializable) {
  std::variant<Message, RequestMessage> something;
  something = Message{};
  std::visit([](auto &&arg) { print(arg); }, something);
}
