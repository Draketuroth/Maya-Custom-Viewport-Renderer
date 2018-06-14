#include "Timer.h"

Timer::Timer()
{
	this->countsPerSecond = 0;
	this->secondsPerCount = 0;

	this->previousTime = 0;
	this->currentTime = 0;

	this->deltaTime;
	this->secondCounter = 0;
}

Timer::~Timer()
{
}

void Timer::initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&this->countsPerSecond);
	this->secondsPerCount = 1.0f / this->countsPerSecond;

	QueryPerformanceCounter((LARGE_INTEGER*)&this->previousTime);
}

float Timer::getDeltaTime()
{
	this->currentTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&this->currentTime);

	this->deltaTime = ((this->currentTime - previousTime) * this->secondsPerCount);
	return this->deltaTime;
}

float Timer::getCurrentTime()
{
	return this->currentTime;
}

void Timer::updateCurrentTime()
{
	this->previousTime = this->currentTime;
}

void Timer::resetTimer()
{

}

float Timer::getCountsPerSecond()
{
	return this->countsPerSecond;
}

