#pragma once
#include <mutex>
#include <condition_variable>
using namespace std;

class Semaphore{
    public:
        Semaphore(int count) {
            this->m_count = count;
        }
        Semaphore(const Semaphore&);
        Semaphore& operator=(const Semaphore&);
        void signal(){
            unique_lock<mutex> lock(m_mutex);
            ++m_count;
            m_cv_uptr.notify_one();
        }
        void wait(){
            unique_lock<mutex> lock(m_mutex);
            while (m_count == 0) { 
                m_cv_uptr.wait(lock);
            }
            --m_count;
        }
    private:
        mutex m_mutex;
        condition_variable m_cv_uptr;
        int m_count;
};