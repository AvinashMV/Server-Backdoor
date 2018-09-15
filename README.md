# Server-Backdoor
Project of Software Security. 
A backdoor server that allows the hacker access to the system at a later date, without exploiting the same vulnerabilities

*HTTP 1.1 server, based on RFC 2616 from scratch, without using any libraries except for the C standard library.

*The backdoor functionality is that when server receives a GET request for a URL in the form of /exec/<command>, then it 
server should take <command> and execute it using the system libc function and the HTTP response will be the stdout of the executed command.

To run server 
./normal_web_server <portno>

In client(browser to connect to webserver):
http://localhost:<portno>/<input>



