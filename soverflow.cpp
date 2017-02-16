#include <map>
#include <iostream>
#include <string>
using namespace std;

struct A {
    typedef int (A::*MFP)(int);
    std::map <string, MFP> fmap;

    int f( int x ) { return x + 1; }
    int g( int x ) { return x + 2; }


    A() {
        fmap.insert( std::make_pair( "f", &A::f ));
        fmap.insert( std::make_pair( "g", &A::g ));
    }

    int Call( const string & s, int x ) {
        MFP fp = fmap[s];
        return (this->*fp)(x);
    }
};

int main() {
    A a;
    cout << a.Call( "f", 0 ) << endl;
    cout << a.Call( "g", 0 ) << endl;
}