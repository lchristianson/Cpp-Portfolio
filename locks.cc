/*
One Producer thread time: real 0.586s    

Five Producer thread time: real 0.394s
                           
Ten Producer thread time: real 0.233s
            
*/

#include <iostream>
#include <string>
#include <mutex>
#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <condition_variable>

using namespace std;

int printcount = 0;
const int sz = 10;
int start = 0;
int stop = 0;
int* buffer;
condition_variable cv;
mutex mtx;

bool isEmpty() {
	return start == stop;
}

int prev(int x) {
	if (x == 0)
		return (sz - 1);
	else
		return (x-1);
}

bool isFull() {
	return prev(start) == stop;
}

int get() {
	std::unique_lock<mutex> lck(mtx);
	while (isEmpty()) {
		cv.wait(lck);
	}
	int r = buffer[prev(stop)];
	buffer[prev(stop)] = 0;
	cout << "Successfully got from buffer." << endl;
	stop = prev(stop);
	cv.notify_one();
	return r;
}

void put(int x) {
	unique_lock<mutex> lck(mtx);
	while (isFull())
		cv.wait(lck);
	buffer[stop] = x;
	cout << "Successfully added to buffer." << endl;
	if (stop < sz - 1)
		stop++;
	else
		stop = 0;
	cv.notify_one();
	}


bool isPrime(int maybe)
{
	if (maybe % 2 == 0)
		return false;
	for (int i = 3; i < maybe; i += 2)
		if (maybe % i == 0)
			return false;
	return true;
}
void makePrimes(int start, int stop)
{
	for (int i = start; i < stop; i++)
		if (isPrime(i))
			put(i);
}
void print()
{
	while (printcount < 81) {
    
		cout << "Prime " << get() << endl;
        printcount++;
    }
}

    int main(int argc, char* argv[]) {
        int size = atoi(argv[1]);
        int increment = 1000/size;
        int begin = 100001;
        buffer = new int[sz];
        for (int i = 0; i < sz; i++) {
            buffer[i] = 0;
        }
        thread* p = new thread[size];
        for(int i = 0; i < size; i++) {
            p[i] = thread(makePrimes, begin, begin+increment);
            begin += increment;
        }
        thread consumer = thread(print);
        for(int i = 0; i < size; i++) {
            p[i].join();
        }
        consumer.join();


        return 0;
    }