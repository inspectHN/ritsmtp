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
 
void s_cl(char *a, int x)
{
    cout<<a;
}
 
 
int main()
{
          
    //Declarations
    DWORD poll;
    int res,i=1,port=999;
    char buf[1000];
    //char msg[100] = "";
    char ip[15];
    WSADATA data;
     
    cout<<"SMTP Client\n\n\n";
    
    while(1)
    {
        printMenu(); 
        cout<<"Enter IP to connect to: ";
        gets(ip);
     
        sockaddr_in ser;
        sockaddr addr;
     
     
        ser.sin_family=AF_INET;
        ser.sin_port=htons(2525);                    //Set the port
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
 
        //while(true)
        {
            char *str = "HELO ";
            
            strcpy(buf,"");
            cout<<"\nEnter hostname\n";
            fgets(buf,sizeof(buf),stdin);
            
            strcat(str, buf);          
            Sleep(5);
            res = send(sock,str,sizeof(str),0);
          
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
            
            // Entering MAIL FROM:
            char *from = "MAIL FROM:<";
            strcpy(buf,"");
            cout<<"\nEnter your email address\n";
            fgets(buf,sizeof(buf),stdin);
            
            strcat(from,buf);
            strcat(from,">");          
            Sleep(5);
            res = send(sock,from,sizeof(from),0);
          
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
            // End of MAIL FROM
            
            // RCPT TO:
            while(1)
            {
                char *rcpt = "RCPT TO:<";
                strcpy(buf,"");
                cout<<"\nEnter recipient's email address. When done adding recipients, enter DATA\n";
                fgets(buf,sizeof(buf),stdin);
                
                if(buf == "DATA")
                {
                    break;
                }
                
                strcat(rcpt, buf);
                strcat(rcpt, ">");          
                Sleep(5);
                res = send(sock,rcpt,sizeof(rcpt),0);
          
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
            // Ending of RCPT TO while loop
            
            // Send "DATA" message to server
            char *DATA = "DATA";
                       
            Sleep(5);
            res = send(sock,DATA,sizeof(DATA),0);
          
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
            // End of sending the message "DATA" to server
            
            // Entering Subject of message            
            char *subject = "Subject: ";
            strcpy(buf,"");
            cout<<"\nEnter Subject(leave blank if none is needed)\n";
            fgets(buf,sizeof(buf),stdin);
                
            strcat(subject, buf);         
            Sleep(5);
            res = send(sock,subject,sizeof(subject),0);
          
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
            // Ending Subject of message
            
            // Enter message body
            strcpy(buf,"");
            cout<<"\nEnter message you wish to send. Place DONE on a line by itself when you are finished\n";
            
            while(1)
            {
                strcpy(buf,"");
                cout << "--> ";
                
                string line;
                getline(cin, line);
                
                if(line == "DONE")
                {
                    break;
                }
                else if(line == ".")
                {
                    line = "..";
                }
                
                
                const char *message = line.c_str();
                //strcat(buf,message);
                Sleep(5);
                res = send(sock,message,sizeof(message),0);
                
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
            }
            char *ending = ".";         
            Sleep(5);
            res = send(sock,ending,sizeof(ending),0);
          
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
            // Done sending message
        }  
     
        closesocket(client);
        WSACleanup();
    }
}
