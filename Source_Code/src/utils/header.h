/*=====================================================================================

    Filename:     header.h

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <stdlib.h>
#include <chrono>
using namespace std;
using Integer = __int128_t;
const Integer MaxValue = numeric_limits<Integer>::max();
const Integer MinValue = -MaxValue;
long long const ZoomTimes = 1000000000000000;
int const Power = 15;
const Integer DefaultUpperBound = 1;
const Integer DefaultLowerBound = 0;
const Integer InfiniteUpperBound = 1000000;
const Integer InfiniteLowerBound = -1000000;
enum class VarType
{
    Binary,
    Integer,
    Fixed
};
string itos(__int128_t v);
std::chrono::_V2::system_clock::time_point TimeNow();
double ElapsedTime(
    std::chrono::_V2::system_clock::time_point &a,
    std::chrono::_V2::system_clock::time_point &b);
