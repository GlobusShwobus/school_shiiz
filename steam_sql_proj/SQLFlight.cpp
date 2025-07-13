#include "SQLFlight.h"

namespace ORDO {

	bool SQLFlight::Connect(std::string_view IP, std::string_view user, std::string_view password)
	{
		//if for any reason attempt a new connection mid flight, clean up old shit
		mConnect.release();
		mDriver = nullptr;
		mIsGood = false;
		SSQLStreamError.clear();

		//establish driver, can fail i guess but should not be likely
		try {
			mDriver = sql::mysql::get_driver_instance();
		}
		catch (const sql::SQLException& expt) {
			SSQLStreamError = "Driver init fail: " + std::string(expt.what());
			return false;
		}

		//establish connection for the flight, can fail for all reasons
		try {
			mConnect = std::unique_ptr<sql::Connection>(mDriver->connect(IP.data(), user.data(), password.data()));
			mIsGood = true;
		}
		catch (const sql::SQLException& expt) {
			SSQLStreamError = "Connection init fail: " + std::string(expt.what());
			mDriver = nullptr;
			return false;
		}

		return true;
	}
	sql::Statement& SQLFlight::acquireStatement() {
		if (!mConnect)
			throw std::runtime_error("connection not established (getStatementAccess)");
		if (!mStatement)
			mStatement = std::unique_ptr<sql::Statement>(mConnect->createStatement());
		return *mStatement;
	}
	bool SQLFlight::Insert(const SQLTable& table) {
		if (!mConnect)
			throw std::runtime_error("connection not established (Insert)");

		try {
			std::unique_ptr<sql::PreparedStatement> pstmt(mConnect->prepareStatement(table.createInsertStatement().data()));
			table.bindToStatement(pstmt.get());
			pstmt->executeUpdate();
		}
		catch (std::exception& expt) {//honestly everything can go wrong so idk type
			SSQLStreamError = "Insert error: " + std::string(expt.what());
			return false;
		}
		return true;
	}
}