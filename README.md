# WebServerPlugin
A plugin for Unreal Engine 4 that uses Unreal's FSocket system to create a local web server. Built using Unreal Engine 4.22. 

Currently verified to work on the following platforms: Windows, Android.

Eventual support for the following platforms: iOS, Mac, Linux.

The WebServer connects the Unreal Engine to requests received on localhost:8080 (127.0.0.1:8080). Upon connecting, each user will spawn a new Connection that will parse the request. The developer must make sure that this request is handled, either by binding a callback function to an endpoint using the WebServer's **RegisterURL** function, adding the correct file to the plugin's "Root" folder (including correct subdirectory), or registering an HTML response during runtime. If there is no handling mechanism in place, the WebServer will default to a simple 404 message, that can be overwritten by binding a callback function, creating a "404.<ext>" file in the plugin's "Root" folder (at base directory level), or registering an HTML response during runtime.
  
 The Connection object has two paths to extracting data:
 
 1) Get all variables in a struct at one time by using the **GetRequestDetails** function (requires developer to use an **SHTTPRequestDetails** struct)
 2) Get specific variables using the **GetRequest{VariableName}** functions.
 
 The Connection object has four paths to setting and sending data:
 1) Set and send all variables in a struct at one time when sending using the **SendResponseWithDetails** function (Requires developer to create and fill an **SHTTPResponseDetails** struct).
 2) Set all variables in a struct at one time by using the **SetResponseDetails** function, then use the **SendResponseDetails** function (requires developer to create and fill an **SHTTPResponseDetails** struct).
 3) Set specific variables using the **SetResponse{VariableName}** functions, then use the SendResponse function.

The server will process a request according to the first applicable category in the following list:

1) Receive callback through C++Blueprints.
- Through C++/Blueprints, the developer can bind a callback function to a request for a certain page or file. This callback function will be responsible for sending a message back to the client.
2) Find and send file from plugin's "Root" directory or from runtime HTML assignment. **[WIP]**
- The web server can serve statically if there is a file available to serve within the correct directory (the plugin's "Root" directory) in the file system, or if the developer has assigned an HTML response for the URL.
3) Serve 404 page (optional C++/Blueprint callback binding OR static file assignment OR runtime HTML assignment).
- The web server will automatically send out a simple 404 document. The developer can optionally choose to overwrite this 404 response with a static file or a runtime HTML string, or use a callback to capture the request the user made to the web server and handle the 404 more granularly.
