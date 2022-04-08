#pragma once

namespace s3d::experimental {
	template<typename T>
	T SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor&);

	template<typename T>
	SQLiteExecutor::ColumnConvertor::operator T() {
		return SQLiteColumnConvertorHelper<T>(*this);
	}

	template<typename T>
	requires std::integral<T>
	T SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor& cvt) {
		if constexpr (sizeof(T) >= 8) {
			return cvt.as_int64().value();
		}
		else {
			return cvt.as_int().value();
		}
	}
	template<typename T>
	requires std::same_as<T, Optional<typename T::value_type>> && std::integral<typename T::value_type>
	Optional<T> SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor& cvt) {
		if constexpr (sizeof(T) >= 8) {
			return cvt.as_int64();
		}
		else {
			return cvt.as_int();
		}
	}
	template<typename T>
	requires std::floating_point<T>
	T SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor& cvt) {
		return cvt.as_double().value();
	}
	template<typename T>
	requires std::same_as<T, Optional<typename T::value_type>> && std::floating_point<typename T::value_type>
	T SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor& cvt) {
		return cvt.as_double();
	}
	template<std::same_as<String> T>
	String SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor& cvt) {
		return cvt.as_text().value();
	}
	template<std::same_as<Optional<String>> T>
	Optional<String> SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor& cvt) {
		return cvt.as_text();
	}

	template<siv3d_serializable T>
	requires (!std::integral<T>) && (!std::same_as<T, String>)
	T SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor& cvt) {
		Deserializer<MemoryReader> reader{ cvt.as_blob().value() };
		T t;
		reader(t);
		return t;
	}
	template<typename T>
	requires std::same_as<T, Optional<typename T::value_type>> && siv3d_serializable<typename T::value_type> && (!std::integral<typename T::value_type>) && (!std::same_as<typename T::value, String>)
	T SQLiteColumnConvertorHelper(SQLiteExecutor::ColumnConvertor& cvt) {
		const auto binary = cvt.as_blob();
		if (binary) {
			Deserializer<MemoryReader> reader{ cvt.as_blob().value() };
			typename T::value_type t;
			reader(t);
			return t;
		}
		else {
			return std::nullopt;
		}
	}
}
