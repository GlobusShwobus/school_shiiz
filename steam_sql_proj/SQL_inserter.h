#pragma once

#include <string>
#include <mysql_driver.h>              //for driver+connect
#include <cppconn/prepared_statement.h>//for prep statements (inserts)
#include <span>
#include <memory>
#include "CharBuffer.h"
#include "payload.h"

namespace badSQL
{
	std::string translate_sql_throw_message_to_peepo_table(const sql::SQLException& e) noexcept
	{
		const std::string& state = e.getSQLState();
		const int code = e.getErrorCode();

		std::string translated = "(" + std::to_string(code) + ") ";

		if (state == "HYT00" || state == "HYT01")
			return "Connection timed out";

		switch (code) {
		//CONNECTION ERRORS
		case 2003:
			translated += "Server unreachable"; break;
		case 2005:
			translated += "Invalid host"; break;
		case 1045:
		case 2059:
			//technically another issue entierly in case of 2059 but i don't get enough yet so keep it this way
			translated += "Invalid username or password"; break;
		case 1040:
			translated += "Server is full..."; break;
		case 2006://server fucked off? whatever that means
		case 2013://lost connection during query
		case 2055://gives a better error but maybe not super important
			translated += "Connection lost"; break;
		//TABLE/DB ACCESS
		case 1044:
			translated += "Access denied"; break;
		case 1049:
			translated += "Database does not exist"; break;
		case 1046:
			translated += "No database selected"; break;
		case 1059:
		case 1102:
			translated += "Invalid name"; break;

		case -1001:
			translated += "ID missing"; break;
		case -1002:
			translated += "Label missing"; break;
		case -1003:
			translated += "Data missing"; break;

		case 1136:
			translated += "Missmatch between field count and value count";
			break;
		//DON'T DEFINE THOSE, THEY'RE FINE AS DEFAULTS
		//insert failures
		//case 1054://basic typo errors, like 'stemid' doesnt exist when it should be 'steamid'
		//case 1265://truncation error, probably enum
		//	//TODO:: log the actual accurate error. SQL provides very specific failures so... they're good for devs. just not users.
		//	translated = "Insert syntax error"; break;
		//case 1064:
		//	translated = "SQL syntax error"; break;
		//case 1062:
		//	translated = "Duplicate entry"; break;
		//case 1050:
		//	translated = "Table already exists"; break;
		//case 1007:
		//	translated = "Schema already exists"; break;
		//case 1142:
		//case 2002:
		//	translated = "Local host unreachable"; break;

		default:
			translated += e.what();
			break;
		}
		return translated;
	}

	class SQLInserter final
	{
	public:
		SQLInserter() = default;
		SQLInserter(const std::string& user, const std::string& service = "tcp://127.0.0.1:3306")
		{
			connect(user, service);
		}
		std::string connect(const std::string& user, const std::string& service) noexcept
		{
			close();

			try {
				CharBuffer password;
				mDriver = sql::mysql::get_driver_instance();
				mConnect.reset(mDriver->connect(service, user, password.data()));
			}
			catch (const sql::SQLException& e) {
				mConnect.release();
				mDriver = nullptr;

				return translate_sql_throw_message_to_peepo_table(e);;
			}

			return "Success";
		}

		std::string set_schema(const std::string& schema) noexcept
		{
			if (!mConnect)
				return "No connection";

			try {
				mConnect->setSchema(schema);
			}
			catch (const sql::SQLException& e) {
				return translate_sql_throw_message_to_peepo_table(e);;
			}

			return "Success";
		}

		std::string inject(const Payload& item, const std::string& insert_statement)
		{
			if (!mConnect) {
				return "No Connection";
			}

			try {
				std::unique_ptr<sql::PreparedStatement> pstmt(mConnect->prepareStatement(insert_statement));

				if (!item.recipient_id.has_value()) throw sql::SQLException("", "", -1001);
				if (!item.label.has_value())        throw sql::SQLException("", "", -1002);
				if (!item.data.has_value())         throw sql::SQLException("", "", -1003);

				pstmt->setInt64 (1, *item.recipient_id);
				pstmt->setString(2, *item.label);
				pstmt->setString(3, item.data->dump());
				pstmt->execute();
			}
			catch (const sql::SQLException& e) {	
				return translate_sql_throw_message_to_peepo_table(e);
			}

			return "Success";
		}
		//std::string inject(std::span<const Payload> data) {
		//	//the table should be predefined IN WORKBENCH. if not log, abort and fuck off
		//	if (!mConnect) {
		//		return "No connection";
		//	}
		//
		//	try {
		//
		//	}
		//	catch (const sql::SQLException& e) {
		//
		//	}
		//
		//	return "Success";
		//}

		//for automation purposes, assuming schema and inejct worked try to call a stored procedure to automate actual distribution
		std::string call_procedure(const std::string& procedure) {
			//if fails log, abort and fuck off
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

/*
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
*/