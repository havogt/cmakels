import { workspace, ExtensionContext, commands, window } from 'vscode';

import {
	LanguageClient,
	LanguageClientOptions,
	ServerOptions,
	TextDocumentPositionParams,
	TextDocumentIdentifier,
} from 'vscode-languageclient';

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

			client.sendRequest("custom/dependencies", params );
		}
	}));

	client.onReady().then(() => {
		client.onRequest("custom/dependencies", () => {
			console.log("received custom/dependencies");
		});
	});

	client.start();
}

export function deactivate(): Thenable<void> | undefined {
	if (!client) {
		return undefined;
	}
	return client.stop();
}
