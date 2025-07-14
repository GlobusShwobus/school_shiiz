#pragma once
#include <string_view>
#include <cppconn/prepared_statement.h>
#include <functional>

namespace ORDO {
	class SQLSchema {
	protected:

		std::string schemaName;

		struct SQLCommand {
			std::string_view command;
			SQLCommand(std::string_view) :command(command) {}
		};
		struct SQLInsertOperation {
			std::string_view command;
			std::function<void(sql::PreparedStatement*)> binder;
		};

	public:
		SQLSchema(std::string_view schemaName):schemaName(schemaName) {}
	
		void setSchemaName(std::string_view name)
		{
			schemaName = name;
		}
		SQLCommand                 createSchemaCommand()const {
			return SQLCommand("CREATE DATABASE " + std::string(schemaName) + ';');
		}
		virtual SQLCommand         createTableCommand()const = 0;
		virtual SQLInsertOperation insertQuery()const = 0;
		virtual std::string_view   getTableName()const = 0;

		virtual ~SQLSchema() = default;
	};
}