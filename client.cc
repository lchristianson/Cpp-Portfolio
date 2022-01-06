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




using namespace std;
const int BUFSIZE=102400;

void ErrorCheck(bool condition, const char *msg)
{
	if (condition)
	{
		perror(msg);
		exit(1);
	}
}

int MakeSocket(const char *host, const char *port) {
	int s; 			
	int len;	
	struct sockaddr_in sa; 
	struct hostent *hp;
	struct servent *sp;
	int portnum;	
	int ret;

	hp = gethostbyname(host);
	ErrorCheck(hp==0, "Gethostbyname");
	bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;
	sscanf(port, "%d", &portnum);
	if (portnum > 0) {
		sa.sin_port = htons(portnum);
	}
	else {
		sp=getservbyname(port, "tcp");
		portnum = sp->s_port;
		sa.sin_port = sp->s_port;
	}
	s = socket(hp->h_addrtype, SOCK_STREAM, 0);
	ErrorCheck(s == -1, "Could not make socket");
	ret = connect(s, (struct sockaddr *)&sa, sizeof(sa));
	ErrorCheck(ret == -1, "Could not connect");
	return s;
}

int main(int argc, char *argv[]) {
	char buf[BUFSIZE];
	string userIn;
	string userRecieved;

	// Make a socket
	int sock = MakeSocket(argv[1], argv[2]);
	cout << "socket is " << sock << endl;
	assert(sock != -1);

	while (true) {
		cout << "Please enter a message to send:" << endl;
		getline(cin, userIn);
		if (userIn == "QUIT") {
			for (int i = 0; i < userIn.length(); i++)
				buf[i] = userIn[i];
			buf[userIn.length()] = 0;
			int quitbit = write(sock, buf, 5);
			if (quitbit == -1)
				perror("QUIT send was bad.");
			assert(quitbit == 5);

			cout << "Quit command recieved, program is exiting." << endl;
			break;
		}
		
		// Send it
		int userSend = write(sock, buf, userIn.length()+1);
		if (userSend == -1)
			perror("Write was bad");
		assert(userSend == userIn.length() + 1);
		
		// Read the file
		int userRec = read(sock, buf, BUFSIZE-2);
		assert(userRec != -1);

		buf[userRec] = 0;
		buf[userRec - 1] = 0;
		cout << "Server sent: " << buf << endl;
		for (int i = 0; i <= userSend; i++) {
			buf[i] = 0;
		}
		userIn = "";

	}

	return 0;
}


