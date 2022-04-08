#include "SQLiteStatementDetail.hpp"
#include "SQLiteExecutorDetail.hpp"
#include <Siv3D/Unicode.hpp>

namespace s3d::experimental {
	SQLiteStatement::SQLiteStatement(std::shared_ptr<SQLiteStatement::SQLiteStatementDetail>&& detail)
		: m_detail(std::move(detail)){
	}
	SQLiteStatement::SQLiteStatement(SQLiteStatement&& stmt)
		: m_detail(std::move(stmt.m_detail)) {
	}

	SQLiteStatement::~SQLiteStatement() = default;

	bool SQLiteStatement::reset() {
		return m_detail->reset();
	}
	SQLiteStatement& SQLiteStatement::bind_int(int index, int var)& {
		m_detail->bind_int(index, var);
		return *this;
	}
	SQLiteStatement&& SQLiteStatement::bind_int(int index, int var)&& {
		m_detail->bind_int(index, var);
		return std::move(*this);
	}
	SQLiteStatement& SQLiteStatement::bind_int64(int index, int64_t var)& {
		m_detail->bind_int64(index, var);
		return *this;
	}
	SQLiteStatement&& SQLiteStatement::bind_int64(int index, int64_t var)&& {
		m_detail->bind_int64(index, var);
		return std::move(*this);
	}
	SQLiteStatement& SQLiteStatement::bind_double(int index, double var)& {
		m_detail->bind_double(index, var);
		return *this;
	}
	SQLiteStatement&& SQLiteStatement::bind_double(int index, double var)&& {
		m_detail->bind_double(index, var);
		return std::move(*this);
	}
	SQLiteStatement& SQLiteStatement::bind_text(int index, const StringView text)& {
		m_detail->bind_text(index, text.toUTF8());
		return *this;
	}
	SQLiteStatement&& SQLiteStatement::bind_text(int index, const StringView text)&& {
		m_detail->bind_text(index, text.toUTF8());
		return std::move(*this);
	}
	SQLiteStatement& SQLiteStatement::bind_blob(int index, const void* var, std::size_t bytes)& {
		m_detail->bind_blob(index, var, bytes);
		return *this;
	}
	SQLiteStatement&& SQLiteStatement::bind_blob(int index, const void* var, std::size_t bytes)&& {
		m_detail->bind_blob(index, var, bytes);
		return std::move(*this);
	}
	SQLiteStatement& SQLiteStatement::bind_null(int index)& {
		m_detail->bind_null(index);
		return *this;
	}
	SQLiteStatement&& SQLiteStatement::bind_null(int index)&& {
		m_detail->bind_null(index);
		return std::move(*this);
	}

	String SQLiteStatement::error_message() const {
		return Unicode::FromUTF8(m_detail->error_message());
	}
	bool SQLiteStatement::has_error() const noexcept {
		return m_detail->has_error();
	}

	SQLiteExecutor SQLiteStatement::exec() {
		return SQLiteExecutor{ m_detail->exec(m_detail) };
	}
}
