#pragma once
#include <string_view>
#include <cppconn/prepared_statement.h>
#include <functional>

namespace ORDO {

	namespace query {
		struct SQLCommand {
			std::string_view command;
		};
		struct SQLInsertOp {
			std::string_view command;
			std::function<void(sql::PreparedStatement*)> binder;
		};
	}

	using sqlCommand = query::SQLCommand;
	using sqlInsertOp = query::SQLInsertOp;

	class SQLSchema {
		
	protected:

		std::string schemaName;

	public:
		SQLSchema(std::string_view schemaName):schemaName(schemaName) {}
	
		void setSchemaName(std::string_view name)
		{
			schemaName = name;
		}
		std::string_view getSchemaName()const {
			return schemaName;
		}

		sqlCommand                 createSchemaCommand()const {
			return sqlCommand("CREATE DATABASE " + std::string(schemaName) + ';');
		}
		virtual sqlCommand         createTableCommand()const = 0;
		virtual sqlInsertOp        insertQuery()const = 0;
		virtual std::string_view   getTableName()const = 0;

		virtual ~SQLSchema() = default;
	};
}