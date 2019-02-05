#include <iostream>
#include <unistd.h> // fork()
#include <cstdlib>  // exit()

using namespace std;

int main()
{
    int x, y;

    x = fork();

    if (x < 0) 
    {
        cout << "Fork failed\n";
        exit(1);
    }

    if (x == 0) 
    { 
        // child
        cout << "Child: fork() returned = " << x << endl;
        cout << "Child: pid = " << getpid() << endl;
        cin >> y;
        cout << "Child: entered = " << y << endl;
    } 
    else // x > 0 
    { 
        // parent
        cout << "Parent: fork() returned = " << x << endl;
        cout << "Parent: pid = " << getpid() << endl;
        cin >> y;
        cout << "Parent: entered = " << y << endl;
    }
} // main
