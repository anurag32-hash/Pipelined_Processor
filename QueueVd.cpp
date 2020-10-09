//#include "QueueVd.h"
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include <cstddef>

using namespace std;

struct QueueVd{
string queue[20];
int capacity,currentSize,rear;

QueueVd(){

	 capacity = 20;
	 currentSize =0;
	 rear = -1;
}

void push (string node) {
       if(isFull()){
         return;
       }
       else if(isEmpty()){
        rear=0;
        queue[0]=node;
        currentSize+=1;
		}
       else{
		currentSize+=1;
		rear=((rear)+1);
		queue[rear]=node;
    }
}
 string pop() {

		   string min = queue[0];

           for(int j=0;j<rear;j++){
            queue[j]= queue[j+1];
           }
           currentSize=currentSize-1;
           rear=rear-1;
           return min;
    }

int size() {
       return currentSize;
    }
bool isEmpty() {
       return (currentSize==0);
    }
bool isFull() {
       return (currentSize==capacity);
    }
void display(){

      if (isEmpty()) {
            cout << "Queue is empty" << endl;
    }
        for(int i =0; i<currentSize;i++){

            cout << queue[i] << endl;

        }
    }
};
