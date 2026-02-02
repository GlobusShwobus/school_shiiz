
#include "NumberGenerator.h"
#include "bad_exceptions.h"

namespace badSQL
{
	RandomNum::RandomNum()
	{
		try {
			rng = std::make_unique<std::mt19937>(std::random_device{}());
		}
		catch (const std::runtime_error& e) {
			throw BadException("Number Generator random_device error", e.what());
		}
	}

	int RandomNum::rInt(int min, int max)const noexcept
	{
		assert(min <= max && "Invalid logic: min is more than max");
		return std::uniform_int_distribution<int>(min, max)(*rng);
	}

	float RandomNum::rFloat(float min, float max)const noexcept
	{
		assert(min <= max && "Invalid logic: min is more than max");
		return std::uniform_real_distribution<float>(min, max)(*rng);
	}
}