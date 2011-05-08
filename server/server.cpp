// SMTP Multi-threaded server
//Modified code from mycourses download

#include <windows.h>
#include  <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <winsock.h>
#include <string>
#include <fstream>


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
     cout << "Received:" << rcvmsg << endl;// << ":stuff" << "\n";
     
     //LOG TIME AND MESSAGE RECEIVED
     //output "mm/dd/yyyy",
    fout << "\"" << timeinfo->tm_mon + 1 << "/" << timeinfo->tm_mday << "/" << timeinfo->tm_year + 1900 << "\"," ;
	//output "time hh:mm:ss"
	fout << "\"" << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec << "\",";
	//output other data
	fout << "\"" << "Thread ID:\"," << "\"" << threadID << "\",";
	fout << "\"" << "Client IP Address:\"," << "\"" << inet_ntoa(cAddress.sin_addr) << "\",";
	fout << "\"" << "Received Message: \"," << "\"" << rcvmsg << "\",";
     

     if (res != 0)
     {
 
        //create copy of rcvbuf in order to change it to lower case for comparison
		char rcvcopy[sizeof(rcvmsg)];
		strcpy(rcvcopy,rcvmsg);
        
          //copy to lowercase string
        for(int i = 0; i < sizeof(rcvcopy); i++)
        {
			rcvcopy[i]=tolower(rcvcopy[i]);
		}
		
		
		
		//check if client says hello or a varient thereof
        if((strcmp(rcvcopy,"hello") == 0) || (strcmp(rcvcopy,"helo") == 0) || (strcmp(rcvcopy,"ehlo") == 0))
		{		
		
			cout << "Initialized connection from: " << inet_ntoa(cAddress.sin_addr) << endl;
			//create the string to send back to the client
			strcpy(sendData, "250  ");
			int len1 = strlen(rcvmsg);
			
			strncat(sendData, rcvmsg, (len1-2));
		//	strcat(sendData, inet_ntoa(cAddress.sin_addr));
			strcat(sendData, ". Thanks for joining my server.");
			send(current_client,sendData,sizeof(sendData),0);
			fout << "\"" << "Sent Message: \",\"" << sendData << "\"\n";
			//Empty the send buffer
			for (int i = 0; i <100; i++)
			{
				sendData[i] = '\0';
			}
			
			
		}//If client says bye or end or quit, close the thread.
		else if((strcmp(rcvcopy,"quit") == 0))
		{
			strcpy(sendData,"221 Bye");
			fout << "\"" << "Sent Message: \",\"" << sendData << "\"\n";
			send(current_client,sendData,sizeof(sendData),0);
			fout.close();
			ExitThread(0);
			
		
		}//Otherwise, echo what the client said back to them
		else
		{
		
			strcpy(sendData,"");
			strcpy(sendData,rcvmsg);
			strcpy(rcvmsg,"");
			fout << "\"" << "Sent Message: \",\"" << sendData << "\"\n";
			send(current_client,sendData,sizeof(sendData),0);
		} 
          
          
          
          
          /* Stop putting your stuff here */  
                              
      
     // clear buffers
       strcpy(sendData,"");
       strcpy(rcvbuf,"");
       strcpy(rcvcopy,"");
      // cout << "at end, sendData, rcvbuf, rcvcpy: " << sendData << " , " << rcvbuf << " , " << rcvcopy << endl; //DEBUGGING
  	}
  }  
}
 
int main(int argc, char *argv[])
{
 printf("Starting up multi-threaded TCP server\n");
 //seed random number generator
 srand(time(NULL));

 SOCKET sock;
  
 // for our thread
 DWORD thread;
  
 WSADATA wsaData;
 sockaddr_in server;
 
  
 // start winsock
 int ret = WSAStartup(0x101,&wsaData); // use highest version of winsock avalible
  
 if(ret != 0)
 {
    return 0;
 }
   
 // fill in winsock struct ...
 server.sin_family=AF_INET;
 server.sin_addr.s_addr=INADDR_ANY;
 server.sin_port=htons(25); // listen on SMTP port 25
  
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
