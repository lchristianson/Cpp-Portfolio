#include <iostream>		// cout, cerr, etc
#include <fstream>
#include <stdio.h>		// perror
#include <string.h>		// bcopy
#include <netinet/in.h>		// struct sockaddr_in
#include <unistd.h>		// read, write, etc
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>
#include <netdb.h>
#include <cassert>
#include <stdlib.h>
using namespace std;

const int BUFSIZE=1024000;

int MakeServerSocket(int port)
{
    int server_fd, new_socket;
    struct sockaddr_in address; 
    int opt = 1, addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
       
    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address)) <0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}


int main(int argc, char *argv[]) {


	int s; 			// socket descriptor
	int len;		// length of reveived data
	char buf[BUFSIZE];	// buffer in which to read
	int ret;		// return code from various system calls
    string filename;
    ofstream log ("log.txt", ios::app);
    ifstream file;
    int code;
    string recieved;
    string request;
    string meta;
    char data[BUFSIZE];
    string denial = "euclid.nmu.edu\r\n";
    string deny = "Not today, Randy.\r\n";
    int w;
    struct hostent* host;
	s = MakeServerSocket(2222);
    time_t t;
    struct tm *tmp;
    int min;
    int hour;
    int sec;
    int day;
    int month;
    int year;

	while (1) {
        int edge = -1;
		struct sockaddr_in sa;
		int sa_len = sizeof(sa);
		int fd = accept(s, (struct sockaddr *)&sa, (unsigned int *)&sa_len);
       // host = gethostbyaddr(&sa, size_t(&sa_len), AF_INET);
        //char* hostname = host->h_name;

        //cout << hostname << endl;
        int pid = fork();
        if(pid = -1)
            exit(1);
        if(pid = 0) {
		    len = read(fd, buf, BUFSIZE);
            recieved = buf;
            cout << recieved << endl;
             /* t = time(NULL);
            tmp = localtime(&t);
            sec = tmp->tm_sec;
            min = tmp->tm_min;
            hour = tmp->tm_hour;
            day = tmp->tm_mday;
            month = tmp->tm_mon;
            */

            log.open("log.txt", ios::app);
            string logsend = "(IP Address) (user-identifier) NAME [";
            string logsend2 = "] " + request + " referer\r\n";
            log.write(logsend.c_str(), logsend.length());
            log.close();
            //log.write(day, 2);
            log.open("log.txt", ios::app);
            log.write(logsend2.c_str(), logsend2.length());
            log.close();



            edge = recieved.find("Edg");
            if( edge > -1) {
                string edgedenial;
                meta = "HTTP/1.0 200 EDGE BAD";
                edgedenial = meta + "\r\n" + deny;
                w = write(fd, edgedenial.c_str(), edgedenial.length());
                cout << "Edge has been denied" << endl << endl;
                continue;
            }
            int slashone = recieved.find('/');
            int space2 = recieved.find("HTTP", slashone);
            request = recieved.substr(slashone+1, space2-6);
        
        
        
        
            if(recieved.find(denial) > -1) {
                w = write(fd, deny.c_str(), deny.length());
            }
		    assert(len > 0);
		    buf[len] = 0;
		
            file.open(filename, ios::in);
            meta = "HTTP/1.0 200 OKAY \r\n";
            file.read(data, BUFSIZE);
            string msg = meta + "\r\n" + data;
            w = write(fd, msg.c_str(), msg.length());

        
            code = 200;
            if(!file.is_open()) {
                file.open("404.txt", ios::out);
                code = 404;
            }
            close(fd);
            exit(0);
        }
        int w;
        do {
           w = waitpid(-1, NULL, WNOHANG);
        }
        while (w > 0);
        close(fd);
	}
    log.close();
}