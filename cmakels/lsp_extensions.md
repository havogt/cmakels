### custom/dependencies

Request:
  - method: ‘custom/dependencies’
  - params: `TextDocumentPositionParams`

Response:

  - result: `Dependencies` | `null` defined as follows:

```typescript
/**
 * The result of a hover request.
 */
interface Dependencies {
	/**
	 * The dependency graph as a list of `DependencyEntry`ies
	 */
	dependencies: DependencyEntry[];

	/**
	 * An optional range is a range inside a text document
	 * that is used to visualize a dependency request, e.g. by changing the background color.
	 */
	range?: Range;
}
```

where `DependencyEntry` is defined as

```typescript
/**
 * A node in the dependency graph
 */
interface DependencyEntry {
	/**
	 * Identifier of the node
	 */
	id: string | number;

	/**
	 * List of children, identified by `id`.
	 */
	children?: string[] | number[];

	/**
	 * The node's content.
	 */
	content: MarkupContent
}
```
