#include "SQLiteAdaptorDetail.hpp"
#include "SQLiteStatementDetail.hpp"


namespace s3d::experimental {
	SQLiteAdaptor::SQLiteAdaptorDetail::SQLiteAdaptorDetail(const std::string& path, Mode mode) {
		open(path, mode);
	}

	SQLiteAdaptor::SQLiteAdaptorDetail::~SQLiteAdaptorDetail(){
		close();
	}

	bool SQLiteAdaptor::SQLiteAdaptorDetail::open(const std::string& path, Mode mode) {
		m_mode = [mode]() {
			switch (mode) {
			case Mode::readwrite:
				return SQLITE_OPEN_READWRITE;
			case Mode::create:
				return SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
			case Mode::readonly:
				return SQLITE_OPEN_READONLY;
			default:
				return SQLITE_OPEN_READONLY;
			};
		}();
		auto result = sqlite3_open_v2(path.c_str(), &m_handle, m_mode, nullptr);
		if (result != SQLITE_OK) {
			m_latest_error = sqlite3_errmsg(m_handle);
			m_is_opened = false;
			return false;
		}
		else {
			m_latest_error = std::nullopt;
			m_is_opened = true;
			return true;
		}
	}

	bool SQLiteAdaptor::SQLiteAdaptorDetail::is_opened() const noexcept {
		return m_is_opened;
	}

	void SQLiteAdaptor::SQLiteAdaptorDetail::close() {
		if (m_handle != nullptr) {
			sqlite3_close(m_handle);
			m_handle = nullptr;
		}
	}

	std::shared_ptr<SQLiteStatement::SQLiteStatementDetail> SQLiteAdaptor::SQLiteAdaptorDetail::make_statement(const std::string& sql_stmt) {
		return std::make_shared<SQLiteStatement::SQLiteStatementDetail>(m_handle, sql_stmt);
	}

	bool SQLiteAdaptor::SQLiteAdaptorDetail::exists_table(const std::string& table_name) {
		decltype(auto) query = "SELECT COUNT(*) FROM sqlite_master WHERE type = \"table\" AND name = ?1;";
		sqlite3_stmt* stmt = nullptr;
		auto result_code = sqlite3_prepare_v2(
			m_handle,
			query,
			sizeof(query),
			&stmt,
			nullptr
		);
		if (stmt == nullptr) return false;
		else if (result_code != SQLITE_OK) {
			sqlite3_reset(stmt);
			sqlite3_finalize(stmt);
			return false;
		}
		result_code = sqlite3_bind_text64(stmt, 1, table_name.c_str(), table_name.size(), nullptr, SQLITE_UTF8);
		if (result_code != SQLITE_OK) [[unlikely]] {
			sqlite3_reset(stmt);
			sqlite3_finalize(stmt);
			return false;
		}

		result_code = sqlite3_step(stmt);
		if (result_code != SQLITE_ROW) [[unlikely]] {
			sqlite3_finalize(stmt);
			return false;
		}

		const auto type = sqlite3_column_type(stmt, 0);
		if (type != SQLITE_INTEGER) [[unlikely]] {
			sqlite3_finalize(stmt);
			return false;
		}
		const auto exist = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);

		return exist == 1;
	}

	std::string SQLiteAdaptor::SQLiteAdaptorDetail::error_message() const {
		return m_latest_error.value();
	}
	bool SQLiteAdaptor::SQLiteAdaptorDetail::has_error() const noexcept {
		return m_latest_error.has_value();
	}
}
