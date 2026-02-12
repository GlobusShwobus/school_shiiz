#pragma once

#include <string>
#include "json.hpp"
#include "Sequence.h"
#include "WebRequestHandle.h"
#include "Payload.h"

namespace badSQL
{
	class PayloadArray final
	{
	public:
		PayloadArray() = default;

		std::string try_make_payload(const nlohmann::json& data, std::string_view label, std::size_t id)noexcept;

		std::string try_make_payload(nlohmann::json&& data, std::string_view label, std::size_t id)noexcept;

		//ideally handle bs handles externally because if this fails it can return the full exception
		std::string try_make_payload(const WebRequestHandle& wrh, std::string_view label, std::size_t id)noexcept;

		const Sequence<Payload>& get()const noexcept;

		const auto cbegin()const noexcept
		{
			return injectables.cbegin();
		}

		const auto cend()const noexcept
		{
			return injectables.cend();
		}

		//too expensive to reasonably allow, though it wouldn't be a bug if it was
		PayloadArray(const PayloadArray&) = delete;
		PayloadArray& operator=(const PayloadArray&) = delete;

		PayloadArray(PayloadArray&&) noexcept = default;
		PayloadArray& operator=(PayloadArray&&) noexcept = default;

	private:
		Sequence<Payload> injectables;
	};
}