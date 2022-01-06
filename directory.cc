#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <map>

using namespace std;

/*string getType(struct dirent d) {
   

} */

ino_t* iNodes = new ino_t[500];
int count = 0;
long totalSize = 0;
int e;


void printPermissions(struct stat i) {
    cout << "   Permission: ";
    printf( (S_ISDIR(i.st_mode)) ? "d" : "-");   //This is from: https://www.daniweb.com/programming/software-development
    printf( (i.st_mode & S_IRUSR) ? "r" : "-");  ///threads/440207/how-to-get-the-permission-of-a-file-using-stat
    printf( (i.st_mode & S_IWUSR) ? "w" : "-");  // It checks the digits of the permission code and prints the permission as r, w, or x.
    printf( (i.st_mode & S_IXUSR) ? "x" : "-");
    printf( (i.st_mode & S_IRGRP) ? "r" : "-");
    printf( (i.st_mode & S_IWGRP) ? "w" : "-");
    printf( (i.st_mode & S_IXGRP) ? "x" : "-");
    printf( (i.st_mode & S_IROTH) ? "r" : "-");
    printf( (i.st_mode & S_IWOTH) ? "w" : "-");
    printf( (i.st_mode & S_IXOTH) ? "x" : "-");
}

bool searchNodes(long k) {
    for(int i = 0; i < count; i++) {
        if(k == iNodes[i]){
            return true;
        }
    }
    return false;
    
}


void dir(const char * path){
    
    DIR * dirstream;
    dirstream = opendir(path);
    if(dirstream == NULL) {
        cout << "Error(" << errno << "), Error opening directory: " << path << endl;
        e = errno;
    }

    string type;
    long size;
    struct dirent * dstruct;
    struct stat info = {0};
    while ((dstruct = readdir(dirstream))  != NULL) {
        if(e == 13) {
            continue;
        }
        string pathstring(dstruct->d_name);
        if(pathstring == "." || pathstring == "..")
            continue;
        cout << path << "/" << dstruct->d_name << " ";
        string newPath(path);
        newPath = newPath + "/" + dstruct->d_name;
        
        if(dstruct->d_type == DT_DIR) {
            type = "[D]";
            if(!searchNodes(dstruct->d_ino)){
                int fd = open(newPath.c_str(), O_RDONLY);
                if(fd == -1) {
                    cout << "Error opening file descriptor." << endl;
                }
                if(fstat(fd, &info) == -1)
                    cout << "Error(" << errno << ") has occured during fstat" << endl; // error check
                size = info.st_size;
                printPermissions(info);
                iNodes[count] = dstruct->d_ino;
                count++;
                cout << " Type: " << type <<  endl << endl;
                dir(newPath.c_str());
            } else {
                int fd = open(newPath.c_str(), O_RDONLY);
                if(fd == -1) {
                    cout << "Error opening file descriptor." << endl;
                }
                if(fstat(fd, &info) == -1)
                    cout << "Error(" << errno << ") has occured during fstat" << endl; // error check
                size = info.st_size;
                printPermissions(info);
                iNodes[count] = dstruct->d_ino;
                count++;
                cout << " Type: " << type <<  endl << endl;
            }
            
        }
        else if(dstruct->d_type == DT_LNK) {
            type = "[L]";
            long bufsize = 1024;
            char buf[bufsize];
            buf[readlink(path, buf, bufsize)] = 0;
            string link(buf);
            cout << " --> " << link << endl;
            count++;
            cout << " Type: " << type << " Size: " << size << " Bytes" << endl << endl;
            iNodes[count] = dstruct->d_ino;
        }
        else if(dstruct->d_type == DT_REG) { 
            int fd = open(newPath.c_str(), O_RDONLY);
            if(fd == -1) {
                cout << "Error opening file descriptor." << endl;
                }
            if(fstat(fd, &info) == -1)
                cout << "Error(" << errno << ") has occured during fstat" << endl; // error check
            size = info.st_size;
            printPermissions(info);
            totalSize += size;
            type = "[F]";
            count++;
            cout << " Type: " << type << " Size: " << size << " Bytes" << endl << endl;
            iNodes[count] = dstruct->d_ino;
        }
        else {
            int fd = open(newPath.c_str(), O_RDONLY);
            if(fd == -1) {
                cout << "Error opening file descriptor." << endl;
            }
            if(fstat(fd, &info) == -1)
                cout << "Error(" << errno << ") has occured during fstat" << endl; // error check
            size = info.st_size;
            printPermissions(info);
            totalSize += size;
            type = "[Unknown]";
            count++;
            cout << " Type: " << type << " Size: " << size << " Bytes" << endl << endl;
            iNodes[count] = dstruct->d_ino;
        }
       

   }


}

int main(int argc, char** argv) {
    cout << "Creating Directory List at: " << argv[1] << "/" << endl << endl;
    iNodes = new __ino_t[500];
    dir(argv[1]);

    cout << "Total size of the whole directory is: " << totalSize << " B." << endl;
    delete iNodes;
    return 0;
}