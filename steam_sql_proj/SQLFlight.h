#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <mysql_driver.h>              //for driver
#include <cppconn/prepared_statement.h>//for prep statements

#include "SQLTable.h"

//#include <mysql_connection.h>//for connection
//#include <cppconn/driver.h>  //for connecting driver with connector

namespace ORDO {


	class SQLFlight {

		sql::mysql::MySQL_Driver* mDriver = nullptr;
		std::unique_ptr<sql::Connection> mConnect = nullptr;
		std::unique_ptr<sql::Statement> mStatement = nullptr;
		bool mIsGood = false;
		std::string SSQLStreamError;

		static constexpr std::string_view defaultLocalServer = "tcp://127.0.0.1:3306";

		bool Connect(std::string_view IP, std::string_view user, std::string_view password);

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
		sql::Statement& acquireStatement();
		void CloseStatement() {
			mStatement.release();
		}
		bool setSchema(std::string_view schema) {
			try {
				mConnect->setSchema(schema.data());
			}
			catch (sql::SQLException& expt) {
				SSQLStreamError = "setSchema failure: " + std::string(expt.what());
				return false;
			}
			return true;
		}
		bool Insert(const SQLTable& table);

		bool isGood()const {
			return mIsGood;
		}
		std::string_view getStreamError()const {
			return SSQLStreamError;
		}

	public:
		//don't want these for streams
		SQLFlight(const SQLFlight&) = delete;
		SQLFlight(SQLFlight&&) = delete;
		SQLFlight& operator=(const SQLFlight&) = delete;
		SQLFlight& operator=(SQLFlight&&) = delete;

	};
}
