#pragma once

#include <string>
#include "bad_exceptions.h"
#include <mysql_driver.h>              //for driver
#include <cppconn/prepared_statement.h>//for prep statements


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

namespace badSQL {


	std::string translate_sql_error_to_peepo_language(const sql::SQLException& e, const std::string& service)
	{
		switch (e.getErrorCode()) {
		case 2005:
			return "Invalid host [" + service + "]";
		case 2002: 
		case 2003: 
			return "Server unreachable [" + service + "]";
		case 1045: 
			return "Invalid username or password";
		case 1049: 
			return "Database does not exist";
		case 1044:
			return "Perimission denied";
		}


		const std::string& state = e.getSQLState();
		if (state.rfind("08", 0) == 0)
			return "Network error while connecting to [" + service + "]";

		if (state == "HYT00" || state == "HYT01")
			return "Connection timed out [" + service + "]";

		return "Failed to connect to [" + service + "]";//default
	}

	class DBConnect 
	{

	public:

		DBConnect(const std::string& user, const std::string& password, const std::string& service = "tcp://127.0.0.1:3306")
			:service(service), user(user), password(password)
		{
			connect(user, password, service);
		}


		void connect(const std::string& user, const std::string& password, const std::string& service)
		{
			close();

			try {
				mDriver = sql::mysql::get_driver_instance();
				mConnect.reset(mDriver->connect(service, user, password));
			}
			catch (const sql::SQLException& e) {
				mConnect.release();
				mDriver = nullptr;

				throw BadException(translate_sql_error_to_peepo_language(e, service).c_str());
			}
		}


		bool setSchema(std::string_view name);
		bool doCommand(const SQLCommand command);
		bool doPreparedInsert(SQLInsertOp op);

		void close()
		{
			mConnect.release();
			mDriver = nullptr;
			service.clear();
			user.clear();
			password.clear();
		}

	private:
		sql::mysql::MySQL_Driver* mDriver = nullptr;
		std::unique_ptr<sql::Connection> mConnect = nullptr;


		std::string service;
		std::string user;
		std::string password;
	};
}
