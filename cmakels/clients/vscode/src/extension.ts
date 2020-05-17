import { workspace, ExtensionContext, commands, window, ViewColumn } from 'vscode';

import {
	LanguageClient,
	LanguageClientOptions,
	ServerOptions,
	TextDocumentPositionParams,
	TextDocumentIdentifier,
	RequestType,
	Range,
	MarkupContent
} from 'vscode-languageclient';

import { GraphvizPreviewGenerator } from './vscode-graphviz/GraphvizPreviewGenerator';

/**
 * Method to get workspace configuration option
 * @param option name of the option (e.g. for cmakels.path should be path)
 * @param defaultValue default value to return if option is not set
 */
function getConfig<T>(option: string, defaultValue?: any): T {
	const config = workspace.getConfiguration('cmakels');
	return config.get<T>(option, defaultValue);
}


let client: LanguageClient;


namespace CustomDependenciesRequest {
    export const type =
        new RequestType<TextDocumentPositionParams, Dependencies, void, void>(
            'custom/dependencies');
}

type DependencyNodeId = string | number;

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

export function activate(context: ExtensionContext) {
	let executablePath = getConfig<string>('path');

	let serverOptions: ServerOptions = {
		command: executablePath,
		args: [getConfig<string>("buildDirectory")]
	};

	// const filePattern: string = '**/*.{' +
	// 	['CMakeLists.txt', 'cmake', 'cmake.in'].join()
	// 	+ '}';

	// Options to control the language client
	let clientOptions: LanguageClientOptions = {
		// Register the server for plain text documents
		// documentSelector: [{ scheme: 'file', pattern: filePattern }],
		documentSelector: [{ scheme: 'file', language: 'cmake' }],
		synchronize: {
			// Notify the server about file changes to '.clientrc files contained in the workspace
			// fileEvents: workspace.createFileSystemWatcher('**/.clientrc')
		},
		outputChannelName: 'cmakels'
	};

	// Create the language client and start the client.
	client = new LanguageClient(
		'cmakels',
		'CMake Language Server',
		serverOptions,
		clientOptions
	);

	context.subscriptions.push(commands.registerCommand('cmakels.restart', async () => {
		await client.stop();
		client.start();
	}));

	context.subscriptions.push(commands.registerCommand('cmakels.target_dependencies', async () => {
		if(window.activeTextEditor) {
			let identifier : TextDocumentIdentifier =
				{ uri: window.activeTextEditor.document.uri.toString()};

			let params :TextDocumentPositionParams = {
				textDocument: identifier,
				position: window.activeTextEditor.selection.active};

			client.sendRequest(CustomDependenciesRequest.type, params ).then((result) => {
				// console.log("yeah: " + result.dependencies[0].id);
				const graphvizPreviewGenerator = new GraphvizPreviewGenerator(context);
				let dotgraph:string = "digraph graphname {\n";
				dotgraph += "node [color=purple]\n";
				for (let e of result.edges) {
					// if(d.children) {
					// 	for(let child of d.children) {
							dotgraph += "\"" + e.from + "\"->\"" + e.to + "\"  [color=\"blue\" style=\"";
							switch(e.kind) {
								case DependencyEdgeKind.Bold:
									dotgraph += "bold";
									break;
								case DependencyEdgeKind.Dashed:
									dotgraph += "dashed";
									break;
								case DependencyEdgeKind.Solid:
									dotgraph += "solid";
									break;
								case DependencyEdgeKind.Dotted:
									dotgraph += "dotted";
									break;
							}
							dotgraph +="\"];";
					// 	}
					// }
				}
				dotgraph += "}";
				graphvizPreviewGenerator.revealOrCreatePreview("my_target", dotgraph, ViewColumn.Beside);

			});
		}
	}));

	client.start();
}

export function deactivate(): Thenable<void> | undefined {
	if (!client) {
		return undefined;
	}
	return client.stop();
}
