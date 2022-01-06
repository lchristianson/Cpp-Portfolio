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

//Parses entered email address for email host name and returns it. ie- gmail.com, hotmail.com, etc.
string hostParse(string n) {
    bool hostCheck = false;
    string host;
        for(int i = 0; i < n.length(); i++) {
            if(hostCheck)
                host = host + n[i];
            if(n[i] == '@')
                hostCheck = true;
        }
        return host;
}

void readFromServer(int sock, char* buff, int len) {
    int in = read(sock, buff, len);
    assert(buff[0] == '2' || buff[0] == '3');
}

void writeToServer(int sock, string buff, int len) {
    int out = write(sock, buff.c_str(), len);
    assert(out != -1);
}

/*string selectServer(string r) {


}*/

int main(int argc, char* argv[]) {
	char buf[BUFSIZE];
    string toEmailHost;
    string toAddress;
    cout << "Lucas Christianson's Email Client" << endl;
    cout << "---------------------------------" << endl << endl;

	
       
        cout << "Please enter the recipients email address: " << endl;
        getline(cin, toAddress);
        toEmailHost = hostParse(toAddress);
        
        /*
        string digCommand = "dig -t mx " + toEmailHost;
        FILE* pipe = popen(digCommand.c_str(), "w");
        if(!pipe)
            cout << "pipeline out failed to open." << endl;
        pclose(pipe);
        
        pipe = popen(digCommand.c_str(), "r");
        
        char * digCommandResponseBuf = new char[10000];
        string digCommandResponse;
        while(fgets(digCommandResponseBuf, 10000, pipe) != NULL)
            digCommandResponse += digCommandResponseBuf;

        pclose(pipe);
        string selectedServer = selectServer(digCommandResponse);
        */
        //Parse return for viable mail sockets
        //telnet "mail socket" + number

        string fromAddress;
        cout << "Please enter your email address: " << endl;
        getline(cin, fromAddress);
        string fromAddressHost;
        bool hostCheck = false;
         for(int i = 0; i < fromAddress.length(); i++) {
            if(hostCheck)
                fromAddressHost = fromAddressHost + fromAddress[i];
            if(toAddress[i] == '@')
                hostCheck = true;
        }

        if(toEmailHost != "gmail.com") {
                cout << "Email address is incompatible with this build." << endl;
                return 0;
        }

        int sock = MakeSocket("gmail-smtp-in.l.google.com.", "25");
            assert(sock != -1);

        string output;

        output = "HELO " + fromAddressHost + "\r\n";
        writeToServer(sock, output, output.length());
        readFromServer(sock, buf, BUFSIZE);

        output = "MAIL FROM:<" + fromAddress +">\r\n";
        writeToServer(sock, output, output.length());
        readFromServer(sock, buf, BUFSIZE);

        output = "RCPT TO:<" + toAddress + ">\r\n";
        writeToServer(sock, output, output.length());
        readFromServer(sock, buf, BUFSIZE);

        output = "DATA\r\n";
        writeToServer(sock, output, output.length());
        readFromServer(sock, buf, BUFSIZE);
        
       
        string subject = "SUBJECT: ";
        cout << "Please enter a subject line: " << endl;
        string tempsub;
        getline(cin, tempsub);
        subject += (tempsub + "\r\n");
        writeToServer(sock, subject, subject.length());
        
        string toHeader = "To: " + toAddress + "\r\n";
        string fromHeader = "From: " + fromAddress + "\r\n";
        writeToServer(sock, toHeader, toHeader.length());
        writeToServer(sock, fromHeader, fromHeader.length());

        cout << "Please enter the body of your message followed by a line with only '.' when finished: " << endl;
         string data;
        while(2+2 == 4) {
            getline(cin, data);
            if(data == ".") {
                data+="\r\n";
                writeToServer(sock, data, data.length());
                break;
            }
            data+= "\r\n";
            writeToServer(sock, data, data.length());
        }
        readFromServer(sock, buf, BUFSIZE);

        output = "QUIT\r\n";
        writeToServer(sock, output, output.length());

        readFromServer(sock, buf, BUFSIZE);
        cout << "Email sent successfully" << endl;

        

        //pipeline out HELO + fromAddressHost
        //pipeline in response, check for response[0] == 2 || 3
        //pipeline out RCOT TO:<toAddress>
        //pipeline in response, check for respone[0] == 2 || 3
        //pipeline DATA

        /*
        string subjectLine;
        cout << "Please provide a subject line for this email: " << endl;
        getline(cin, subjectLine);
        
        string body;
        string tempBody;
        cout << "Enter the body of your email: " << endl;
        while(tempBody != "."){
        getline(cin, tempBody);
        */

        //pipeline out SUBJECT: subjectLine \r TO: toAddress \r FROM: fromAddress \r\r body \r .
        



        // Make a socket
        /*
	    int sock = MakeSocket(argv[1], argv[2]);
	    cout << "socket is " << sock << endl;
	    assert(sock != -1);
       
        */

       return 0;
    }
	
	

	


