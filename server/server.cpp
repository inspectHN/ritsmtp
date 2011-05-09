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
#include <fnctl.h>
#pragma comment(lib, "Ws2_32.lib")
#define SERVER_PORT 25
#define LSIDE 
#define RSIDE
#define OUR_DOMAIN "droptables.com"


using namespace std;

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
  char rcvbuf[100];
  // buffer to hold our sent data
  char sendData[100];
  // for error checking
  int res;
  int flags = fcntl(current_client, F_GETFL);
  int result = fcntl(current_client, F_SETFL, flags & ~O_NONBLOCK);


   
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

			if((strcmp(rcvmsg,".") == 0))
			{
				strncpy(sendData,"250 Ok:",7);
				strcat(sendData, " Queued as: ");
				strcat(sendData, itoa(threadID, ThrdID, 10));
				send(current_client,sendData,sizeof(sendData),0);
			}
			if((strcmp(rcvmsg,"QUIT") == 0))
		{
			strcpy(sendData,"221 Bye");
			send(current_client,sendData,sizeof(sendData),0);
			fout.close();
			ExitThread(0);
		}
		if((strcmp(rcvmsg,"DATA") == 0))
		{
			strcpy(sendData,"354 End data with <CR><LF>.<CR><LF>");
			send(current_client,sendData,sizeof(sendData),0);
			strcpy(sendData,"");
		send(current_client,sendData,sizeof(sendData),0);
		}
	}
// 		
//        //create copy of rcvbuf
//		char rcvcopy[100];
//		strcpy(rcvcopy,rcvmsg); // create a copy for modification
//		char fromAddress[strlen(rcvcopy)];//Create a variable to store the from address
//		char toDomain[strlen(rcvcopy)]; //Create a new char array to store rcpt address	
//		int rcptCtr = 0;
//		//check if client says hello or a varient thereof
//        if((strncmp(rcvcopy,"HELO",4) == 0) || (strncmp(rcvcopy,"EHLO",4) == 0))
//		{		
//			
//			cout << "Initialized connection from: " << inet_ntoa(cAddress.sin_addr) << endl;
//			//create the string to send back to the client
//			strcpy(sendData, "250  ");
//			//int len1 = strlen(rcvmsg);
//			
//			strncat(sendData, rcvmsg, (strlen(rcvmsg)));
//		//	strcat(sendData, inet_ntoa(cAddress.sin_addr));
//			strcat(sendData, ". Thanks for joining my server.");
//			send(current_client,sendData,sizeof(sendData),0);
//		//	fout << sendData << "\n";
//			//Empty the send buffer
//			for (int i = 0; i <100; i++)
//			{
//				sendData[i] = '\0';
//			}
//			
//		//	strcpy(rcvcopy,rcvmsg);
//		}//If client says bye or end or quit, close the thread.
//		else if((strcmp(rcvcopy,"QUIT") == 0))
//		{
//			strcpy(sendData,"221 Bye");
//		//	fout << "\"" << "Sent Message: \",\"" << sendData << "\"\n";
//			send(current_client,sendData,sizeof(sendData),0);
//			fout.close();
//			ExitThread(0);
//			
//		
//		}//Check for MAIL FROM And store the from address into a variable
//		else if((strncmp(rcvcopy,"MAIL FROM:",10) == 0)){			
//			strncpy(fromAddress,rcvcopy,strlen(rcvcopy));
//			strcpy(sendData, "250 Ok");
//			send(current_client,sendData,sizeof(sendData),0);
//		}//Check for RCPT TO and see where it is destined for and route it properly
//		else if((strncmp(rcvcopy,"RCPT TO:",8) == 0))
//		{
//			
//			strcpy(toDomain,(strstr(rcvcopy,"@")+1)); //copy string from char after @ to end
//			toDomain[(strlen(toDomain))-1]='\0'; //append null char over the last > to terminate the string
//			if(strcmp(toDomain,OUR_DOMAIN) == 0)
//			{
//				
//			cout << "OUR DOMAIN!!!!" << endl;
//			
//			}
//		}
//		else{
//		
//			strcpy(sendData,"");
//			strcpy(sendData,rcvmsg);
//			strcpy(rcvmsg,"");
//		//	fout << "\"" << "Sent Message: \",\"" << sendData << "\"\n";
//			send(current_client,sendData,sizeof(sendData),0);
//		} 
//          
//          
//          
//          
//     
//                              
//      
//     // clear buffers
//       strcpy(sendData,"");
//       strcpy(rcvbuf,"");
//       strcpy(rcvcopy,"");
//      // cout << "at end, sendData, rcvbuf, rcvcpy: " << sendData << " , " << rcvbuf << " , " << rcvcopy << endl; //DEBUGGING
//  	}
  }  
}
 
int main(int argc, char *argv[])
{
 printf("Starting up multi-threaded TCP server\n");
 //seed random number generator
 srand(time(NULL));
 u_long iMode = 1;

 SOCKET sock;
  
 // for our thread
 DWORD thread;
  
 WSADATA wsaData;
 
 sockaddr_in server;
 
  
 // start winsock
 int ret = WSAStartup(0x202,&wsaData); // use highest version of winsock avalible
// int iResult = ioctlsocket(sock, FIONBIO, &iMode);
//  cout << "iresult: " << iResult << endl;
// if (iResult != NO_ERROR)
//  printf("Error at WSAStartup()\n");
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
   
 // bind our socket to a port(port 123)
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
