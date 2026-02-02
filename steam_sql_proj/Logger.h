#pragma once

#include <string>
#include "Sequence.h"
#include <span>
#include <string_view>
#include <mutex>

namespace badSQL
{
	class Logger final
	{
	public:
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(Logger&&) = delete;

		static Logger& instance()
		{
			static Logger instance;
			return instance;
		}

		// add some info
		void add_log(std::string message) 
		{
			std::lock_guard<std::mutex> lock(mMutex);
			mLogs.push_back(std::move(message));
		}

		// add an error
		void add_error(std::string error)
		{
			std::lock_guard<std::mutex> lock(mMutex);
			mErrors.push_back(std::move(error));
		}

		std::span<const std::string> get_logs()const
		{
			return mLogs;
		}

		std::span<const std::string> get_errors()const
		{
			return mErrors;
		}

		// return last entry. may return nullptr
		std::string_view last_log()const
		{
			return mLogs.isEmpty() ? nullptr : mLogs.back();
		}

		// return last entry. may return nullptr
		std::string_view last_error()const
		{
			return mErrors.isEmpty() ? nullptr : mErrors.back();
		}

	private:
		Logger() = default;
		~Logger() = default;

		mutable std::mutex mMutex;
		Sequence<std::string> mLogs;
		Sequence<std::string> mErrors;
	};
}