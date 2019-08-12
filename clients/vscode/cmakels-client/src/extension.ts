import * as path from 'path';
import { workspace, ExtensionContext } from 'vscode';

import {
	LanguageClient,
	LanguageClientOptions,
	ServerOptions,
	TransportKind
} from 'vscode-languageclient';

/**
 * Method to get workspace configuration option
 * @param option name of the option (e.g. for clangd.path should be path)
 * @param defaultValue default value to return if option is not set
 */
function getConfig<T>(option: string, defaultValue?: any): T {
	const config = workspace.getConfiguration('cmakels');
	return config.get<T>(option, defaultValue);
}


let client: LanguageClient;

export function activate(context: ExtensionContext) {
	let executablePath = getConfig<string>('path');

	let rootpath = workspace.workspaceFolders ? workspace.workspaceFolders[0].uri.toString() : "undefined";
	let serverOptions: ServerOptions = {
		command: executablePath,
		args: [rootpath, getConfig<string>("buildDirectory")]
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

	// Start the client. This will also launch the server
	client.start();
}

export function deactivate(): Thenable<void> | undefined {
	if (!client) {
		return undefined;
	}
	return client.stop();
}
