#include <iostream>
#include <ucontext.h>
#include <unistd.h>

using namespace std;
int i1=0,i2=0;
ucontext_t ctx1,ctx2;

void func1(){
    getcontext(&ctx1);
    cout<<"Func1 Message "<<++i1<<": Hello World!\n";
    usleep(100000);
    setcontext(&ctx1);
}

void func2_sub1(){
    cout<<"Func2 Message "<<++i2<<": Hello World!\n";
}

void func2(){
    getcontext(&ctx2);
    ctx2.uc_stack.ss_sp=new char[2048];
    ctx2.uc_stack.ss_size=2048;
    ctx2.uc_link=nullptr;
    makecontext(&ctx2,func2_sub1,0);
    setcontext(&ctx2);
}

void func3_sub1(){
    cout<<"Func3.1 Message "<<++i1<<": Hello World!\n";
    usleep(100000);
    setcontext(&ctx2);
}

void func3_sub2(){
    cout<<"Func3.2 Message "<<++i2<<": Hello World!\n";
    usleep(100000);
    setcontext(&ctx1);
}

void func3(){
    getcontext(&ctx1);
    getcontext(&ctx2);

    ctx1.uc_stack.ss_sp=new char[2048];
    ctx1.uc_stack.ss_size=2048;
    ctx1.uc_link=&ctx2;
    makecontext(&ctx1,func3_sub1,0);

    ctx2.uc_stack.ss_sp=new char[2048];
    ctx2.uc_stack.ss_size=2048;
    ctx2.uc_link=&ctx1;
    makecontext(&ctx2,func3_sub2,0);

    setcontext(&ctx1);
}

int main(){
    cout<<"main start...\n\n";
    func3();
    cout<<"main return.\n";
    return 0;
}