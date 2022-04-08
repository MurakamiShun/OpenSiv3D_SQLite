#include "SQLiteAdaptorDetail.hpp"
#include "SQLiteStatementDetail.hpp"
#include <Siv3D/Unicode.hpp>

namespace s3d::experimental {
	SQLiteAdaptor::SQLiteAdaptor(const StringView path, Mode mode)
		: m_detail(std::make_unique<SQLiteAdaptorDetail>(path.toUTF8(), mode)){
	}

	SQLiteAdaptor::~SQLiteAdaptor() = default;

	bool SQLiteAdaptor::open(const StringView path, Mode mode) {
		return m_detail->open(path.toUTF8(), mode);
	}
	bool SQLiteAdaptor::is_opened() const noexcept {
		return m_detail->is_opened();
	}
	void SQLiteAdaptor::close() {
		return m_detail->close();
	}

	SQLiteStatement SQLiteAdaptor::make_stmt(const StringView query) {
		return SQLiteStatement{ m_detail->make_statement(query.toUTF8()) };
	}

	bool SQLiteAdaptor::exists_table(const StringView table_name) {
		return m_detail->exists_table(table_name.toUTF8());
	}
	String SQLiteAdaptor::error_message() const {
		return Unicode::FromUTF8(m_detail->error_message());
	}
	bool SQLiteAdaptor::has_error() const noexcept {
		return m_detail->has_error();
	}
}
