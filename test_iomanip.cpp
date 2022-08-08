#include <iostream>
#include <iomanip>

using namespace std;

int main(){
    cout<<"Before set: "<<1.23456<<' '<<9.97654<<' '<<1<<endl;
    cout<<"Set: "<<fixed<<setprecision(2)<<1.23456<<' '<<9.97654<<' '<<1<<endl;
    cout<<"After: "<<1.23456<<' '<<9.97654<<' '<<1<<endl;
    cout<<"Default: "<<defaultfloat<<setprecision(6)<<1.23456<<' '<<9.97654<<' '<<1<<endl;
    return 0;
}