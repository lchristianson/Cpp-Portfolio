#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

using namespace std;


int sizeOut;

int main(int argc, char* argv[]) {

    //Prints all of argv/////////////////////////////////////////////////////////////////////
    for(int i = 0; i < argc; i++) {
        cout << argv[i] << endl;
    }
    //Creates array of FD's for input files./////////////////////////////////////////////////
    int* fd = new int[argc-2];

    //Creates array of input sizes for use in copying data.///////////////////////////////////
    int* sizes = new int[argc-2];

    //Opens and tests validity of each input file. /////////////////////////
    //Then runs stat to obtain file size and stores in variable size.////////////////////////
    struct stat infoArgs = {0};
    for(int i = 0; i < (argc - 2); i++) {
        fd[i] = open(argv[i+1], O_RDONLY);
        if(fd[i] == -1){
            cout << "Input File Error, Killing Self......" << endl;
            exit(1);
        }
        if(stat(argv[i+1], &infoArgs) == 0){
            sizeOut += infoArgs.st_size;
            sizes[i] = infoArgs.st_size;
        } else {
            cout << "Size grabber error, killing self..." << endl;
            exit(1);
        }
    }

    int totalSize = 0;
    for (int i = 0; i < argc-2; i++)
        totalSize += sizes[i];
    if (totalSize == sizeOut) cout << "totalSize is good" << endl;
    else cout << "totalSize is bad" << endl;

    //create arrays for input file addresses//////////////////////////////////////////////
   
    void** maps = new void*[argc-2];
    
    
    //Open, get size, and map each input file////////////////////////////////////////////////////////////////

    for(int i = 0; i < argc - 2; i++) {
        maps[i] = mmap(NULL, infoArgs.st_size, PROT_READ, MAP_SHARED, fd[i], 0);
        if(maps[i] == MAP_FAILED){
            cout << "Error mapping argument file, killing self..." << endl;
            exit(1);
        }
        close(fd[i]);
    }

    //Open's output file and checks if already exists//////////////////////////////////////////////////////////
    int out = open(argv[argc-1], O_RDWR | O_CREAT | O_EXCL, 0666);
    cout << argv[argc-1] << " is the output file name" << endl;
    if(out == -1){
        cout << "Error opening output file, or output already exists.  Tying a noose...." << endl;
        exit(1);
    }
    if(ftruncate(out, sizeOut) == -1) {
        cout << "Error truncating output file, dying a painful death...." << endl;
        exit(1);
    }

    //Maps output file./////////////////////////////////////////////////////////////////////////////
    void* outAddr = mmap(NULL, sizeOut, PROT_WRITE, MAP_SHARED, out, 0);

    //Copys eachfile of inputs to output location/////////////////////////////////////////////////////////
    int offset = 0;
    for(int i = 0; i < argc - 2; i++) {
        memcpy((char*)outAddr + offset, maps[i], sizes[i]);
        offset += sizes[i];
    }

    
}