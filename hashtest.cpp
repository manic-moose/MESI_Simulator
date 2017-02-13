
#include <map>
#include <iostream>

using namespace std;

int main (void) {
    
    map<int, char> mymap;
    
    mymap[4] = 'a';
    
    mymap[2] = 'c';
    
    mymap[7] = 'x';
        
    map<int, char>::iterator it;
    
    for (it=mymap.begin(); it != mymap.end(); it++) {
        cout << it->first << ':' << it->second << endl;   
    }

    
   return 0; 
}