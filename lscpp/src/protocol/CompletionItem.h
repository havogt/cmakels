#pragma once

#include <string>

namespace lscpp {
namespace protocol {

struct CompletionItem {
  /**
   * The label of this completion item. By default
   * also the text that is inserted when selecting
   * this completion.
   */
  std::string label;

  /**
   * The kind of this completion item. Based of the kind
   * an icon is chosen by the editor. The standardized set
   * of available values is defined in `CompletionItemKind`.
   */
  //   kind ?: number;

  /**
   * A human-readable string with additional information
   * about this item, like type or symbol information.
   */
  //   detail ?: string;

  /**
   * A human-readable string that represents a doc-comment.
   */
  //   documentation ?: string | MarkupContent;

  /**
   * Indicates if this item is deprecated.
   */
  //   deprecated ?: boolean;

  /**
   * Select this item when showing.
   *
   * *Note* that only one completion item can be selected and that the
   * tool / client decides which item that is. The rule is that the *first*
   * item of those that match best is selected.
   */
  //   preselect ?: boolean;

  /**
   * A string that should be used when comparing this item
   * with other items. When `falsy` the label is used.
   */
  //   sortText ?: string;

  /**
   * A string that should be used when filtering a set of
   * completion items. When `falsy` the label is used.
   */
  //   filterText ?: string;

  /**
   * A string that should be inserted into a document when selecting
   * this completion. When `falsy` the label is used.
   *
   * The `insertText` is subject to interpretation by the client side.
   * Some tools might not take the string literally. For example
   * VS Code when code complete is requested in this example `con<cursor
   * position>` and a completion item with an `insertText` of `console` is
   * provided it will only insert `sole`. Therefore it is recommended to use
   * `textEdit` instead since it avoids additional client side interpretation.
   */
  //   insertText ?: string;

  /**
   * The format of the insert text. The format applies to both the `insertText`
   * property and the `newText` property of a provided `textEdit`.
   */
  //   insertTextFormat ?: InsertTextFormat;

  /**
   * An edit which is applied to a document when selecting this completion. When
   * an edit is provided the value of `insertText` is ignored.
   *
   * *Note:* The range of the edit must be a single line range and it must
   * contain the position at which completion has been requested.
   */
  //   textEdit ?: TextEdit;

  /**
   * An optional array of additional text edits that are applied when
   * selecting this completion. Edits must not overlap (including the same
   * insert position) with the main edit nor with themselves.
   *
   * Additional text edits should be used to change text unrelated to the
   * current cursor position (for example adding an import statement at the top
   * of the file if the completion item will insert an unqualified type).
   */
  //   additionalTextEdits ?: TextEdit[];

  /**
   * An optional set of characters that when pressed while this completion is
   * active will accept it first and then type that character. *Note* that all
   * commit characters should have `length=1` and that superfluous characters
   * will be ignored.
   */
  //   commitCharacters ?: string[];

  /**
   * An optional command that is executed *after* inserting this completion.
   * *Note* that additional modifications to the current document should be
   * described with the additionalTextEdits-property.
   */
  //   command ?: Command;

  /**
   * A data entry field that is preserved on a completion item between
   * a completion and a completion resolve request.
   */
  //   data ?: any
};

} // namespace protocol
} // namespace lscpp
