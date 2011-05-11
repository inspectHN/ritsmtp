// SMTP Multi-threaded server
//Modified code from mycourses download

#include <windows.h>
#include  <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <winsock.h>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <sstream>
#pragma comment(lib, "Ws2_32.lib")
#define SERVER_PORT 2525
#define OUR_DOMAIN "droptables.com"

DWORD WINAPI receive_cmds(LPVOID lpParam);
int sendMessage(sockaddr_in cAddress, char messageLog[]);


using namespace std;


//GLOBAL VARIABLES FOR IP ADDRESS
string LSIDE;
string RSIDE;
 
int main(int argc, char *argv[])
{
	
	if(argc != 3)
	{
		cout << "Not enough variables on the command line, please check and try again.\n\nUSAGE: PROGRAM NAME  LSIDE_IP RSIDE_IP\n\n\n";
		system("pause");
		return 1;
	}else
	{
		LSIDE = argv[1];
		RSIDE = argv[2];
	}

	
 printf("Starting up multi-threaded SMTP server\n");

 SOCKET sock;
  
 // for our thread
 DWORD thread;
  
 WSADATA wsaData;
 
 sockaddr_in server;
 
  
 // start winsock
 int ret = WSAStartup(0x202,&wsaData); // use highest version of winsock avalible

 if(ret != 0)
 {
    return 0;
 }
   
 // fill in winsock struct ...
 server.sin_family=AF_INET;
 server.sin_addr.s_addr=INADDR_ANY;
 server.sin_port=htons(SERVER_PORT); // listen on SMTP port 25
  
 // create our socket
 sock=socket(AF_INET,SOCK_STREAM,0);
  
 if(sock == INVALID_SOCKET)
 {
    return 0;
 }
   
 // bind our socket to a port(port 25)
 if( bind(sock,(sockaddr*)&server,sizeof(server)) !=0 )
 {
    return 0;
 }
   
 // listen for a connection 
 if(listen(sock,5) != 0)
 {
    return 0;
 }
  
 // socket that we sendrecv data on
 SOCKET client;
  
 sockaddr_in from;
 int fromlen = sizeof(from);
 

 // loop forever
 while(true)
 {
  // accept connections
  client = accept(sock,(struct sockaddr*)&from,&fromlen);
  printf("Client connected\r\n");
   
  // create our recv_cmds thread and parse client socket as a parameter
  CreateThread(NULL, 0,receive_cmds,(LPVOID)client, 0, &thread); 
 }
  
 // shutdown winsock
 closesocket(sock);
 WSACleanup();
  
 // exit
 return 0;
}
//==================================================================================================//

// our thread for recving commands
DWORD WINAPI receive_cmds(LPVOID lpParam)
{
  printf("thread created\r\n");
  
  // set our socket to the socket passed in as a parameter 
  SOCKET current_client = (SOCKET)lpParam;
	//Create new socket address struct
	sockaddr_in cAddress;
	//get length of socket address struct
	int len=sizeof(cAddress);
	//populate socket address struct for IP address later
	getpeername(current_client, (SOCKADDR*)&cAddress, &len);
  // buffer to hold our recived data
  char rcvbuf[1000];
  // buffer to hold our sent data
  char sendData[100];
  // for error checking
  int res;
  
  //clear all buffers
  for (int i = 0; i < 1000; i++)
  {
		rcvbuf[i]='\0';
		sendData[i] ='\0';
  }

   
   //LOGGING BEGIN
   
   
   //Get the current thread ID for logging
  int threadID = GetCurrentThreadId();
  
  //open master log file
  ofstream fout;
  fout.open("connection_log.csv", fstream::app);
  
  
  //Log the time of the connection
  	time_t seconds;
	struct tm * timeinfo;
	time(&seconds);
	timeinfo = localtime (&seconds);
	     
    //output "mm/dd/yyyy",
    fout << "\"" << timeinfo->tm_mon + 1 << "/" << timeinfo->tm_mday << "/" << timeinfo->tm_year + 1900 << "\"," ;
	//output "time hh:mm:ss"
	fout << "\"" << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec << "\",";
	//Output connection attempt
	fout << "\"" << "Thread ID:\"," << "\"" << threadID << "\",";
	fout << "\"" << "Client IP Address:\"," << "\"" << inet_ntoa(cAddress.sin_addr) << "\"\n";
	fout.close();
  
  
  //Create the name for the output file
  char messageLog[32];
  char ThrdID[16];
  strcpy(messageLog, "message_log_");
  strcat(messageLog, itoa(threadID, ThrdID, 10));
  strcat(messageLog, ".csv");
  
  cout << "Message log name for thread " << threadID << ": " << messageLog << endl;
  fout.open(messageLog);
  
  
  strcpy(sendData,"220 smtp.droptables.com\n");
  //cout << strlen(sendData) << "\n";
  send(current_client,sendData,strlen(sendData),0);
  fout << inet_ntoa(cAddress.sin_addr);
  // our recv loop
  while(true)
  {
	 
     res = recv(current_client,rcvbuf,sizeof(rcvbuf),0); // recv cmds
	 char rcvmsg[(strlen(rcvbuf)-1)];//CREATE NEW BUFFER WITH SIZE OF MESSAGE RECIEVED MINUS 1
	 strcpy(rcvmsg, ""); //EMPTY "RECIEVED MESSAGE"
	//FILL RECIEVED MESSAGE WITH RECIEVED BUFFER
	 strcpy(rcvmsg, rcvbuf);
	 int endln = strlen(rcvmsg);
	 rcvmsg[endln-1] = '\0';//APPEND A NULL TO THE END OF THE BUFFER
	 
	 //what did I receive
	 fout << endl << rcvmsg;//Output to file all data
     cout << "Received:" << rcvmsg << endl;// << ":stuff" << "\n";
     
    	
	 if (res != 0)
     {
		if((strncmp(rcvmsg,"HELO",4) == 0) || (strncmp(rcvmsg,"EHLO",4) == 0))
		{		
			
			cout << "Initialized connection from: " << inet_ntoa(cAddress.sin_addr) << endl;
			//create the string to send back to the client
			strcpy(sendData, "250  ");
			//int len1 = strlen(rcvmsg);
			
			strncat(sendData, rcvmsg, (strlen(rcvmsg)));
	
			strcat(sendData, ". Thanks for joining my server.");
			send(current_client,sendData,sizeof(sendData),0);
		
			//Empty the send buffer
			for (int i = 0; i <100; i++)
			{
				sendData[i] = '\0';
			}
			
		}
		if((strncmp(rcvmsg,"MAIL FROM:",10) == 0) || (strncmp(rcvmsg,"RCPT TO:",8) == 0))
		{
			strncpy(sendData,"250 Ok\0",7);
			send(current_client,sendData,sizeof(sendData),0);
		}

			
			if((strcmp(rcvmsg,"QUIT") == 0))
		{
			strcpy(sendData,"221 Bye");
			send(current_client,sendData,sizeof(sendData),0);
			fout.close();
			sendMessage(cAddress, messageLog); //STILL IN PROGRESS
			ExitThread(0);
		}
		if((strncmp(rcvmsg,"DATA",5) == 0))
		{
			
			strcpy(sendData,"354 End data with <CR><LF>.<CR><LF>");
			send(current_client,sendData,sizeof(sendData),0);
			strcpy(sendData,"");
			
			
			int check = 1;
	 		
			while(check != 0)
			{	
			res = recv(current_client,rcvbuf,sizeof(rcvbuf),0);
			rcvbuf[(strlen(rcvbuf))-1]='\0'; //remove newline char from end and replace it with a null char						
			check = strcmp(rcvbuf, ".");			
	
			
			fout << endl << rcvbuf;
			
			
			if(check != 0)
				send(current_client,sendData,sizeof(sendData),0);
						
			if(check == 0)
				{
					for(int i = 0; i < sizeof(sendData); i++)
					{
						sendData[i] = '\0';
					}
					strncpy(sendData,"250 Ok: Queued as ",18);
					strcat(sendData, itoa(threadID, ThrdID, 10));
					send(current_client,sendData,sizeof(sendData),0);
					
				}
      	  }
		}
	  }
    }  
}

//===================================================================================================================//

int sendMessage(sockaddr_in client, char messageLog[32])
{
	ifstream fin; //open message log for reading
	fin.open(messageLog);
	if(!fin.is_open())
	{
		cout << "error opening queued message: " << messageLog << ".  Check the file and try again.\n";
	}
	
	string dummy;
	int cnt=0;
	getline(fin, dummy); //dummy string for line counting
	cnt++;//number of lines in the file
	while(!fin.eof())//loop to count total lines in the file
	{
		getline(fin, dummy);
	//	cout << dummy << endl;
		cnt++;
	}
	
	string msg[cnt];//array of strings for the message
	fin.clear(); //clear the eof flag
	fin.seekg(0);//reset file pointer to beginning
	
	for(int i = 0; i < cnt; i++)//loop to read in lines
	{
		getline(fin, msg[i]);
	}
	
	int numRcpts = 0;
	int numLocalRcpt = 0;
	
	for(int i = 3; i < cnt; i++) //Count number of rcpts.
	{
         if( msg[i].substr(0,7) == "RCPT TO"  )
            numRcpts++;
    }

    bool myDomain = false; //set to true if any rcpt is OUR_DOMAIN
    bool otherDomain = false;
    for(int i = 0; i < numRcpts; i++) //check if any rcpts are for OUR_DOMAIN
	{
         if(  (msg[i+3].substr((msg[i+3].find_first_of("@")+1), 14)) == OUR_DOMAIN)//match domain name (start at @+1, go for 14 chars)
         {
            myDomain = true;
            numLocalRcpt++;            
         }
         else 
         {
                otherDomain = true;//if there are other domains, we need to know.
        
         }
    }
//  cout << "numLocalRcpt: " << numLocalRcpt << endl;//DEBUGGING
    string localRcpts[numLocalRcpt];//array for local users
    
    if(myDomain)//Create an array of local users to modify the save message log
    {
         int k = 0;
          for(int i = 0; i < numRcpts; i++) //check if any rcpts are for OUR_DOMAIN
         
	    {
             if(  (msg[i+3].substr((msg[i+3].find_first_of("@")+1), 14)) == OUR_DOMAIN)//match domain name (start at @+1, go for 14 chars)
             {
                localRcpts[k] = (msg[i+3].substr(9));
                localRcpts[k]=localRcpts[k].substr(0,(localRcpts[k].length()-1));//truncate the final ">"
                k++;
             }
        }
   }
    
    
    
    
    if(myDomain)//write out email to local user .eml file
     {
            for(int i = 0; i < numLocalRcpt; i++)
            {
                int id = GetCurrentThreadId();
                std::stringstream ss;
                ss << id;
                ofstream fout;
                string outputFile = localRcpts[i]; 
                outputFile += "_";
                outputFile+= ss.str();
                outputFile += ".eml";//These lines create a new output .eml file for each user who is a RCPT on an email
                fout.open(outputFile.c_str());
                for(int k = 0; k < cnt; k++)
                {
                    fout << msg[k] << endl;
                    k++;
                }
            }
                
            if(otherDomain)
            {
                    
                SOCKET sock;
                if((inet_ntoa(client.sin_addr)) != RSIDE)
                {
                
                sockaddr_in ser;
                sockaddr addr;
              
                ser.sin_family=AF_INET;
                ser.sin_port=htons(2525);            //Set the port
           
                ser.sin_addr.s_addr=inet_addr(RSIDE.c_str()); //Set the address we want to connect to
             
                memcpy(&addr,&ser,sizeof(SOCKADDR_IN));
                WSADATA data;
                int res = WSAStartup(MAKEWORD(1,1),&data);      //Start Winsock
         
                if(res != 0)
                    cout << "WSAStarup failed\n";
         
                sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);       //Create the socket
                    if(sock==INVALID_SOCKET )
                        cout << "INVALID SOCKET\n";
                    else if(sock==SOCKET_ERROR)
                        cout << "SOCKET ERROR\n";
                    else
                        cout<<"Socket Established"<<endl;//DEBUGGING
            
                res=connect(sock,&addr,sizeof(addr));               //Connect to the server
                    if(res !=0 )
                    {
                        cout << "SERVER UNAVAILABLE - CHECK PORT\n";
                    }
                    else
                    {
                        cout<<"\nConnected to Server: ";
                        memcpy(&ser,&addr,sizeof(SOCKADDR));
                    }
                }
                for(int i = 0; i < cnt; i++)
                {
                       // char str;
                        //str = msg[i].c_str();
                        int res = send(sock,msg[i].c_str(),sizeof(msg[i].c_str()),0);
                          //cout << "Sent message\n\n";
                            if(res==0)
                            { 
                                //0==other side terminated conn
                                printf("\nSERVER terminated connection\n");
                                Sleep(40);
                                closesocket(sock);
                                break;
                            }
                            else if(res==SOCKET_ERROR)
                            { 
                                //-1 == send error
                                printf("Socket error\n");
                                Sleep(40);
                                break;
                            }
                            char *RecvdData;
                           int ret = recv(sock,RecvdData,sizeof(RecvdData),0);
                }
            }
    }
    
    
            
	
	
	
	
	 
     
	
	return 0;
}
