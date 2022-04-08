#pragma once
#include <memory>
#include <Siv3D/String.hpp>
#include <Siv3D/StringView.hpp>
#include <Siv3D/Optional.hpp>
#include <Siv3D/Uncopyable.hpp>
#include <Siv3D/Blob.hpp>
#include <Siv3D/MemoryReader.hpp>
#include <Siv3D/MemoryWriter.hpp>
#include <Siv3D/Serialization.hpp>

namespace s3d::experimental {
	class SQLiteExecutor {
	private:
		friend class SQLiteStatement;
		class SQLiteExecutorDetail;
		std::shared_ptr<SQLiteExecutorDetail> m_detail;
	public:
		class EndofColumn {};

		class ColumnConvertor {
		private:
			std::shared_ptr<SQLiteExecutorDetail> m_extractor;
			const int m_col;
		public:
			ColumnConvertor(std::shared_ptr<SQLiteExecutorDetail>&, int col);

			[[nodiscard]] Optional<int> as_int();
			[[nodiscard]] Optional<int64_t> as_int64();
			[[nodiscard]] Optional<double> as_double();
			[[nodiscard]] Optional<String> as_text();
			[[nodiscard]] Optional<Blob> as_blob();

			template<typename T>
			operator T();
		};

		SQLiteExecutor(std::shared_ptr<SQLiteExecutorDetail>&&) noexcept;
		~SQLiteExecutor();
		[[nodiscard]] int column_count();
		[[nodiscard]] int row_count();

		SQLiteExecutor& next();

		bool is_done() const noexcept;

		enum class Type {
			integer,
			floating_point,
			text,
			blob,
			null
		};

		[[nodiscard]] Type get_type(int col);

		ColumnConvertor operator[](int col);
		[[nodiscard]] String error_message() const;
		[[nodiscard]] bool has_error() const noexcept;

		SQLiteExecutor& operator++() noexcept;
		bool operator!=(EndofColumn) noexcept;
		SQLiteExecutor& operator*() noexcept;

		SQLiteExecutor& begin();
		EndofColumn end() const noexcept;
	};

	class SQLiteStatement : public s3d::Uncopyable {
	private:
		friend class SQLiteAdaptor;
		friend class SQLiteExecutor;
		class SQLiteStatementDetail;
		std::shared_ptr<SQLiteStatementDetail> m_detail;
	public:
		SQLiteStatement(std::shared_ptr<SQLiteStatementDetail>&&);
		SQLiteStatement(SQLiteStatement&&);
		~SQLiteStatement();
		bool reset();

		SQLiteStatement& bind_int(int index, int var)&;
		SQLiteStatement&& bind_int(int index, int var)&&;
		SQLiteStatement& bind_int64(int index, int64_t var)&;
		SQLiteStatement&& bind_int64(int index, int64_t var)&&;
		SQLiteStatement& bind_double(int index, double var)&;
		SQLiteStatement&& bind_double(int index, double var)&&;
		SQLiteStatement& bind_text(int index, const StringView text)&;
		SQLiteStatement&& bind_text(int index, const StringView text)&&;
		SQLiteStatement& bind_blob(int index, const void* var, std::size_t bytes)&;
		SQLiteStatement&& bind_blob(int index, const void* var, std::size_t bytes)&&;
		SQLiteStatement& bind_null(int index)&;
		SQLiteStatement&& bind_null(int index)&&;
		template<typename T>
		SQLiteStatement& bind(int index, const T& var) &;
		template<typename T>
		SQLiteStatement&& bind(int index, const T& var) &&;
		[[nodiscard]] String error_message() const;
		[[nodiscard]] bool has_error() const noexcept;
		SQLiteExecutor exec();
	};

	class SQLiteAdaptor : public s3d::Uncopyable {
	private:
		class SQLiteAdaptorDetail;
		std::unique_ptr<SQLiteAdaptorDetail> m_detail;
	public:
		enum class Mode : int {
			readonly,
			readwrite,
			create
		};

		SQLiteAdaptor(const StringView path, Mode mode = Mode::create);
		~SQLiteAdaptor();

		bool open(const StringView path, Mode mode);
		bool is_opened() const noexcept;
		void close();

		[[nodiscard]] SQLiteStatement make_stmt(const StringView query);

		[[nodiscard]] bool exists_table(const StringView table_name);

		[[nodiscard]] String error_message() const;
		[[nodiscard]] bool has_error() const noexcept;
	};

	template<typename T>
	concept siv3d_serializable =
		std::invocable<Deserializer<MemoryReader>, T>
		&& std::invocable<Serializer<MemoryWriter>, T>;
}

#include "SQLiteStatement.ipp"
#include "ColumnConvertor.ipp"
