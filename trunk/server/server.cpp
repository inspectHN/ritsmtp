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
	
	string mailFrom = msg[2].substr((msg[2].find_first_of("@")+1));//get domain name
	mailFrom = mailFrom.substr(0,(mailFrom.length()-1)); //truncate trailing ">"

	if(mailFrom == OUR_DOMAIN)
	cout << "yay\n";
	
	
	
	
	 
     
	
	return 0;
}
