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
        //printMenu(); 
        cout<<"Enter IP to connect to: ";
        gets(ip);
        //fgets(ip,sizeof(ip),stdin);
        sockaddr_in ser;
        sockaddr addr;
     
     
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
            char str[100] = "HELO ";
            
            strcpy(buf,"");
            cout<<"\nEnter hostname\n";
            fgets(buf,sizeof(buf),stdin);
            cout << "Before strcat\n\n";
            strcat(str, buf);          
            cout << str << endl << endl;
            
            Sleep(5);
            res = send(sock,str,sizeof(str),0);
          cout << "Sent message\n\n";
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
            char from[100] = "MAIL FROM:<";
            strcpy(buf,"");
            cout<<"\nEnter your email address\n";
            fgets(buf,sizeof(buf),stdin);
            
            // This portion doesn't make much sense, just do it.
            int len = strlen(buf);
            buf[len-1] = '\0';
            // Ending of non-sense
            
            strcat(from,buf);
            strcat(from,">");
            
            // Another line of non-sense
            strcat(from,"\n");
            // Ending this line of non-sense
                 
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
                char rcpt[100] = "RCPT TO:<";
                strcpy(buf,"");
                cout<<"\nEnter recipient's email address. When done adding recipients, enter DATA\n";
                fgets(buf,sizeof(buf),stdin);
                
                if(strcmp(buf,"DATA\n") == 0)
                {
                    break;
                }
                
                // This portion doesn't make much sense, just do it.
                int len = strlen(buf);
                buf[len-1] = '\0';
                // Ending of non-sense
            
                strcat(rcpt, buf);
                strcat(rcpt, ">");          
                
                // Another line of non-sense
                strcat(rcpt,"\n");
                // Ending this line of non-sense
                
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
            char DATA[10] = "DATA\n";
                       
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
//            char subject[100] = "Subject: ";
//            strcpy(buf,"");
//            cout<<"\nEnter Subject(leave blank if none is needed)\n";
//            fgets(buf,sizeof(buf),stdin);
//                
//            strcat(subject, buf);         
//            Sleep(5);
//            res = send(sock,subject,sizeof(subject),0);
//          
//            if(res==0)
//            { 
//                //0==other side terminated conn
//                printf("\nSERVER terminated connection\n");
//                Sleep(40);
//                closesocket(client);
//                client = 0;
//                break;
//            }
//            else if(res==SOCKET_ERROR)
//            { 
//                //-1 == send error
//                printf("Socket error\n");
//                Sleep(40);
//                break;
//            }
            // Ending Subject of message
            
            // Enter message body
            strcpy(buf,"");
            cout<<"\nEnter message you wish to send. Place DONE on a line by itself when you are finished\n";
            
            while(1)
            {
                cout << "--> ";
                fgets(buf,sizeof(buf),stdin);
                
                if(strcmp(buf,"DONE\n") == 0)
                {
                    break;
                }
                else if(strcmp(buf,".\n") == 0)
                {
                    strcpy(buf,"");
                    strcat(buf,"..\n");
                }
                
                cout << buf << endl;
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
        
            for(int i=0; i<1000; i++)
            {
                buf[i] = '\0';    
            }
            }
            char *ending = ".\n";         
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
