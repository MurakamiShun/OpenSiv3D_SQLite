#pragma once
#include "../include/SQLiteAdaptor.hpp"
#include <string>
#include <optional>
#include <span>
#include <sqlite3.h>

namespace s3d::experimental {
	class SQLiteExecutor::SQLiteExecutorDetail {
	private:
		std::optional<std::string> m_latest_error = std::nullopt;
		int m_latest_result_code;
		std::shared_ptr<SQLiteStatement::SQLiteStatementDetail> m_sqlite_stmt;
	public:
		SQLiteExecutorDetail(std::shared_ptr<SQLiteStatement::SQLiteStatementDetail>& sqlite_stmt) noexcept;
		~SQLiteExecutorDetail();
		[[nodiscard]] int column_count();
		[[nodiscard]] int row_count();

		[[nodiscard]] Type get_type(int col);

		bool next();

		bool is_done() const noexcept;

		[[nodiscard]] std::string error_message() const;
		[[nodiscard]] bool has_error() const noexcept;

		[[nodiscard]] std::optional<int> get_int(int col);
		[[nodiscard]] std::optional<int64_t> get_int64(int col);
		[[nodiscard]] std::optional<double> get_double(int col);
		[[nodiscard]] std::optional<std::string> get_text(int col);
		[[nodiscard]] std::optional<std::span<const uint8_t>> get_blob(int col);
	};
}
