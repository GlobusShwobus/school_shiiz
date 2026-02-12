#include "Payload_array.h"

namespace badSQL
{
	std::string PayloadArray::try_make_payload(const nlohmann::json& data, std::string_view label, std::size_t id)noexcept
	{
		if (data.empty())
			return "Failure: empty data";

		if (label.empty())
			return "Failure: Missing label";

		if (label.size() > 256)
			return "Failure: Large label";

		Payload dataset;
		dataset.data = data;
		dataset.label = std::string(label);
		dataset.recipient_id = id;

		injectables.push_back(std::move(dataset));

		return "Success: created injectable \"" + *dataset.label + "\"";
	}

	std::string PayloadArray::try_make_payload(nlohmann::json&& data, std::string_view label, std::size_t id)noexcept
	{
		Payload dataset;
		dataset.data = std::move(data);

		if (dataset.data->empty())
			return "Failure: empty data";

		if (label.empty())
			return "Failure: Missing label";

		if (label.size() > 256)
			return "Failure: Large label";

		dataset.label = std::string(label);
		dataset.recipient_id = id;

		injectables.push_back(std::move(dataset));

		return "Success: created injectable \"" + *dataset.label + "\"";
	}

	//ideally handle bs handles externally because if this fails it can return the full exception
	std::string PayloadArray::try_make_payload(const WebRequestHandle& wrh, std::string_view label, std::size_t id)noexcept
	{
		if (!wrh.is_good)
			return "Failure: Broken handle";

		if (wrh.data.isEmpty())
			return "Failure: Empty data";

		if (label.empty())
			return "Failure: Missing label";

		if (label.size() > 256)
			return "Failure: Large label";

		if (wrh.contentType == "application/json")
			return "Failure: Not a json";

		Payload dataset;

		try {
			dataset.data = nlohmann::json::parse(wrh.data.data());
		}
		catch (const nlohmann::json::exception& e) {
			return e.what();
		}

		dataset.label = std::string(label);
		dataset.recipient_id = id;
		injectables.push_back(std::move(dataset));

		return "Success: created injectable \"" + *dataset.label + "\"";
	}

	const Sequence<Payload>& PayloadArray::get()const noexcept
	{
		return injectables;
	}
}