#include "SQLFlight.h"

namespace ORDO {

	bool SQLFlight::Connect(std::string_view IP, std::string_view user, std::string_view password)
	{
		//if for any reason attempt a new connection mid flight, clean up old shit
		close();

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
	bool SQLFlight::setSchema(std::string_view name) {
		ConnectionCheck();
		try {
			mConnect->setSchema(name.data());
		}
		catch (sql::SQLException& expt) {
			SSQLStreamError = "Schema assignment Failure: " + std::string(expt.what());
			return false;
		}
		return true;
	}
	bool SQLFlight::doCommand(const SQLCommand command) {
		ConnectionCheck();

		try {
			std::unique_ptr<sql::Statement> stmt(mConnect->createStatement());
			stmt->execute(command.command.data());
		}
		catch (sql::SQLException& expt) {
			SSQLStreamError = "Command failure " + std::string(expt.what());
			return false;
		}

		return true;
	}
	bool SQLFlight::doPreparedInsert(SQLInsertOp op) {
		ConnectionCheck();

		try {
			std::unique_ptr<sql::PreparedStatement> pstmt(mConnect->prepareStatement(op.command.data()));
			op.binder(pstmt.get());
			pstmt->executeUpdate();
		}
		catch (sql::SQLException& expt) {
			SSQLStreamError = "Prepard Insert failure " + std::string(expt.what());
			return false;
		}

		return true;
	}
}