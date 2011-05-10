#include "client_functions.h"

// Print menu and get choice
void printMenu
{
    int choice;
    
    cout << "1) Send an Email" << endl << "2) Quit" << endl;
    
    cin >> choice;
    if(choice == 2)
    {
        exit(1);
    }
}
