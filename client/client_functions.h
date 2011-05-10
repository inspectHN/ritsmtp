#include <iostream>
#include <string>
#include "WinSock2.h"
using namespace std;

// Prints menu and gets choice
void printMenu();
SOCKET createSocket(sockaddr_in&, char, WSADATA);
void s_cl(char*, int);
