/**
 * @file Histogram.cpp
 * @author MP
 */
#include<iostream>
#include<string>
#include "Byte.h"
#include "Histogram.h"

using namespace std;

const double Histogram::HISTOGRAM_TOLERANCE=0.01; ///< Value of the default tolerance

Histogram::Histogram() {
    clear();
}
int Histogram::size() const{
    return HISTOGRAM_LEVELS;
}

void Histogram::clear() {
    for (int i = 0; i < size(); i++)
        setLevel(i, 0);
}

int Histogram::getLevel(Byte level) const {
    return _data[level.getValue()];
}

void Histogram::setLevel(Byte level, int npixeles) {
    _data[level.getValue()] = npixeles;
}

int Histogram::getMaxLevel() const {
    int max = -1;
    for (int i = 0; i < size(); i++)
        if (getLevel(i) > max)
            max = getLevel(i);
    return max;
}

int Histogram::getAverageLevel() const {
    int average = 0;
    for (int i = 0; i < size(); i++)
        average += getLevel(i);
    return average/size();
}

int Histogram::getBalancedLevel() const {
    int sum = 0, t, tope;
    for (int i = 0; i < size(); i++)
        sum += getLevel(i);
    tope=sum/2;
    sum=0;
    for (t=0; t<size() && sum < tope; t++) 
        sum+=getLevel(t);
    return t-1;
}


string Histogram::inspect() const{
    std::string result = "";
    for (int i = 0; i < HISTOGRAM_LEVELS; i++) {
        result += to_string(_data[i]);
    }
    result = std::to_string(std::hash<std::string>{}
    (result));
    result = std::to_string(HISTOGRAM_LEVELS) + " " + result;
    return result;
}
