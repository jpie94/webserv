# Webserv - @42Paris
### 🌐 About the project
Webserv is a high-performance HTTP/1.1 server written in C++ 98. This project involves writing an entire server from scratch, capable of handling multiple client connections simultaneously without ever being blocked.

The project focuses on socket programming, multiplexing, and strict adherence to the RFC 7230 to 7235 standards.

### 🏗️ Architecture
The server follows a non-blocking I/O model. It uses a single process to monitor multiple file descriptors (sockets) and handle events as they occur.

#### Key Components:
* Multiplexer: Implementation of poll() to handle concurrent connections efficiently.

* Request Parser: A robust state-machine based parser that handles headers, body, chunked encoding, and multipart uploads.

* Response Generator: Builds valid HTTP responses with correct status codes and headers.

* CGI Handler: Executes scripts (Python, PHP, etc.) via environment variables and pipes.

### 🛠️ Features
Mandatory Features
* HTTP Methods: Full support for GET, POST, and DELETE.

* Configuration File: Custom parser for a Nginx-like config file (ports, hostnames, default error pages, limit body size, routes).

* Static File Server: Serving HTML, CSS, images, and other static assets.

* Directory Listing: Automatic generation of directory index pages.

Bonus Features
* CGI Execution: Fully functional Common Gateway Interface to handle dynamic content (e.g., .php, .py, .sh).

* Cookies & Session Management: Ability to handle Set-Cookie and track user sessions.

* File Upload: Support for multipart/form-data allowing users to upload files to the server.

* Multiple Servers: Support for multiple server blocks with different server_name and ports (Virtual Hosting).

### ⚙️ Configuration File
The server is configured via a file that defines its behavior. Example:

```
server {
	listen 8090;
	root root;
	client_max_body_size 55000000;
	upload_folder /uploads;


	location /error
	{
		allowed_methods POST;
		upload_folder /img;
	}

	location /cgi-bin
	{
		cgi .php /bin/php8.1;
		cgi .py /usr/bin/python3;
		allowed_methods GET POST;
	}
}
```
### 🚀 Getting Started
```Bash
# Compilation
make

# Execution
./webserv [path_to_config_file]

# If no config file is provided, the server uses a default configuration.
```
### Testing
You can test the server using a web browser at http://localhost:8080 or using curl:

```Bash

curl -v localhost:8080
```
### 🛡️ Technical Challenges
* Non-blocking sockets: Ensuring the server never hangs while waiting for data from a slow client.

* Memory Management: Strictly ensuring no memory leaks occur during high-traffic stress tests.

##### 📬 Contact
Login: jpiech, qsomarri

School: 42 Paris
