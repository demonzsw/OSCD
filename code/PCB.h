#pragma once
#include <ctime>

enum prostate { READY, RUNNING, WAITING, FINISHED};   //进程状态：就绪，执行，阻塞，完成

class PCB {
    public:
        PCB(unsigned int pid, int needTime, time_t arrivalTime, int needA, int needB) {
            this->pid = pid;
            this->needTime = needTime;
            this->state = WAITING;
            this->prio = 1;
            this->arrivalTime = arrivalTime;
            this->startTime = -1;
            this->runTime = 0;
            this->needA = needA;
            this->needB = needB;
            this->hasA = 0;
            this->hasB = 0;
        }
        ~PCB(){};
        unsigned int getPid(){
            return this->pid;
        }
        int getNeedTime(){
            return this->needTime;
        }
        prostate getState(){
            return this->state;
        }
        unsigned int getPrio(){
            return this->prio;
        }
        time_t getArrivalTime(){
            return this->arrivalTime;
        }
        time_t getStartTime(){
            return this->startTime;
        }
        int getRunTime(){
            return this->runTime;
        }
        int getNeedA(){
            return this->needA;
        }
        int getNeedB(){
            return this->needB;
        }
        int getHasA(){
            return this->hasA;
        }
        int getHasB(){
            return this->hasB;
        }
        void setNeedTime(int needTime){
            this->needTime = needTime;
        }
        void setState(prostate state){
            this->state = state;
        }
        void setPrio(unsigned int prio){
            this->prio = prio;
        }
        void setStartTime(time_t startTime){
            this->startTime = startTime;
        }
        void setRunTime(int runTime){
            this->runTime = runTime;
        }
        void setNeedA(int needA){
            this->needA = needA;
        }
        void setNeedB(int needB){
            this->needB = needB;
        }
        void setHasA(int hasA){
            this->hasA = hasA;
        }
        void setHasB(int hasB){
            this->hasB = hasB;
        }
    private:
        unsigned int pid;   //进程id
        int needTime;       //进程完成需要运行时间
        prostate state;     //进程状态
        unsigned int prio;  //进程优先级 
        time_t arrivalTime; //进程创建时间
        time_t startTime;   //进程开始时间
        int runTime;        //进程已占用CPU时间
        int needA;          //进程需要资源A的个数
        int needB;          //进程需要资源B的个数
        int hasA;           //进程拥有资源A的个数
        int hasB;           //进程拥有资源B的个数
};