#include "SQLiteExecutorDetail.hpp"
#include <Siv3D/Unicode.hpp>

namespace s3d::experimental {
	SQLiteExecutor::SQLiteExecutor(std::shared_ptr<SQLiteExecutorDetail>&& extractor) noexcept
		: m_detail(std::move(extractor)){
	}
	int SQLiteExecutor::column_count() {
		return m_detail->column_count();
	}
	int SQLiteExecutor::row_count() {
		return m_detail->row_count();
	}

	SQLiteExecutor::~SQLiteExecutor() = default;

	SQLiteExecutor& SQLiteExecutor::next() {
		m_detail->next();
		return *this;
	}

	bool SQLiteExecutor::is_done() const noexcept {
		return m_detail->is_done();
	}

	SQLiteExecutor::Type SQLiteExecutor::get_type(int col) {
		return m_detail->get_type(col);
	}

	SQLiteExecutor::ColumnConvertor SQLiteExecutor::operator[](int col) {
		return SQLiteExecutor::ColumnConvertor{ m_detail, col };
	}
	String SQLiteExecutor::error_message() const {
		return Unicode::Widen(m_detail->error_message());
	}
	bool SQLiteExecutor::has_error() const noexcept {
		return m_detail->has_error();
	}

	SQLiteExecutor& SQLiteExecutor::operator++() noexcept {
		m_detail->next();
		return *this;
	}
	bool SQLiteExecutor::operator!=(EndofColumn) noexcept {
		return !(m_detail->is_done());
	}
	SQLiteExecutor& SQLiteExecutor::operator*() noexcept {
		return *this;
	}

	SQLiteExecutor& SQLiteExecutor::begin() {
		return *this;
	}
	SQLiteExecutor::EndofColumn SQLiteExecutor::end() const noexcept {
		return EndofColumn();
	}

	SQLiteExecutor::ColumnConvertor::ColumnConvertor(std::shared_ptr<SQLiteExecutorDetail>& ext, int col)
		: m_extractor(ext),
		m_col(col){
	}

	Optional<int> SQLiteExecutor::ColumnConvertor::as_int() {
		const auto val = m_extractor->get_int(m_col);
		if (val) return val.value();
		else return std::nullopt;
	}
	Optional<int64_t> SQLiteExecutor::ColumnConvertor::as_int64() {
		const auto val = m_extractor->get_int64(m_col);
		if (val) return val.value();
		else return std::nullopt;
	}
	Optional<double> SQLiteExecutor::ColumnConvertor::as_double() {
		const auto val = m_extractor->get_double(m_col);
		if (val) return val.value();
		else return std::nullopt;
	}
	Optional<String> SQLiteExecutor::ColumnConvertor::as_text() {
		const auto val = m_extractor->get_text(m_col);
		if (val) return Unicode::FromUTF8(val.value());
		else return std::nullopt;
	}
	Optional<Blob> SQLiteExecutor::ColumnConvertor::as_blob() {
		const auto val = m_extractor->get_blob(m_col);
		if (val) return MakeOptional<Blob>(val.value().data(), val.value().size_bytes());
		else return std::nullopt;
	}
}
