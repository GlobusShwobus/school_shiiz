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

		sql::mysql::MySQL_Driver*        mDriver = nullptr;
		std::unique_ptr<sql::Connection> mConnect = nullptr;
		std::unique_ptr<sql::Statement>  mStatement = nullptr;
		
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

		void doCommand(sqlCommand command) {

		}
		void doPreparedInsert(sqlInsertOp op) {

		}



		//DEPRICATED
		sql::Statement& acquireStatement() {
			if (!mConnect)
				throw std::runtime_error("connection not established (getStatementAccess)");
			if (!mStatement)
				mStatement = std::unique_ptr<sql::Statement>(mConnect->createStatement());
			return *mStatement;
		}
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
		bool Insert(const SQLTable& table) {
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
		//####################################

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
