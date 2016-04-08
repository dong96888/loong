/*************************************************************************
*  File Name: TestTimer.cpp
*     Author: dengxiudong
*       Mail:
* Created on: 2015年09月24日 星期四 16时00分21秒
*   Describe:
*
**************************************************************************/
#include "Timer.h"
#include "TimerManager.h"
#include <iostream>
#include <functional>

using namespace std;
using namespace loong;


void onTimer(void *arg)
{
    cout<<"-----------Hello miss timer!-----------"<<endl;
}
int main()
{
    int a=9;
    TimerManager::instance().start();
    //Timer *t=new Timer(6,onTimer,&a,Timer::TIMER_ONCE);
    //t->start();
    Timer *t1=new Timer(2,bind(onTimer,a),Timer::TIMER_CIRC);
    t1->start();
    Timer *t2=new Timer(3,bind(onTimer,a),Timer::TIMER_CIRC);
    t2->start();
    Timer *t3=new Timer(4,bind(onTimer,a),Timer::TIMER_CIRC);
    t3->start();
    getchar();
}
