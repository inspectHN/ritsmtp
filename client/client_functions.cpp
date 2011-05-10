#include "client_functions.h"

// Print menu and get choice
void printMenu()
{
    int choice;
    
    cout << "1) Send an Email" << endl << "2) Quit" << endl;
    
    cin >> choice;
    if(choice == 2)
    {
        exit(1);
    }
}

SOCKET createSocket(sockaddr_in &ser, char ip[15])
{
       //sockaddr addr;
     
        ser.sin_family=AF_INET;
        ser.sin_port=htons(25);                    //Set the port
        ser.sin_addr.s_addr=inet_addr(ip);      //Set the address we want to connect to
     
        memcpy(&addr,&ser,sizeof(SOCKADDR_IN));
     
        res = WSAStartup(MAKEWORD(1,1),&data);      //Start Winsock
 
        if(res != 0)
            s_cl("WSAStarup failed",WSAGetLastError());
 
        sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);       //Create the socket
            if(sock==INVALID_SOCKET )
                s_cl("Invalid Socket ",WSAGetLastError());
            else if(sock==SOCKET_ERROR)
                s_cl("Socket Error)",WSAGetLastError());
            else
                cout<<"Socket Established"<<endl;
 
        return sock;        
       }

