#include "Insert_statement.h"

namespace badSQL
{
	std::string build_sql_insert_query(const sql_insert_statement& build, std::size_t insert_count)
	{
		std::string sql;
		sql.reserve(128 + build.fields.size() * insert_count * 4);//a guess

		sql += "INSERT INTO ";
		sql += build.schema_name + "." + build.table_name + " (";


		for (const auto& field : build.fields)
			sql += field + ",";

		sql.back() = ')';//replace the last comma with )

		sql += "VALUES ";

		for (std::size_t i = 0; i < insert_count; ++i) {
			sql += "(";

			for (std::size_t j = 0; j < build.fields.size(); ++j)
				sql += "?,";

			sql.back() = ')';//fix last comma again
			sql += ",";
		}
		sql.back() = ';';//and again for final
		return sql;
	}
}