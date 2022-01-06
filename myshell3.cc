#include<string>
#include<iostream>
#include<sstream>
#include<stdio.h>
#include<unistd.h>
#include<dirent.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<bits/stdc++.h>
#include<wordexp.h>

using namespace std;

int readlinecount = 0;
int envSize = 0;
int redsize = 0;

string* getEnvArray(string s, string c) {
    istringstream stream(s);
    string word;
    string* r = new string[100];
    envSize = 0;
    int p = 0;
    while (getline(stream, word, ':')) {
        r[p] = word + "/" + c;
        p++;
    }
    envSize = p;
    return r;
}

void inputRedirect(string* c, int j) {
    pid_t cpid = fork();
    if(cpid == 0) {
        int fd = open(c[j+1].c_str(), O_RDONLY, 0666);
        if(fd < 0){
            cout << "An error occurred with the fd for input redirect" << endl;
            return;
        }
        dup2(fd, 0);
        close(fd);
        const char * loc = getenv("PATH");
            int e;
            if(loc == NULL) {
                cout << "Error getting path environment." << endl;
            }
            else {
                string paths(loc);
                string* envArray = getEnvArray(paths, c[0].c_str());
                char** execargs;
                execargs = new  char*[j+1];
                for(int i = 0; i < j; i++) {
                    int n = c[i].length();
                    char temp[j+1];
                    strcpy(temp,c[i].c_str());
                    execargs[i] = temp;
                }
                execargs[j] = nullptr;
                    
                for(int i = 0; i < envSize; i++) {
                    e = execv(envArray[i].c_str(), execargs);
                }
                exit(0);
            }
    } else {
        waitpid(cpid, NULL, 0);
        return;
    }
    
}

void outputRedirect(string* c, int j) {
    pid_t cpid = fork();
    if(cpid == 0) {
        int fd = open(c[j+1].c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
        if(fd < 0){
            cout << "An error occurred with the fd for output redirect" << endl;
            return;
        }
        dup2(fd, 1);
        close(fd);
        const char * loc = getenv("PATH");
            int e;
            if(loc == NULL) {
                cout << "Error getting path environment." << endl;
            }
            else {
                string paths(loc);
                string* envArray = getEnvArray(paths, c[0].c_str());
                char** execargs;
                execargs = new  char*[j+1];
                for(int i = 0; i < j; i++) {
                    int n = c[i].length();
                    char temp[j+1];
                    strcpy(temp,c[i].c_str());
                    execargs[i] = temp;
                }
                execargs[j] = nullptr;
                    
                for(int i = 0; i < envSize; i++) {
                    e = execv(envArray[i].c_str(), execargs);
                }
                exit(0);
            }
    } else {
        waitpid(cpid, NULL, 0);
        return;
    }
    
}

void appendRedirect(string* c, int j) {
    pid_t cpid = fork();
    if(cpid == 0) {
        int fd = open(c[j+1].c_str(), O_RDWR | O_APPEND, 0666);
        if(fd < 0){
            cout << "An error occurred with the fd for output append redirect" << endl;
            return;
        }
        dup2(fd, 1);
        close(fd);
        const char * loc = getenv("PATH");
            int e;
            if(loc == NULL) {
                cout << "Error getting path environment." << endl;
            }
            else {
                string paths(loc);
                string* envArray = getEnvArray(paths, c[0].c_str());
                char** execargs;
                execargs = new  char*[j+1];
                for(int i = 0; i < j; i++) {
                    int n = c[i].length();
                    char temp[j+1];
                    strcpy(temp,c[i].c_str());
                    execargs[i] = temp;
                }
                execargs[j] = nullptr;
                    
                for(int i = 0; i < envSize; i++) {
                    e = execv(envArray[i].c_str(), execargs);
                }
                exit(0);
            }
    } else {
        waitpid(cpid, NULL, 0);
        return;
    }

}

void fileRedirect(char* prompt) {
    string s(prompt);
    stringstream stream(s);
    string* commands = new string[15];
    int i = 0;
    string word;
    while(stream >> word) {
        commands[i] = word;
        cout << commands[i] << endl;
        i++;
    }
    redsize = i;
    for(int j = 0; j < redsize + 1; j++) {
        if(commands[j] == "<"){
            inputRedirect(commands, j);
            return;
        }
        else if(commands[j] == ">") {
            outputRedirect(commands, j);
            return;
        } else if(commands[j] == ">>"){
            appendRedirect(commands, j);
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    wordexp_t p;
    bool running = true;
    char* prompt;
    char path[256];
    string* scommands; 
    bool env;
    getcwd(path, 256);
    string sp(path);
    sp = sp + ">";                //Gets current path for readline prompt.
    setenv("PS1", sp.c_str(), 1);
    bool red;



    ofstream historyout;
    historyout.open("history.txt", ios::out | ios::app);      //opens history.txt for output.
    ifstream historyin;
    historyin.open("history.txt", ios::in);
    string temp;
    if(historyin.is_open()){
        while(historyin >> temp) {          //////loads in history from history.txt if any.
            add_history(temp.c_str());
        }
    historyin.close();
    }
    
    while(running) {
        int stdout;
        int stdin;
        env = false;
        red = false;
       

        prompt = readline(getenv("PS1"));   //gets the user command
        
        if(strlen(prompt) > 0) {  //Adds user entry to readline history,
            add_history(prompt);
            if(historyout.is_open()) {
                historyout << prompt << endl; //outputs entry to history.txt
            }
        } else {                  //or skips loop if nothing entered.
            continue;
        }

        for(int i = 0; i < strlen(prompt); i++) {
            if(prompt[i] == '>' || prompt[i] == '<') {   //checks for > or < which would indicate a redirect
                stdin = dup(0);
                stdout = dup(1);
                fileRedirect(prompt);   //send the command to the fileRedirect processor
                red = true;          //redirect = true;
                break;
            }
        }

        if(red) {       //if a redirect occured, fix the fd's and continue.
            dup2(stdin, 0);
            dup2(stdout, 1);
            close(stdin);
            close(stdout);
            continue;
        }

        int w = wordexp(prompt, &p, 0);   //expand the user entry with wordexp and stor in p
        if(p.we_wordc == 0) {               //if wordc is 0, nothing to do so continue.
            continue;
        } else {
            scommands = new string[p.we_wordc];    //create array of strings from wordexp structure for ease of use.
            for(int i = 0; i < p.we_wordc; i++) {
                string temp(p.we_wordv[i]);
                scommands[i] = temp;
            }
        }
        for(int i = 0; i < p.we_wordc; i++) {  // checks commands for an equal sign indicating that an env variable will be changed or added.
            cout << scommands[i] << endl;
            if(scommands[i] == "=") 
                env = true;
        }

        if(env) {                    ///////////////////////env command is in the form: Variable = value//////////////////////////
            int e = setenv(scommands[0].c_str(), scommands[2].c_str(), 1);
            if(e < 0) {
                cout << "error ocurred setting env variable" << endl;
            }
            continue;
        }


        if(scommands[0] == "cd") {       ///////////if first command is cd, change directories accordingly
            if((chdir(scommands[1].c_str())) == -1)
                cout << "ERROR - Failed while changing directories." << endl;
        } 
        else {
            int pid = fork();            ///if we make it this far, time to exec.
            if(pid == 0) {
                const char * loc = getenv("PATH");
                int e;
                if(loc == NULL) {
                    cout << "Error getting path environment." << endl;
                }
                else {
                    string paths(loc);
                    string* envArray = getEnvArray(paths, p.we_wordv[0]);
                    char** execargs;
                    execargs = new  char*[p.we_wordc+1];
                    for(int i = 0; i < (p.we_wordc); i++) {
                        execargs[i] = p.we_wordv[i];
                    }
                    execargs[p.we_wordc] = nullptr;
                    
                    for(int i = 0; i < envSize; i++) {
                        e = execv(envArray[i].c_str(), execargs);
                    }
                    
                }

                cout << "ERROR(" << e << ") - Fork failed to exec." << endl;
                exit(0);
            } 
            else {
                int junk;
                wait(&junk);
            }
            

        }
        
    }
}
