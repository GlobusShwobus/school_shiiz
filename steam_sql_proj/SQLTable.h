#pragma once
#include <string_view>
#include <cppconn/prepared_statement.h>
#include <functional>

namespace ORDO {
	class SQLTable {
	protected:
		std::string schemaName;

		struct SQLCommand {
			std::string_view command;
		};
		struct SQLInsertOp {
			std::string_view command;
			std::function<void(sql::PreparedStatement*)>binder;
		};

	public:
		SQLTable(std::string_view schemaName):schemaName(schemaName) {}
	
		virtual SQLCommand createSchema()const = 0;
		virtual SQLCommand setSchema()const = 0;
		virtual SQLCommand createTable()const = 0;
		virtual SQLInsertOp insertOperation()const = 0;
		virtual std::string_view tableName()const = 0;
		virtual ~SQLTable() = default;
	};
}