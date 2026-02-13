#pragma once

#include <string_view>
#include <string>
#include "Sequence.h"

namespace badSQL
{
	struct sql_insert_statement
	{
		sql_insert_statement(std::string_view schema, std::string_view table)
			:schema_name(schema), table_name(table)
		{
		}

		std::string schema_name;
		std::string table_name;
		Sequence<std::string> fields;
	};


	std::string build_sql_insert_query(const sql_insert_statement& build, std::size_t insert_count = 1);
}