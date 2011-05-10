//SMTP CLIENT 

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include "client_functions.h"

using namespace std;

//DECLARATIONS

// SOCKETS
SOCKET sock,client;
 
int main()
{
          
    //Declarations
    DWORD poll;
    int res,i=1,port=999;
    char buf[100];
    //char msg[100] = "";
    char ip[15];
    WSADATA data;
     
    cout<<"SMTP Client ";
    
    while(1)
    {
        printMenu(); 
        cout<<"\n\n\nEnter IP to connect to: ";
        gets(ip);
     
        sockaddr_in ser;
        sockaddr addr;
    
        memcpy(&addr,&ser,sizeof(SOCKADDR_IN));
     
        sock = createSocket(ser, *ip, data);    
     
        res=connect(sock,&addr,sizeof(addr));               //Connect to the server
            if(res !=0 )
            {
                s_cl("SERVER UNAVAILABLE",res);
            }
            else
            {
                cout<<"\nConnected to Server: ";
                memcpy(&ser,&addr,sizeof(SOCKADDR));
            }
     
        char RecvdData[100] = "";
        int ret;
    
        ret = recv(sock,RecvdData,sizeof(RecvdData),0);
            if(ret > 0)
            {
                cout<<endl<<RecvdData;
                strcpy(RecvdData,"");
	        }
 
        while(true)
        {
            strcpy(buf,"");
            cout<<"\nEnter message to send ->\n";
            fgets(buf,sizeof(buf),stdin);
          
            Sleep(5);
            res = send(sock,buf,sizeof(buf),0);
          
            if(res==0)
            { 
                //0==other side terminated conn
                printf("\nSERVER terminated connection\n");
                Sleep(40);
                closesocket(client);
                client = 0;
                break;
            }
            else if(res==SOCKET_ERROR)
            { 
                //-1 == send error
                printf("Socket error\n");
                Sleep(40);
                break;
            }
        
            ret = recv(sock,RecvdData,sizeof(RecvdData),0);
            if(ret > 0)
            {
                cout<<endl<<RecvdData;
                strcpy(RecvdData,"");
            }
        }  
     
        closesocket(client);
        WSACleanup();
    }
}
