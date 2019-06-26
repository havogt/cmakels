#pragma once

#include "TextDocumentPositionParams.h"

namespace lscpp {
namespace protocol {

struct CompletionParams : public TextDocumentPositionParams {
	/**
	 * The completion context. This is only available if the client specifies
	 * to send this using `ClientCapabilities.textDocument.completion.contextSupport === true`
	 */
	// context?: CompletionContext;
};

} // namespace protocol
} // namespace lscpp
