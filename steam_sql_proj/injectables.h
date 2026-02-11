#include <string>
#include "json.hpp"
#include "Sequence.h"
#include "WebRequestHandle.h"
#include "Logger.h"

namespace badSQL
{
	struct passable_dataset {
		std::string label;
		nlohmann::json data;
	};

	class Injectables
	{
	public:
		Injectables() = default;

		std::string store_injectable(const nlohmann::json& data, std::string_view label)
		{
			//TODO
		}

		std::string store_injectable(const WebRequestHandle& wrh, std::string_view label)
		{
			if (!wrh.is_good || wrh.data.isEmpty())
				return "Failure: Broken handle";

			if (label.empty())
				return "Failure: Missing label";

			if (label.size() > 256)
				return "Failure: Evil label";

			if (wrh.contentType == "application/json")
				return "Failure: Not a json";
			
			passable_dataset dataset;

			try {
				dataset.data = nlohmann::json::parse(wrh.data.data());
			}
			catch (const nlohmann::json::exception& e) {
				return e.what();
			}

			dataset.label = std::string(label);
			injectables.push_back(std::move(dataset));

			return "Success: created injectable \"" + dataset.label + "\"";
		}


		const Sequence<passable_dataset>& get()const noexcept{
			return injectables;
		}

		const auto cbegin()const noexcept
		{
			return injectables.cbegin();
		}

		const auto cend()const noexcept
		{
			return injectables.cend();
		}

		//too expensive to reasonably allow, though it wouldn't be a bug if it was
		Injectables(const Injectables&) = delete;
		Injectables& operator=(const Injectables&) = delete;

		Injectables(Injectables&&) noexcept = default;
		Injectables& operator=(Injectables&&) noexcept = default;

	private:

		Sequence<passable_dataset> injectables;
	
	};

}