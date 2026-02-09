#pragma once

#include <string>
#include <mysql_driver.h>              //for driver
#include <cppconn/prepared_statement.h>//for prep statements
#include <span>
#include "Logger.h"
#include "input.h"


/*
check validity before doing commands is either connection.isValid or ping()
*/

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


	std::string translate_sql_throw_message_to_peepo_table(const sql::SQLException& e)
	{
		const std::string& state = e.getSQLState();
		const int code = e.getErrorCode();

		if (state == "HYT00" || state == "HYT01")
			return "Connection timed out";
		if (state.rfind("08", 0) == 0)
			return "Network / connection error";

		if (state.rfind("28", 0) == 0)
			return "Authorization failure";

		if (state.rfind("42", 0) == 0)
			return "Invalid SQL statement";

		if (state.rfind("22", 0) == 0)
			return "Invalid data value";

		if (state.rfind("23", 0) == 0)
			return "Constraint violation";

		if (state.rfind("40", 0) == 0)
			return "Transaction conflict (deadlock)";


		switch (code) {
		case 1062:
			return "Duplicate entry";

		case 1050:
			return "Table already exists";

		case 1007:
			return "Schema already exists";

		case 1049:
			return "Database does not exist";

		case 1045:
			return "Invalid username or password";

		case 1142:
		case 1044:
			return "Permission denied";

		case 2002:
		case 2003:
			return "Server unreachable";

		case 2005:
			return "Invalid host";

		case 2006:
		case 2013:
		case 2055:
			return "Connection lost while executing command";

		case 1064:
			return "SQL syntax error";
		}

		return "Undefined error: " + std::string(e.what());
	}

	class DBConnect final
	{

	public:
		DBConnect() = default;
		DBConnect(const std::string& user, const std::string& service = "tcp://127.0.0.1:3306")
		{
			connect(user, service);
		}


		std::string connect(const std::string& user, const std::string& service)
		{
			auto& logger = Logger::instance();
			close();
			std::string responce = "Success";

			try {
				Sequence<char> password = input_password();
				mDriver = sql::mysql::get_driver_instance();
				mConnect.reset(mDriver->connect(service, user, password.data()));

				//two layer wipe, intentional
				std::fill(password.begin(), password.end(), '\0');
				password.wipe();
			}
			catch (const sql::SQLException& e) {
				mConnect.release();
				mDriver = nullptr;

				responce = translate_sql_throw_message_to_peepo_table(e);
				logger.add_error(responce);
			}
			return responce;
		}

		std::string do_simple_command(const std::string& command)
		{
			auto& logger = Logger::instance();
			std::string responce = "Success";

			try {
				std::unique_ptr<sql::Statement> stmt(mConnect->createStatement());
				stmt->execute(command);
			}
			catch (const sql::SQLException& e) {
				responce = translate_sql_throw_message_to_peepo_table(e);
				logger.add_error(responce);
			}
			return responce;
		}

		//TODO:: a good way to harden binder, not using poly. template allows for customs
		template<typename T, typename Binder>
		std::string do_prepared_statement(const std::string& statement, const T& info, Binder&& binder)
		{
			auto& logger = Logger::instance();
			std::string responce = "Success";
		
			try {
				std::unique_ptr<sql::PreparedStatement> pstmt(mConnect->prepareStatement(statement));
				binder(pstmt.get(), info);
				pstmt->executeUpdate();
			}
			catch(const sql::SQLException&e ){
				responce = translate_sql_throw_message_to_peepo_table(e);
				logger.add_error(responce);
			}
			
			return responce;
		}
		//TODO:: a good way to harden binder, not using poly. template allows for customs
		//TODO2:: reseach if on a failure stop entierly or go to the next entry
		//TODO3:: a better way??? turn off auto commit -> prep statement - > commit -> reset auto commit
		//TODO4:: conflicts with the other overload when tempalte type deduction fails
		template<typename T, typename Binder>
		std::string do_prepared_statement(const std::string& statement,  std::span<const T> infos, Binder&& binder)
		{
			auto& logger = Logger::instance();
			std::string responce = "Success";
			bool autoCommitOriginal = true;


			try {
				//disable auto commit
				auto connector = mConnect.get();
				autoCommitOriginal = connector->getAutoCommit();
				connector->setAutoCommit(false);
				//prep statement
				std::unique_ptr<sql::PreparedStatement> pstmt(mConnect->prepareStatement(statement));
				//batch bind
				for (const T& info : infos) {
					binder(pstmt.get(), info);
					pstmt->executeUpdate();
				}

				//commit and reset
				mConnect->commit();
				mConnect->setAutoCommit(autoCommitOriginal);
			}
			catch (const sql::SQLException& e) {
				mConnect->rollback();
				responce = translate_sql_throw_message_to_peepo_table(e);
				logger.add_error(responce);
			}

			return responce;
		}

		void close()
		{
			mConnect.release();
			mDriver = nullptr;
		}

	private:
		sql::mysql::MySQL_Driver* mDriver = nullptr;
		std::unique_ptr<sql::Connection> mConnect = nullptr;
	};
}