#pragma once

#include <string>
#include <mysql_driver.h>              //for driver+connect
#include <cppconn/prepared_statement.h>//for prep statements (inserts)
#include <span>
#include <memory>
#include "insert_statement.h"
#include "CharBuffer.h"

namespace badSQL
{
	template <typename T>
	concept BINDABLE = requires(sql::PreparedStatement * stmt, const T & v)
	{
		user_bind(stmt, v);//adl magic
	};

	class SQLInserter final
	{
	public:
		SQLInserter() = default;

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

				return e.what();
			}

			return "Success";
		}

		template <BINDABLE T>
		std::string inject(const T& item, const sql_insert_statement& statement)
		{
			if (!mConnect) {
				return "No Connection";
			}

			try {
				const std::string insert_statement = build_sql_insert_query(statement);

				std::unique_ptr<sql::PreparedStatement> pstmt(mConnect->prepareStatement(insert_statement));

				user_bind(pstmt.get(), item);

				pstmt->execute();
			}
			catch (const sql::SQLException& e) {	
				return e.what();
			}

			return "Success";
		}

		template <BINDABLE T>
		std::string inject(std::span<const T> data, const sql_insert_statement& statement) {
			//the table should be predefined IN WORKBENCH. if not log, abort and fuck off
			if (!mConnect) {
				return "No connection";
			}
			
		
			try {
				const std::string insert_statement = build_sql_insert_query(statement, data.size());
				std::unique_ptr<sql::PreparedStatement> pstmt(mConnect->prepareStatement(insert_statement));
		
				std::size_t param_index = 1;
		
				for (const auto& load : data) {
		
				}
		
			}
			catch (const sql::SQLException& e) {
		
			}
		
			return "Success";
		}

		//std::string inject(/*from file param*/ const std::string& insert_statement) {
		//	//the table should be predefined IN WORKBENCH. if not log, abort and fuck off
		//	if (!mConnect) {
		//		return "No connection";
		//	}
		//
		//
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
* 		//std::string set_schema(const std::string& schema) noexcept
		//{
		//	if (!mConnect)
		//		return "No connection";
		//
		//	try {
		//		mConnect->setSchema(schema);
		//	}
		//	catch (const sql::SQLException& e) {
		//		return translate_sql_throw_message_to_peepo_table(e);;
		//	}
		//
		//	return "Success";
		//}
* 
* 
* 
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