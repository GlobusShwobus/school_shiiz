#include "SQLFlight.h"

namespace ORDO {

	bool SQLFlight::setSchema(std::string_view name) {
		ConnectionCheck();
		try {
			mConnect->setSchema(name.data());
		}
		catch (sql::SQLException& expt) {
			SSQLStreamError = "Schema assignment Failure: " + std::string(expt.what());
			return false;
		}
		return true;
	}
	bool SQLFlight::doCommand(const SQLCommand command) {
		ConnectionCheck();

		try {
			std::unique_ptr<sql::Statement> stmt(mConnect->createStatement());
			stmt->execute(command.command.data());
		}
		catch (sql::SQLException& expt) {
			SSQLStreamError = "Command failure " + std::string(expt.what());
			return false;
		}

		return true;
	}
	bool SQLFlight::doPreparedInsert(SQLInsertOp op) {
		ConnectionCheck();

		try {
			std::unique_ptr<sql::PreparedStatement> pstmt(mConnect->prepareStatement(op.command.data()));
			op.binder(pstmt.get());
			pstmt->executeUpdate();
		}
		catch (sql::SQLException& expt) {
			SSQLStreamError = "Prepard Insert failure " + std::string(expt.what());
			return false;
		}

		return true;
	}
}