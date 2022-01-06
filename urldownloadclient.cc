/* WARNING!  THIS CODE DOES NOT WORK FOR REASONABLE YET LARGE  FILE SIZES!!!! */

#include <iostream>		
#include <stdio.h>	
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <chrono>




using namespace std;
const int BUFSIZE = 102400;

void ErrorCheck(bool condition, const char* msg)
{
	if (condition)
	{
		perror(msg);
		exit(1);
	}
}

int MakeSocket(const char* host, const char* port) {
	int s;
	int len;
	struct sockaddr_in sa;
	struct hostent* hp;
	struct servent* sp;
	int portnum;
	int ret;

	hp = gethostbyname(host);
	ErrorCheck(hp == 0, "Gethostbyname");
	bcopy((char*)hp->h_addr, (char*)&sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;
	sscanf(port, "%d", &portnum);
	if (portnum > 0) {
		sa.sin_port = htons(portnum);
	}
	else {
		sp = getservbyname(port, "tcp");
		portnum = sp->s_port;
		sa.sin_port = sp->s_port;
	}
	s = socket(hp->h_addrtype, SOCK_STREAM, 0);
	ErrorCheck(s == -1, "Could not make socket");
	ret = connect(s, (struct sockaddr*)&sa, sizeof(sa));
	ErrorCheck(ret == -1, "Could not connect");
	return s;
}

int readFromUrl(int sock, char* buf, int BUFSIZE) {
	int response  = read(sock, buf, BUFSIZE);
	cout << buf << endl;
	assert(buf[9] == '2');
	return response;
	
}

void writeToUrl(int sock, const char* buf, int BUFSIZE) {
	int request = write(sock, buf, BUFSIZE);
	assert(request != -1);
}

int main(int argc, char* argv[]) {
	char buf[BUFSIZE];
	string url;

	cout << "Enter a webpage that you would like to download: " << endl;
	getline(cin, url);
	cout << "url recieved" << endl;

	int slashct = 0;
	string realurl = "";
	string file = "";
	for(int i = 0; i < url.length(); i++) {
		if(slashct == 2 && url[i] != '/')
			realurl += url[i];
		if(url[i] == '/')
			slashct++;
		if(slashct >= 3)
			file+=url[i];
	}
	cout << "" << realurl << endl;
	cout << "" << file << endl;

	// Make a socket
	auto start = std::chrono::steady_clock::now();
	int sock = MakeSocket(realurl.c_str(), "80");
	assert(sock != -1);

	cout << "about to send request" << endl;
	string sendrequest = "GET " + file + " HTTP/1.0\r\n\r\n";
	cout << "Send Request: " << sendrequest << endl;
	writeToUrl(sock, sendrequest.c_str(), sendrequest.length());
	cout << "wrote to url" << endl;
	
	int len = readFromUrl(sock, buf, BUFSIZE);
	while(len > 0)
		len = readFromUrl(sock, buf, BUFSIZE);
	cout << "read from url" << endl;
	
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	cout << "It took " << elapsed_seconds.count() << "ms to download the webpage." << endl;






	return 0;
}


