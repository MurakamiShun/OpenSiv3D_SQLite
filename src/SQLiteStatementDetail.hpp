#pragma once
#include "../include/SQLiteAdaptor.hpp"
#include <string>
#include <optional>
#include <sqlite3.h>

namespace s3d::experimental {
	class SQLiteStatement::SQLiteStatementDetail {
	private:
		std::optional<std::string> m_latest_error = std::nullopt;
		void set_errmsg_if_error(int result_code);
	public:
		using sqlite3_ptr = sqlite3*;
		const sqlite3_ptr& m_parent_handle;
		sqlite3_stmt* m_stmt = nullptr;

		SQLiteStatementDetail(const sqlite3_ptr& parent_handle, const std::string& query);
		~SQLiteStatementDetail();
		bool reset();
		void bind_int(int index, int var);
		void bind_int64(int index, int64_t var);
		void bind_double(int index, double var);
		void bind_text(int index, const std::string& text);
		void bind_blob(int index, const void* var, std::size_t bytes);
		void bind_null(int index);
		[[nodiscard]] std::string error_message() const;
		[[nodiscard]] bool has_error() const noexcept;
		static std::shared_ptr<SQLiteExecutor::SQLiteExecutorDetail> exec(std::shared_ptr<SQLiteStatementDetail>&);
	};
}
