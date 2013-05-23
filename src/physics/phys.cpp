
#include <random>
#include <chrono>
#include <math.h>

#include <limits.h>

#include "phys.h"



typedef std::mt19937 MyRNG;  // the Mersenne Twister with a popular choice of parameters
double Phys::timeResolution = 0;
int Phys::macroFactor = 0;
unsigned long long Phys::c_timeStep = 0;
std::uniform_int_distribution<uint64_t> Phys::uint_dist;
MyRNG Phys::rng;



void Phys::incTime(){
	Phys::c_timeStep++;
}

void Phys::setTimeRes(double timeResolution){
	Phys::timeResolution = timeResolution;
}

double Phys::getTimeRes(){
	return Phys::timeResolution;
}

void Phys::setMacroFactor(int macroFactor){
	Phys::macroFactor = macroFactor;
}

int Phys::getMacroFactor(){
	return Phys::macroFactor;
}

unsigned long long Phys::speedOfSound(double x_origin, double y_origin,
		double x_dest, double y_dest){

	double distance = sqrt( pow((x_origin-x_dest), 2) + pow((y_origin-y_dest),2) );

	double tmp = distance / (343.2 * Phys::timeResolution);
	unsigned long long a_timestep = tmp + Phys::c_timeStep;

	return a_timestep;
}

unsigned long long Phys::speedOfSound(double x_origin, double y_origin,
		double x_dest, double y_dest, double propagationSpeed){

	double distance = sqrt( pow((x_origin-x_dest), 2) + pow((y_origin-y_dest),2) );

	double tmp = distance / (propagationSpeed * Phys::timeResolution);
	unsigned long long a_timestep = tmp + Phys::c_timeStep;

	return a_timestep;
}

double Phys::calcDistance(double x_origin, double y_origin, 
				double x_dest, double y_dest){
	return  sqrt( pow((x_origin-x_dest), 2) + pow((y_origin-y_dest),2) );
}

unsigned long long Phys::getCTime(){
	return Phys::c_timeStep;
}

void Phys::setCTime(unsigned long long ctime){
	Phys::c_timeStep = ctime;
}

double Phys::getMersenneFloat(double min=0, double max=1){
	return min + (double)Phys::uint_dist(Phys::rng)/((double)ULLONG_MAX/(max-min));
}

uint64_t Phys::getMersenneInteger(uint64_t min=0, uint64_t max=ULLONG_MAX){
	return min + Phys::uint_dist(Phys::rng)%max;
}
