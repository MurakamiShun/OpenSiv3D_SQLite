#pragma once
#include "../include/SQLiteAdaptor.hpp"
#include <string>
#include <optional>
#include <sqlite3.h>

namespace s3d::experimental {
	class SQLiteAdaptor::SQLiteAdaptorDetail {
	private:
		int m_mode;
		sqlite3* m_handle = nullptr;
		std::optional<std::string> m_latest_error = std::nullopt;
		bool m_is_opened;
	public:
		SQLiteAdaptorDetail(const std::string& path, Mode mode);
		SQLiteAdaptorDetail(const SQLiteAdaptorDetail&) = delete;
		void operator=(const SQLiteAdaptorDetail&) = delete;
		~SQLiteAdaptorDetail();

		bool open(const std::string& path, Mode mode);
		bool is_opened() const noexcept;
		void close();

		[[nodiscard]] std::shared_ptr<SQLiteStatement::SQLiteStatementDetail> make_statement(const std::string& sql_stmt);

		[[nodiscard]] bool exists_table(const std::string& table_name);

		[[nodiscard]] std::string error_message() const;
		[[nodiscard]] bool has_error() const noexcept;
	};
}
