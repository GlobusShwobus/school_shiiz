#pragma once
#include <string>
#include "Sequence.h"
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
namespace badSQL
{
	/// TODO::
	/// CUSTOM SECTION
	/// 
	/// 
	struct Column
	{
		std::string name;
		std::string type; //SQL types
		Sequence<std::string> constraints;// NOT NULL, AUTO_INCREMENT... should not be sued for primary and foreign keys
	};

	// EXAMPLE USAGE:
	//                  auto sql = create_table_command(
	//                  	"achievements",                                            
	//                  	{
	//                  		{ "game_id", "BIGINT", { "NOT NULL" } },
	//                  		{ "achievement_id", "VARCHAR(40)", { "NOT NULL" } },
	//                  		{ "name", "VARCHAR(100)" },
	//                  		{ "description", "VARCHAR(256)" }
	//                  	},
	//                  {
	//                  	"PRIMARY KEY (game_id, achievement_id)"
	//                  }
	//                  );
	std::string create_custom_table_command(std::string_view table_name, Sequence<Column> columns, Sequence<std::string> table_constraints = {})
	{
		std::string command = "CREATE TABLE ";
		command += table_name;
		command += " (";

		bool first = true;
		for (const auto& col : columns) {

			command += col.name + ' ' + col.type;	       // add the name and the type of the field
			//
			// 
			for (const auto& c : col.constraints) {        // add constraints associated with the given type
				command += ' ' + c;
			}

			if (!first) 							       // add a comma after every field except the first iteration
				command += ", ";					       //
			first = false;							       // ...might get optimzed away
		}

		for (const auto& constraint : table_constraints) { //add table level constrains, mainly key info
			command += ", " + constraint;
		}

		command += ");";

		return command;
	}



	//TODO:: function to see if custom table and insert commands fit
}