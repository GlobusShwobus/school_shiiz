#pragma once

#include <random>
#include <memory>

namespace badSQL
{
	class RandomNum final
	{
	public:

		RandomNum();

		int rInt(int min, int max)const noexcept;

		float rFloat(float min, float max)const noexcept;

	private:
		std::unique_ptr<std::mt19937> rng;
	};
}