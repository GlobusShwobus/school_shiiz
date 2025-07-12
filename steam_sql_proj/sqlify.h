#pragma once

#include <string>
#include <string_view>
#include <memory>

#include <cppconn/prepared_statement.h>//for prep statements

#include <mysql_driver.h>    //for driver
#include <mysql_connection.h>//for connection
#include <cppconn/driver.h>  //for connecting driver with connector

namespace ORDO {


	class SSQLUser {

		std::string serverIP;
		std::string serverName;
		std::string serverPassword;

	public:

		SSQLUser() = default;
		SSQLUser(std::string_view IP, std::string_view server_name, std::string_view password)
			:serverIP(IP), serverName(server_name), serverPassword(password) {}

		void setServerIP(std::string_view ip) {
			serverIP = ip;
		}
		void setServerName(std::string_view name) {
			serverName = name;
		}
		void setServerPassword(std::string_view password) {
			serverPassword = password;
		}
		std::string_view getIP()const {
			return serverIP;
		}
		std::string_view getSName()const {
			return serverName;
		}
		std::string_view getSPassword()const {
			return serverPassword;
		}

	};

	class SSQLTable {
	public:

		virtual std::string_view createTableStatement() const = 0;
		virtual std::string_view createInsertStatement()const = 0;
		virtual void bindToStatement(sql::PreparedStatement* stmt)const = 0;
		virtual std::string_view tableName()const = 0;
		virtual ~SSQLTable() = default;
	};

	class SSQLFlight {

		sql::mysql::MySQL_Driver* mDriver = nullptr;
		std::unique_ptr<sql::Connection> mConnect = nullptr;
		std::unique_ptr<sql::Statement> mStatement = nullptr;
		bool mIsGood = false;
		std::string SSQLStreamError;

		static constexpr std::string_view defaultLocalServer = "tcp://127.0.0.1:3306";

		bool Connect(std::string_view IP, std::string_view user, std::string_view password)
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

	public:
		SSQLFlight() = default;
		SSQLFlight(std::string_view IP, std::string_view user, std::string_view password)
		{
			Connect(IP, user, password);
		}
		SSQLFlight(std::string_view user, std::string_view password)
		{
			Connect(defaultLocalServer, user, password);
		}

		bool connect(std::string_view IP, std::string_view user, std::string_view password) {
			return Connect(IP, user, password);
		}
		bool connect(std::string_view user, std::string_view password) {
			return Connect(defaultLocalServer, user, password);
		}
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

		bool isGood()const {
			return mIsGood;
		}
		std::string_view getStreamError()const {
			return SSQLStreamError;
		}

	public:
		//don't want these for streams
		SSQLFlight(const SSQLFlight&) = delete;
		SSQLFlight(SSQLFlight&&) = delete;
		SSQLFlight& operator=(const SSQLFlight&) = delete;
		SSQLFlight& operator=(SSQLFlight&&) = delete;

/*

need schema assigner - each instance is a unique landing schema - data should be handled internally by sql itself not me, i just pass on a landing strip
	data should be dumped at once in script part

need manual close

need internal statement
need internal prepared statement

*/
	};
    //THIS SHOULD BE IN SOME SCRIPTING PART OF THE CODE NOT HERE, TOO MUCH RESPONSIBILITY
	//bool establishSchema()
	//{
	//	const std::string name = GenerateRandomName("LANDING_");
	//	const std::string cmdStatement = "CREATE DATABASE " + name + ";";
	//	try {
	//		std::unique_ptr<sql::Statement> statement(mConnect->createStatement());
	//		statement->execute(cmdStatement);
	//		mIsSchemaSet = true;
	//		return true;
	//	}
	//	catch (const sql::SQLException& expt) {
	//		SSQLStreamError = "Schema creation error: " + std::string(expt.what());
	//		mIsSchemaSet = false;
	//	}
	//	return false;
	//}
	
	//THIS TOO
	//std::string GenerateRandomName(std::string prefix = "") {
	//	static constexpr int ASCII_NUM_START = 48;
	//	static constexpr int ASCII_NUM_END = 57;
	//	static constexpr int ASCII_BIGCHAR_START = 65;
	//	static constexpr int ASCII_BIGCHAR_END = 90;
	//	static constexpr int ASCII_SMALLCHAR_START = 97;
	//	static constexpr int ASCII_SMALLCHAR_END = 122;
	//
	//	NumberGenerator rng;
	//
	//	for (int i = 0; i < 10; i++)
	//	{
	//		int dice = rng.getRandInt(0, 2);
	//		char c;
	//		switch (dice) {
	//		case 0:c = rng.getRandInt(ASCII_NUM_START      , ASCII_NUM_END);	   break;
	//		case 1:c = rng.getRandInt(ASCII_BIGCHAR_START  , ASCII_BIGCHAR_END);   break;
	//		case 2:c = rng.getRandInt(ASCII_SMALLCHAR_START, ASCII_SMALLCHAR_END); break;
	//		default:
	//			printf("random name generation RNG missfire\n");
	//			continue;
	//		}
	//		prefix.push_back(c);
	//	}
	//	return prefix;
	//}
}
