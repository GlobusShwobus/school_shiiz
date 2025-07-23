#pragma once
#include <string_view>
#include <cppconn/prepared_statement.h>
#include <functional>

namespace ORDO {

	namespace query {
		struct SQLCommand {
			std::string command;
		};
		struct SQLInsertOp {
			std::string command;
			std::function<void(sql::PreparedStatement*)> binder;
		};
	}

	using SQLCommand = query::SQLCommand;
	using SQLInsertOp = query::SQLInsertOp;

	class SQLSchema {

		std::string schemaName;

	public:
		SQLSchema(std::string_view schemaName) :schemaName(schemaName) {}

		void setSchemaName(std::string_view name)
		{
			schemaName = name;
		}
		std::string_view getSchemaName()const {
			return schemaName;
		}

		SQLCommand                 createSchemaCommand()const {
			return SQLCommand("CREATE DATABASE " + std::string(schemaName) + ';');
		}
		virtual SQLCommand         createTableCommand()const = 0;
		virtual SQLInsertOp        insertQuery()const = 0;
		virtual std::string_view   getTableName()const = 0;

		virtual ~SQLSchema() = default;
	};
}