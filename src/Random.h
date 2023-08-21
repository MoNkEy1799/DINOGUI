#pragma once

#include <random>

class Random
{
public:
	Random()
	{
		m_randomEngine.seed(std::random_device()());
	}

	float randFloat(float min = 0.0, float max = 1.0)
	{
		std::uniform_real_distribution<float> floatDist (min, max);
		return floatDist(m_randomEngine);
	}

	int randInt(int min, int max)
	{
		std::uniform_int_distribution<int> intDist(min, max);
		return intDist(m_randomEngine);
	}

	bool randBool()
	{
		return (randFloat() <= 0.5) ? true : false;
	}

	std::mt19937 getEngine()
	{
		return m_randomEngine;
	}

private:
	std::mt19937 m_randomEngine;
};