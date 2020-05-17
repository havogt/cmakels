### custom/dependencies

Request:
  - method: ‘custom/dependencies’
  - params: `TextDocumentPositionParams`

Response:

  - result: `Dependencies` | `null` defined as follows:

```typescript
/**
 * The result of a dependency request.
 */
interface Dependencies {
	/**
	 * The dependency graph as a list of `DependencyNode`s ...
	 */
	nodes: DependencyNode[];

	/**
	 * ... and a list of edges between nodes.
	 */
	edges: DependencyEdge[];

	/**
	 * An optional root, identifies the root node, if any.
	 */
	root: DependencyNodeId;

	/**
	 * An optional range is a range inside a text document
	 * that is used to visualize a dependency request, e.g. by changing the background color.
	 */
	range?: Range;
}
```

where

```typescript
type DependencyNodeId = string | number;
```

and `DependencyNode` is defined as

```typescript
/**
 * A node in the dependency graph
 */
interface DependencyNode {
	/**
	 * Identifier of the node
	 */
	id: DependencyNodeId;

	/**
	 * The node's content.
	 */
	content: MarkupContent;
}
```

where `DependencyEdge`

```typescript
/**
 * An edge in the dependency graph
 */
interface DependencyEdge {
	/**
	 * Parent
	 */
	from: DependencyNodeId;

	/**
	 * Child
	 */
	to: DependencyNodeId;

	/**
	 * Edge kind
	 */
	kind: number;
}
```

where `kind` one of `DependencyEdgeKind`

```typescript
/**
 * Edge kind.
 * TODO: instead of graphical representation, the kind could be a semantic kind.
 * But not sure if it could be expressed universal enough.
 */
namespace DependencyEdgeKind {
	export const Solid = 1;
	export const Dashed = 2;
	export const Dotted = 3;
	export const Bold = 4;
}
```
