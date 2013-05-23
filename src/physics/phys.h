#ifndef PHYS_H
#define PHYS_H

#include <math.h>
#include <random>
#include <chrono>

class Phys
{
	public:
		Phys(){
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			rng.seed(seed);
		};

		static unsigned long long speedOfSound(double x_origin, double y_origin,
				double x_dest, double y_dest);

		static unsigned long long speedOfSound(double x_origin, double y_origin,
				double x_dest, double y_dest, double propagationSpeed);

		static double calcDistance(double x_origin, double y_origin, 
				double x_dest, double y_dest);
		static unsigned long long getCTime();

		static void incTime();
		static void setTimeRes(double timeResolution);
		static double getTimeRes();
		static int getMacroFactor();
		static void setMacroFactor(int macroFactor);
		static void setCTime(unsigned long long ctime);
		static double getMersenneFloat(double min, double max);
		static uint64_t getMersenneInteger(uint64_t min, uint64_t max);


	private:
		static int macroFactor;
		static double timeResolution;
		static unsigned long long c_timeStep;
		//random distribution 0-INT_MAX
		static std::uniform_int_distribution<uint64_t> uint_dist;

		typedef std::mt19937 MyRNG;  // the Mersenne Twister with a popular choice of parameters
		static MyRNG rng;

};



#endif // PHYS_H
