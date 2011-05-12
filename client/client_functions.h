#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>
using namespace std;

// Prints menu and gets choice
void printMenu();
void sendData(SOCKET, char*);
void receiveData(SOCKET);
int errorCheck(int);
