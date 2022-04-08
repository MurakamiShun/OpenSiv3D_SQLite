#pragma once

namespace s3d::experimental {
	template<typename T>
	void SQLiteStatementBindHelper(SQLiteStatement&, int index, const T& var);

	template<typename T>
	SQLiteStatement& SQLiteStatement::bind(int index, const T& var) &{
		SQLiteStatementBindHelper<T>(*this, index, var);
		return *this;
	}

	template<typename T>
	SQLiteStatement&& SQLiteStatement::bind(int index, const T& var) && {
		SQLiteStatementBindHelper<T>(*this, index, var);
		return std::move(*this);
	}

	template<std::integral T>
	void SQLiteStatementBindHelper(SQLiteStatement& stmt, int index, const T& var) {
		if constexpr (sizeof(T) >= 8) {
			stmt.bind_int64(index, static_cast<int64_t>(var));
		}
		else if constexpr (sizeof(T) == 4 && std::is_unsigned_v<T>) {
			stmt.bind_int64(index, static_cast<int64_t>(var));
		}
		else {
			stmt.bind_int(index, static_cast<int>(var));
		}
	}
	template<typename T>
	requires std::same_as<T, Optional<typename T::value_type>> && std::integral<typename T::value_type>
		void SQLiteStatementBindHelper(SQLiteStatement& stmt, int index, const T& var) {
		using IntType = typename T::value_type;
		if (T.has_value()) {
			if constexpr (sizeof(IntType) >= 8) {
				stmt.bind_int64(index, static_cast<int64_t>(var.value()));
			}
			else if constexpr (sizeof(IntType) == 4 && std::is_unsigned_v<IntType>) {
				stmt.bind_int64(index, static_cast<int64_t>(var.value()));
			}
			else {
				stmt.bind_int(index, static_cast<int>(var.value()));
			}
		}
		else {
			stmt.bind_null(index);
		}
	}
	template<std::floating_point T>
	void SQLiteStatementBindHelper(SQLiteStatement& stmt, int index, const T& var) {
		stmt.bind_double(index, static_cast<double>(var));
	}
	template<typename T>
	requires std::same_as<T, Optional<typename T::value_type>> && std::floating_point<typename T::value_type>
		void SQLiteStatementBindHelper(SQLiteStatement& stmt, int index, const T& var) {
		if (T.has_value()) {
			stmt.bind_int64(index, static_cast<double>(var.value()));
		}
		else {
			stmt.bind_null(index);
		}
	}
	template<std::convertible_to<const StringView> T>
	void SQLiteStatementBindHelper(SQLiteStatement& stmt, int index, const T& var) {
		stmt.bind_text(index, static_cast<const StringView>(var));
	}
	template<typename T>
	requires std::same_as<T, Optional<typename T::value_type>> && std::convertible_to<typename T::value_type, const StringView>
	void SQLiteStatementBindHelper(SQLiteStatement& stmt, int index, const T& var) {
		if (T.has_value()) {
			stmt.bind_text(index, static_cast<const StringView>(var.value()));
		}
		else {
			stmt.bind_null(index);
		}
	}
	template<siv3d_serializable T>
	requires (!std::convertible_to<T, const StringView>)
	void SQLiteStatementBindHelper(SQLiteStatement& stmt, int index, const T& var) {
		Serializer<MemoryWriter> writer;
		writer(var);
		stmt.bind_blob(index, writer->getBlob().data(), writer->getBlob().size());
	}
	template<typename T>
	requires std::same_as<T, Optional<typename T::value_type>> && siv3d_serializable<typename T::value_type> && (!std::convertible_to<T, const StringView>)
		void SQLiteStatementBindHelper(SQLiteStatement& stmt, int index, const T& var) {
		if (T.has_value()) {
			Serializer<MemoryWriter> writer;
			writer(var.value());
			stmt.bind_blob(index, writer->getBlob().data(), writer->getBlob().size());
		}
		else {
			stmt.bind_null(index);
		}
	}
}
