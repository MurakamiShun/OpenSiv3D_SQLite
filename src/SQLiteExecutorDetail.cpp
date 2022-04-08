#include "SQLiteExecutorDetail.hpp"
#include "SQLiteStatementDetail.hpp"
#include <span>

namespace s3d::experimental {
	// SQLiteExecutor
	SQLiteExecutor::SQLiteExecutorDetail::SQLiteExecutorDetail(std::shared_ptr<SQLiteStatement::SQLiteStatementDetail>& sqlite_stmt) noexcept
		: m_sqlite_stmt(sqlite_stmt) {
		next();
	}

	SQLiteExecutor::SQLiteExecutorDetail::~SQLiteExecutorDetail() {
		m_sqlite_stmt->reset();
	}

	int SQLiteExecutor::SQLiteExecutorDetail::column_count() {
		return sqlite3_column_count(m_sqlite_stmt->m_stmt);
	}
	int SQLiteExecutor::SQLiteExecutorDetail::row_count() {
		return sqlite3_data_count(m_sqlite_stmt->m_stmt);
	}

	SQLiteExecutor::Type SQLiteExecutor::SQLiteExecutorDetail::get_type(int col) {
		const auto type_code = sqlite3_column_type(m_sqlite_stmt->m_stmt, col);
		switch (type_code) {
		case SQLITE_INTEGER:
			return Type::integer;
		case SQLITE_FLOAT:
			return Type::floating_point;
		case SQLITE_TEXT:
			return Type::text;
		case SQLITE_BLOB:
			return Type::blob;
		case SQLITE_NULL:
		default:
			return Type::null;
		}
	}

	bool SQLiteExecutor::SQLiteExecutorDetail::next() {
		m_latest_result_code = sqlite3_step(m_sqlite_stmt->m_stmt);
		if (m_latest_result_code == SQLITE_DONE || m_latest_result_code == SQLITE_ROW) {
			m_latest_error = std::nullopt;
			return true;
		}
		else {
			m_latest_error = sqlite3_errmsg(m_sqlite_stmt->m_parent_handle);
			return false;
		}
	}

	bool SQLiteExecutor::SQLiteExecutorDetail::is_done() const noexcept {
		return m_latest_result_code != SQLITE_ROW;
	}

	std::string SQLiteExecutor::SQLiteExecutorDetail::error_message() const {
		return m_latest_error.value();
	}
	bool SQLiteExecutor::SQLiteExecutorDetail::has_error() const noexcept {
		return m_latest_error.has_value();
	}

	std::optional<int> SQLiteExecutor::SQLiteExecutorDetail::get_int(int col) {
		const auto type = sqlite3_column_type(m_sqlite_stmt->m_stmt, col);
		if (type == SQLITE_INTEGER) {
			return sqlite3_column_int(m_sqlite_stmt->m_stmt, col);
		}
		else if (type == SQLITE_NULL) {
			return std::nullopt;
		}
		else {
			throw std::bad_cast();
		}
	}

	std::optional<int64_t> SQLiteExecutor::SQLiteExecutorDetail::get_int64(int col) {
		const auto type = sqlite3_column_type(m_sqlite_stmt->m_stmt, col);
		if (type == SQLITE_INTEGER) {
			return sqlite3_column_int64(m_sqlite_stmt->m_stmt, col);
		}
		else if (type == SQLITE_NULL) {
			return std::nullopt;
		}
		else {
			throw std::bad_cast();
		}
	}

	std::optional<double> SQLiteExecutor::SQLiteExecutorDetail::get_double(int col) {
		const auto type = sqlite3_column_type(m_sqlite_stmt->m_stmt, col);
		if (type == SQLITE_FLOAT) {
			return sqlite3_column_double(m_sqlite_stmt->m_stmt, col);
		}
		else if (type == SQLITE_NULL) {
			return std::nullopt;
		}
		else {
			throw std::bad_cast();
		}
	}

	std::optional<std::string> SQLiteExecutor::SQLiteExecutorDetail::get_text(int col) {
		const auto type = sqlite3_column_type(m_sqlite_stmt->m_stmt, col);
		if (type == SQLITE_TEXT) {
			const unsigned char* text_ptr = sqlite3_column_text(m_sqlite_stmt->m_stmt, col);
			return std::make_optional<std::string>(reinterpret_cast<const char*>(text_ptr));
		}
		else if (type == SQLITE_NULL) {
			return std::nullopt;
		}
		else {
			throw std::bad_cast();
		}
	}

	std::optional<std::span<const uint8_t>> SQLiteExecutor::SQLiteExecutorDetail::get_blob(int col) {
		const auto type = sqlite3_column_type(m_sqlite_stmt->m_stmt, col);
		if (type == SQLITE_BLOB) {
			const uint8_t* ptr = reinterpret_cast<const uint8_t*>(sqlite3_column_blob(m_sqlite_stmt->m_stmt, col));
			const std::size_t size = static_cast<std::size_t>(sqlite3_column_bytes(m_sqlite_stmt->m_stmt, col));

			return std::span<const uint8_t>(ptr, size);
		}
		else if (type == SQLITE_NULL) {
			return std::nullopt;
		}
		else {
			throw std::bad_cast();
		}
	}
}
