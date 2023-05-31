#pragma once
#include <chrono>


/* this file is for debug purpose .*/

using namespace std;
using namespace chrono;
class myTime
{
private:
	time_point<high_resolution_clock> start;
	time_point<high_resolution_clock> end;
	double duration;
public:
	myTime():start(),end(),duration(-1) {};
	~myTime() { };
	void tic(){
	    start = high_resolution_clock::now();//获取当前时间
    };
	void toc(){
        end = high_resolution_clock::now();//获取当前时间
        auto dur = duration_cast<nanoseconds>(end - start);
        duration = double(dur.count()) * nanoseconds::period::num / nanoseconds::period::den * 1000000000;
    };
    double getDuration(){
        return duration;
    };
};