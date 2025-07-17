#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <mysql_driver.h>              //for driver
#include <cppconn/prepared_statement.h>//for prep statements

#include "SQLSchema.h"

/*
	CREATING A NEW SCHEMA OR ASSIGNIG TO EXISTING SCHEMA MUST PASS SOME SIMILAR QUERY TO THIS TO MAKE SURE SCHEMA NAME IS FREE/OR SCHEMA ALREADY EXISTS
	std::unique_ptr<std::vector<std::string>> DATABASE::GetSchemaList()const {
		auto list = std::make_unique<std::vector<std::string>>();

		std::unique_ptr<sql::Connection> conn(driver->connect(ip, server_name, server_password));
		std::unique_ptr<sql::Statement>  stmt(conn->createStatement());
		std::unique_ptr<sql::ResultSet>  results(stmt->executeQuery("SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME NOT IN ('sys', 'performance_schema', 'information_schema', 'mysql');"));

		while (results->next()) {
			list->push_back(results->getString(1));
		}

		return list;
	}
*/

namespace ORDO {

	class SQLFlight {

	public:

		using SQLCommand = SQLSchema::SQLCommand;
		using SQLInsertOp = SQLSchema::SQLInsertOp;

	private:

		sql::mysql::MySQL_Driver*        mDriver = nullptr;
		std::unique_ptr<sql::Connection> mConnect = nullptr;
		
		bool mIsGood = false;
		std::string SSQLStreamError;

		static constexpr std::string_view defaultLocalServer = "tcp://127.0.0.1:3306";

		bool Connect(std::string_view IP, std::string_view user, std::string_view password);
		void ConnectionCheck() {
			if (!mConnect)
				throw std::runtime_error("Uninitalized connection");
		}

	public:
		SQLFlight() = default;
		SQLFlight(std::string_view IP, std::string_view user, std::string_view password)
		{
			Connect(IP, user, password);
		}
		SQLFlight(std::string_view user, std::string_view password)
		{
			Connect(defaultLocalServer, user, password);
		}

		bool connect(std::string_view IP, std::string_view user, std::string_view password) {
			return Connect(IP, user, password);
		}
		bool connect(std::string_view user, std::string_view password) {
			return Connect(defaultLocalServer, user, password);
		}

		bool setSchema(std::string_view name);
		bool doCommand(const SQLCommand command);
		bool doPreparedInsert(SQLInsertOp op);

		bool isGood()const
		{
			return mIsGood;
		}
		void close() {
			mConnect.release();
			mDriver = nullptr;
			mIsGood = false;
			SSQLStreamError.clear();
		}
		std::string_view getStreamError()const
		{
			return SSQLStreamError;
		}
	private:
		//don't want these for streams
		SQLFlight(const SQLFlight&) = delete;
		SQLFlight(SQLFlight&&) = delete;
		SQLFlight& operator=(const SQLFlight&) = delete;
		SQLFlight& operator=(SQLFlight&&) = delete;
	};
}
