#include <lscpp/lsp_launcher.h>

#include <chrono>
#include <future>
#include <sstream>
#include <thread>

#include "../external/loguru/loguru.hpp"
#include "lsp_header.h"
#include "lsp_message_handler.h"

namespace lscpp {

void write_lsp_message(transporter &t, std::string const &content) {
  std::stringstream content_length;
  content_length << "Content-Length: ";
  content_length << content.size();
  t.write_line(content_length.str());
  t.write_line("");

  t.write_message(content);
}

void launch(lsp_server &&server, launch_config config,
            transporter &&transporter_) {

  // Allows to attach a debugger,
  // before the language server starts to communicate with the client.
  std::this_thread::sleep_for(std::chrono::seconds(config.startup_delay));

  // If we want to play with different message handlers, we can revert to a
  // templated launch (or duplicate the launch as long as it is simple)
  lsp_message_handler message_handler{};

  // setup logger
  loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
  loguru::g_colorlogtostderr = false;
  if (config.logger.filename.size() > 0)
    loguru::add_file(config.logger.filename.c_str(), loguru::Truncate,
                     config.logger.verbosity);

  auto rcv = std::async(std::launch::async, [&]() {
    while (true) {
      auto header = parse_header(transporter_);
      LOG_F(INFO, "content-length: '%d'", header.content_length);
      if (header.content_length <
          0) { // TODO parse header should not use -1 to report an error
        continue;
      }
      auto msg = transporter_.read_message(header.content_length);
      LOG_F(INFO, "msg: '%s'", msg.c_str());

      auto result = message_handler.handle_message(server, msg);
      if (result) {
        LOG_F(INFO, "Sending response: '%s'", (*result).c_str());
        write_lsp_message(transporter_, *result);
      }
    }
  });

  rcv.wait();
}

} // namespace lscpp
