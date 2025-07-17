#pragma once
#include <string_view>
#include <cppconn/prepared_statement.h>
#include <functional>

namespace ORDO {

	class SQLFlight;

	class SQLSchema {

		friend class SQLFlight;

	protected:

		std::string schemaName;

		struct SQLCommand {
			std::string command;
			SQLCommand(std::string_view str) :command(str.data()) {}
		};
		struct SQLInsertOp {
			std::string command;
			std::function<void(sql::PreparedStatement*)> binder;
		};

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