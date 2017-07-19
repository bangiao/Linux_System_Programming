#include <iostream>
using namespace std;
int main()
{
    for(int i=1;i<7;i++)
    {
        for(int k=0;k<i;k++)
        {
            if(i==2||i==4||i==6)
            continue;
            cout << "*";
        }
        cout << "\n";
    }
}
