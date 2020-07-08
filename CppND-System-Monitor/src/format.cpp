#include <string>

#include "format.h"

using std::string;

std::string Format::Zeroedint(int t) {
    std::string time = std::to_string(t);
    if(time.length() < 2) {
        time = "0" + time;
    }
    return time;
}

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
	long int h = seconds/3600;
    long int m = (seconds - h*3600)/60;
    long int s = seconds - h*3600 - m*60;
    std::string elapsedTime = Zeroedint(h) + ":" + Zeroedint(m) + ":" + Zeroedint(s);
    return elapsedTime; 
}