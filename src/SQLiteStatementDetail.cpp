#include "SQLiteStatementDetail.hpp"
#include "SQLiteExecutorDetail.hpp"

namespace s3d::experimental {
	SQLiteStatement::SQLiteStatementDetail::SQLiteStatementDetail(const sqlite3_ptr& parent_handle, const std::string& query)
	  : m_parent_handle(parent_handle) {
		const auto result_code = sqlite3_prepare_v2(
			m_parent_handle,
			query.c_str(),
			static_cast<int>(query.size()),
			&m_stmt,
			nullptr
		);
		if (result_code != SQLITE_OK || m_stmt == nullptr) {
			m_latest_error = sqlite3_errmsg(m_parent_handle);
		}
		else {
			m_latest_error = std::nullopt;
		}
	}
	SQLiteStatement::SQLiteStatementDetail::~SQLiteStatementDetail() {
		if (m_stmt != nullptr) {
			reset();
			sqlite3_finalize(m_stmt);
			m_stmt = nullptr;
		}
	}
	void SQLiteStatement::SQLiteStatementDetail::set_errmsg_if_error(int result_code) {
		if (result_code == SQLITE_OK) m_latest_error = std::nullopt;
		else m_latest_error = sqlite3_errmsg(m_parent_handle);
	}
	bool SQLiteStatement::SQLiteStatementDetail::reset() {
		auto result_code = sqlite3_clear_bindings(m_stmt);
		if (result_code != SQLITE_OK) {
			m_latest_error = sqlite3_errmsg(m_parent_handle);
			return false;
		}

		result_code = sqlite3_reset(m_stmt);
		if (result_code != SQLITE_OK) {
			m_latest_error = sqlite3_errmsg(m_parent_handle);
			return false;
		}
		else {
			m_latest_error = std::nullopt;
			return true;
		}
	}

	void SQLiteStatement::SQLiteStatementDetail::bind_int(int index, int var) {
		const auto result_code = sqlite3_bind_int(m_stmt, index, var);
		set_errmsg_if_error(result_code);
	}
	void SQLiteStatement::SQLiteStatementDetail::bind_int64(int index, int64_t var) {
		const auto result_code = sqlite3_bind_int64(m_stmt, index, var);
		set_errmsg_if_error(result_code);
	}
	void SQLiteStatement::SQLiteStatementDetail::bind_double(int index, double var) {
		const auto result_code = sqlite3_bind_double(m_stmt, index, var);
		set_errmsg_if_error(result_code);
	}
	void SQLiteStatement::SQLiteStatementDetail::bind_text(int index, const std::string& text) {
		const auto result_code = sqlite3_bind_text64(m_stmt, index, text.c_str(), text.size(), SQLITE_TRANSIENT, SQLITE_UTF8);
		set_errmsg_if_error(result_code);
	}
	void SQLiteStatement::SQLiteStatementDetail::bind_blob(int index, const void* var, std::size_t bytes) {
		const auto result_code = sqlite3_bind_blob64(m_stmt, index, var, bytes, SQLITE_TRANSIENT);
		set_errmsg_if_error(result_code);
	}
	void SQLiteStatement::SQLiteStatementDetail::bind_null(int index) {
		const auto result_code = sqlite3_bind_null(m_stmt, index);
		set_errmsg_if_error(result_code);
	}


	std::string SQLiteStatement::SQLiteStatementDetail::error_message() const {
		return m_latest_error.value();
	}

	bool SQLiteStatement::SQLiteStatementDetail::has_error() const noexcept {
		return m_latest_error.has_value();
	}

	std::shared_ptr<SQLiteExecutor::SQLiteExecutorDetail> SQLiteStatement::SQLiteStatementDetail::exec(std::shared_ptr<SQLiteStatementDetail>& stmt) {
		return std::make_shared<SQLiteExecutor::SQLiteExecutorDetail>(stmt);
	}
}
