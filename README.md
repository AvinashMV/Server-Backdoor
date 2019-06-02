# Server-Backdoor
Software Security Project (C/C++)

This is server backdoor program, which sits on victims machine. It allows hacker to run a shell command as a HTTP request from the browser. It sends HTTP/1.1 as a response through firewall.

*The backdoor functionality is that when server receives a GET request for a URL in the form of /exec/<command>, then
server takes <command> and execute it using the system libc function and the HTTP response will be the stdout of the executed command.

To run server 
./normal_web_server <portno>

In client(browser to connect to webserver):
http://localhost:<portno>/<input>



