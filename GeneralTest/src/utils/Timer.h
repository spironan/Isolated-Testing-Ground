#pragma once

#include <cmath>
#include <string>
#include <chrono>
#include <iostream>

class Profiler 
{
public:
    Profiler(std::string name)
        : m_name(std::move(name)),
        m_beg(std::chrono::high_resolution_clock::now()) 
    {
    }

    ~Profiler() 
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto durNano = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_beg);
        auto durMilli = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_beg);
        auto durMicro = std::chrono::duration_cast<std::chrono::microseconds>(end - m_beg);
        std::cout << m_name << " \t : " << durMilli << "[" << durMicro << "]<" << durNano << ">\n";
    }

private:
    std::string m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_beg;
};
