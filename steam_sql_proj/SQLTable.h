#pragma once
#include <string_view>
#include <cppconn/prepared_statement.h>
namespace ORDO {
	class SQLTable {
	public:

		virtual std::string_view createTableStatement() const = 0;
		virtual std::string_view createInsertStatement()const = 0;
		virtual void bindToStatement(sql::PreparedStatement* stmt)const = 0;
		virtual std::string_view tableName()const = 0;
		virtual ~SQLTable() = default;
	};
}