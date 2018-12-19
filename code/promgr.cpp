#include <iostream>
#include "PCB.h"
#include "Semaphore.h"
#include <list>
#include <map>
#include <thread>
#include <windows.h>

using namespace std;

map<unsigned int,PCB*> m;

list<PCB*> l0;
list<PCB*> l1;
list<PCB*> l2;
list<PCB*> l3;     
list<PCB*> l4;
list<PCB*> l5;      //5级反馈队列调度

const int ANUM = 10;
const int BNUM = 10;
int readyNum = 0;
int runningNum = 0;
int waitingNum = 0;
int finishedNum = 0;
int numA = ANUM;
int numB = BNUM;
Semaphore A = Semaphore(numA);
Semaphore B = Semaphore(numB);

void showProInfo(PCB* p){
    cout<<"\nPID: "<<p->getPid()<<
    "\nPRIORITY: "<<p->getPrio()<<
    "\nNEEDTIME: "<<p->getNeedTime()<<
    "\nNEED A NUM: "<<p->getNeedA()<<
    "\nHAS A NUM: "<<p->getHasA()<<
    "\nNEED B NUM: "<<p->getNeedB()<<
    "\nHAS B NUM: "<<p->getHasB()<<
    "\nSTATE: ";
    switch(p->getState()){
        case 0: cout<<"READY"<<endl;break;
        case 1: cout<<"RUNNING"<<endl;break;
        case 2: cout<<"WAITING"<<endl;break;
        case 3: cout<<"FINISHED"<<endl;break;
    }
    time_t arrivalTime = p->getArrivalTime();
    tm* tm1 = localtime(&arrivalTime);
    cout<<"ARRIVALTIME: "<<tm1->tm_year+1900<<"-"<<tm1->tm_mon+1<<"-"<<tm1->tm_mday
    <<" "<<tm1->tm_hour<<":"<<tm1->tm_min<<":"<<tm1->tm_sec<<endl; 
    time_t startTime = p->getStartTime();
    tm* tm2 = localtime(&startTime);
    cout<<"STARTTIME: ";
    if(startTime==-1){
        cout<<"NULL"<<endl;
    } else {
        cout<<tm2->tm_year+1900<<"-"<<tm2->tm_mon+1<<"-"<<tm2->tm_mday
        <<" "<<tm2->tm_hour<<":"<<tm2->tm_min<<":"<<tm2->tm_sec<<endl;
    }
    cout<<"RUNTIME: "<<p->getRunTime()<<endl;
}

void show(){
    cout<<"........................................................\n"
    <<"     1.creat      "<<"2.kill      "<<"3.check     "<<"4.close     \n"
    <<"........................................................"<<endl;
}

void operateP(PCB* p){
    int needA = p->getNeedA();
    int needB = p->getNeedB();
    p->setState(WAITING);
    while(needA--){
        p->setHasA(p->getHasA()+1);
        A.wait(); 
        numA = A.getCount();
    }
    while(needB--){
        p->setHasB(p->getHasB()+1);
        B.wait();
        numB = B.getCount();
    }
    p->setState(READY);
    readyNum++;
    waitingNum--;
}

void operateV(PCB* p){
    int needA = p->getNeedA();
    int needB = p->getNeedB();
    while(needA--){
        A.signal();
        numA = A.getCount();  
    }
    while(needB--){
        B.signal();
        numB = B.getCount();
    }
    p->setHasA(0);
    p->setHasB(0);
}

void creat(){
    unsigned int pid;   
    int needTime, needA, needB;            
    cout<<"PID: ";
    cin>>pid;
    if(m.count(pid)){
        cout<<"EXSISTED"<<endl;
    } else {
        cout<<"NEEDTIME: ";
        cin>>needTime;
        time_t arrivalTime = time(NULL);
        cout<<"NEED A NUM: ";
        cin>>needA;
        cout<<"NEED B NUM: ";
        cin>>needB;
        PCB* p = new PCB(pid, needTime, arrivalTime, needA, needB);
        m[pid] = p;
        l0.push_back(p);
        waitingNum++;
    }
}

void del(list<PCB*> &l, PCB* p){
    for(list<PCB*>::iterator iter=l.begin();iter!=l.end();iter++){
        if(*iter==p) {
            l.erase(iter);
            break;
        } 
    }
}

void kill(){
    unsigned int pid;
    cout<<"PID: ";
    cin>>pid;
    if(m.count(pid)){
        PCB* p = m[pid];
        m.erase(m.find(pid));
        switch(p->getPrio()){
            case 1: del(l1,p);break;
            case 2: del(l2,p);break;
            case 3: del(l3,p);break;
            case 4: del(l4,p);break;
            case 5: del(l5,p);break;
        }
        switch(p->getState()){
            case 0: readyNum--;break;
            case 1: runningNum--;break;
            case 2: waitingNum--;break;
            case 3: finishedNum--;break;
        }
        numA += p->getHasA();
        numB += p->getHasB();
        delete p;
    } else {
        cout<<"NOT FOUND"<<endl;
    }
}

void check(){
    unsigned int pid;
    cout<<"PROCESS NUM: "<<m.size()<<endl;
    cout<<"READY PROCESS NUM: "<<readyNum<<endl;
    cout<<"RUNNING PROCESS NUM: "<<runningNum<<endl;
    cout<<"WAITING PROCESS NUM: "<<waitingNum<<endl;
    cout<<"FINISHED PROCESS NUM: "<<finishedNum<<endl;
    cout<<"A NUM: "<<ANUM<<endl;
    cout<<"B NUM: "<<BNUM<<endl;
    cout<<"FREE A NUM: "<<numA<<endl;
    cout<<"FREE B NUM: "<<numB<<endl;
    cout<<"\nPID: ";
    cin>>pid;
    if(m.count(pid)){
        PCB* p = m[pid];
        showProInfo(p);
    } else {
        cout<<"NOT FOUND"<<endl;
    }
}

void controller(int kind){
    switch(kind){
        case 1: creat();break;
        case 2: kill();break;
        case 3: check();break;
        case 4: exit(0);break;
    }
}

void process(PCB* p, int cnt, list<PCB*> &l){
    runningNum++;
    readyNum--;
    int runTime = p->getRunTime();
    int time;
    while(cnt--){
        Sleep(1000);
        runTime++;
        p->setRunTime(runTime);
        time = p->getNeedTime();
        p->setNeedTime(--time);
        if(time<=0){
            p->setState(FINISHED);
            finishedNum++;
            break;
        }
    }
    operateV(p);
    runningNum--;
    if(time>0) {
        p->setState(WAITING);
        if(p->getPrio()<5){
            p->setPrio(p->getPrio()+1);
        }
        l0.push_back(p);
        waitingNum++;
    } 
}

void deal(PCB* p, list<PCB*> &l){
    if(p->getState()==0){
        time_t startTime = time(NULL);
        p->setState(RUNNING);
        p->setStartTime(startTime);
        process(p,1,l);
    }
}

void deal2(PCB* p, int cnt, list<PCB*> &l){
    if(p->getState()==0){
        p->setState(RUNNING);
        process(p,cnt,l);
    }
}

void schedule(){
    while(1){
        while(!l1.empty()){
            PCB* p = l1.front();
            deal(p,l2);
            l1.pop_front();
        }
        while(!l2.empty()){
            if(!l1.empty()) break;
            PCB* p = l2.front();
            deal2(p,2,l3);
            l2.pop_front();
        }
        while(!l3.empty()){
            if(!l1.empty()||!l2.empty()) break;
            PCB* p = l3.front();
            deal2(p,4,l4);
            l3.pop_front();
        }
        while(!l4.empty()){
            if(!l1.empty()||!l2.empty()||!l3.empty()) break;
            PCB* p = l4.front();
            deal2(p,8,l5);
            l4.pop_front();
        }
        while(!l5.empty()){
            if(!l1.empty()||!l2.empty()||!l3.empty()||!l4.empty()) break;
            PCB* p = l5.front();
            deal2(p,16,l5);
            l5.pop_front();
        }
    }
}

void enList(){
    while(1){
        while(!l0.empty()){
            PCB* p = l0.front();
            if(p->getNeedA()<=ANUM && p->getNeedB()<=BNUM){
                operateP(p);
                switch(p->getPrio()){
                    case 1: l1.push_back(p);break;
                    case 2: l2.push_back(p);break;
                    case 3: l3.push_back(p);break;
                    case 4: l4.push_back(p);break;
                    case 5: l5.push_back(p);break;
                }
            }
            l0.pop_front();
        }
    }
}

void window(){
    int kind;
    show();
    while(cin>>kind){
        controller(kind);
        show();
    }
}

int main() {
    thread mgr(schedule);
    thread win(window);
    thread enl(enList);
    mgr.join();
    win.join();
    enl.join();
    return 0;
}