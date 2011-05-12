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

void sendData(SOCKET sock, char* str)
{
     int res;
     Sleep(5);
     cout << "sending" <<str <<endl;
     res = send(sock,str,sizeof(str),0);
     //cout << "Sent message\n\n";
     if(res==0)
     { 
         //0==other side terminated conn
         printf("\nSERVER terminated connection\n");
         Sleep(40);
         //closesocket(client);
         //client = 0;
         //break;
         }
     else if(res==SOCKET_ERROR)
     { 
         //-1 == send error
         printf("Socket error\n");
         Sleep(40);
         //break;
         }
 }
 
 void receiveData(SOCKET sock)
 {
      int ret;
      char RecvdData[100] = "";
      ret = recv(sock,RecvdData,sizeof(RecvdData),0);
     if(ret > 0)
     {
         cout<<endl<<RecvdData;
         //cout << "before strcpy" << endl;
         strcpy(RecvdData,"");
         //cout << "In recv if" << endl;
         }
         //cout << "out of recv if" << endl;
  }
  
int errorCheck(int res)
{
    int error = 0;
    if(res==0)
    { 
        //0==other side terminated conn
        printf("\nSERVER terminated connection\n");
        Sleep(40);
        //closesocket(client);
        error=1;
        //client = 0;
        //break;
    }
    else if(res==SOCKET_ERROR)
    { 
        //-1 == send error
        printf("Socket error\n");
        Sleep(40);
        error=1;
        //break;
    }
    return error;
}
