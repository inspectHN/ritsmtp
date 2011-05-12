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
    int res,i=1,port=999,error;
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
        //fgets(ip,sizeof(ip),stdin);
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
        
            receiveData(sock);
        
            char str[100] = "HELO ";
            
            strcpy(buf,"");
            cout<<"\nEnter hostname\n";
            fgets(buf,sizeof(buf),stdin);
            cout << "Before strcat\n\n";
            strcat(str, buf);          
            cout << str << endl << endl;
            
            Sleep(5);
            res = send(sock,str,sizeof(str),0);
          //cout << "Sent message\n\n";
            error = errorCheck(res);
            receiveData(sock);
            if(error==1)
            {
                 break;
            }
            // End of HELO message
            
            // Entering MAIL FROM:
            char from[100] = "MAIL FROM:<";
            strcpy(buf,"");
            cout<<"\nEnter your email address\n";
            fgets(buf,sizeof(buf),stdin);
            
            // This portion doesn't make much sense, just do it.
            	//you have to append a null char in place of the newline char from fgets
            int len = strlen(buf);
            buf[len-1] = '\0';
            // Ending of non-sense
            
            strcat(from,buf);
            strcat(from,">\n");
            
            // Another line of non-sense
          //  strcat(from,"\n");
            // Ending this line of non-sense
                 
            Sleep(5);
            res = send(sock,from,sizeof(from),0);
            
            error = errorCheck(res);
            receiveData(sock);
            if(error==1)
            {
                 break;
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
                //same as above, replace newline with null
                int len = strlen(buf);
                buf[len-1] = '\0';
                // Ending of non-sense
            
                strcat(rcpt, buf);
                strcat(rcpt, ">\n");          
                
                // Another line of non-sense
               // strcat(rcpt,"\n");
                // Ending this line of non-sense
                
                Sleep(5);
                res = send(sock,rcpt,sizeof(rcpt),0);
                error = errorCheck(res);
                receiveData(sock);
                if(error==1)
                {
                     break;
                }
                
            } //END OF FIRST WHILE TRUE LOOP
            // Ending of RCPT TO while loop
            
            // Send "DATA" message to server
            char DATA[10] = "DATA\n";
                  
            Sleep(5);
            res = send(sock,DATA,sizeof(DATA),0);
            
            error = errorCheck(res);
            if(error==1)
            {
                 break;
            }
            receiveData(sock);
            // End of sending the "DATA" command
            
            // Starting of entering From: for from field of email
            char fromField[300] = "From:\"";
            strcpy(buf,"");
            cout<<"\nEnter the name of the person this message is from\n";
            fgets(buf,sizeof(buf),stdin);
            
            // This portion doesn't make much sense, just do it.
            //you have to append a null char in place of the newline char from fgets
            len = strlen(buf);
            buf[len-1] = '\0';
            // Ending of non-sense
            
            strcat(fromField,buf);
            strcat(fromField,"\" ");
            
            char arrow[200] = "<";
            strcpy(buf,"");
            cout<<"\nEnter the email address of the person this message is from\n";
            fgets(buf,sizeof(buf),stdin);
            
            strcat(arrow,buf);
            // This portion doesn't make much sense, just do it.
            //you have to append a null char in place of the newline char from fgets
            len = strlen(arrow);
            arrow[len-1] = '\0';
            // Ending of non-sense
            // Another line of non-sense
          //  strcat(from,"\n");
            // Ending this line of non-sense
            
            strcat(arrow,">\n");
            strcat(fromField,arrow);     
            Sleep(5);
            res = send(sock,fromField,sizeof(fromField),0);
            error = errorCheck(res);
            if(error==1)
            {
                 break;
            }
            receiveData(sock);
            // End of From field (From:"name here" <email here>
            
            // Starting of entering To: for to field of email
            char toField[300] = "To:\"";
            strcpy(buf,"");
            cout<<"\nEnter the name of the person this message is to\n";
            fgets(buf,sizeof(buf),stdin);
            
            // This portion doesn't make much sense, just do it.
            //you have to append a null char in place of the newline char from fgets
            len = strlen(buf);
            buf[len-1] = '\0';
            // Ending of non-sense
            
            strcat(toField,buf);
            strcat(toField,"\" ");
            
            char temp[200] = "<";
            strcpy(buf,"");
            cout<<"\nEnter the email address of the person this message is to\n";
            fgets(buf,sizeof(buf),stdin);
            
            strcat(temp,buf);
            // This portion doesn't make much sense, just do it.
            //you have to append a null char in place of the newline char from fgets
            len = strlen(temp);
            temp[len-1] = '\0';
            // Ending of non-sense
            // Another line of non-sense
          //  strcat(from,"\n");
            // Ending this line of non-sense
            
            strcat(temp,">\n");
            strcat(toField,temp);     
            Sleep(5);
            res = send(sock,toField,sizeof(toField),0);
            error = errorCheck(res);
            if(error==1)
            {
                 break;
            }
            receiveData(sock);
            // End of To field (To:"name here" <email here>
            
            // Beginning of sending the message 
            strcpy(buf,"");
            cout<<"\nEnter message you wish to send.\nPlace DONE on a line by itself when you are finished\n";
            
            while(1)
            {
                cout << "--> ";
                fgets(buf,sizeof(buf),stdin);
                
                if(strcmp(buf,"DONE\n") == 0)
                {
                    break;
                }
                if(strcmp(buf,".\n") == 0)
                {
                    strcpy(buf,"");
                    strcat(buf,"..\n");
                }
                
                cout << buf << endl;
                Sleep(5);
                res = send(sock,buf,sizeof(buf),0);
                error = errorCheck(res);
                if(error==1)
                {
                    break;
                }
                Sleep(5);
                receiveData(sock);    
			}
            
            for(int i=0; i<1000; i++)
            {
                buf[i] = '\0';    
            }

            char *ending = ".\n";         
            Sleep(5);
            res = send(sock,ending,sizeof(ending),0);
            error = errorCheck(res);
            if(error==1)
            {
                 break;
            }
            receiveData(sock);
            
            strcpy(buf,"");
            strcpy(buf,"QUIT\n");
            res = send(sock,buf,sizeof(buf),0);
            error = errorCheck(res);
            if(error==1)
            {
                 break;
            }
            receiveData(sock);   
            // Done sending message
          
        closesocket(client);
        WSACleanup();
    }
}
