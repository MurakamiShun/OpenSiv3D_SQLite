# include <Siv3D.hpp> // OpenSiv3D v0.6.1
#include "include/SQLiteAdaptor.hpp"

void Main() {
	experimental::SQLiteAdaptor db_adaptor{ U"hello.sqlite", experimental::SQLiteAdaptor::Mode::create };
	if (!db_adaptor.exists_table(U"hello_table")) {
		auto executor = db_adaptor.make_stmt(U"CREATE TABLE hello_table(name TEXT);").exec();
		if (executor.has_error()) {
			Print << executor.error_message();
		}
	}

	if (auto executor = db_adaptor.make_stmt(U"SELECT count(*) FROM hello_table;").exec(); !executor.has_error()) {
		if (static_cast<int>(executor[0]) == 0) {
			auto stmt = db_adaptor.make_stmt(U"INSERT INTO hello_table(name) VALUES (?1);");

			const Array<String> names = {
				U"斎藤",
				U"eisa",
				U"後藤",
				U"遠藤",
				U"面倒"
			};

			for (const auto& name : names) {
				if (auto e = stmt.bind(1, name).exec(); e.has_error()) {
					Print << e.error_message();
					break;
				}
			}
		}
	}


	Array<std::pair<uint64_t, String>> names;
	for (auto e : db_adaptor.make_stmt(U"SELECT OID, name FROM hello_table;").exec()) {
		names.push_back({ e[0], e[1] });
		Print << names.back().first << U":" << names.back().second;
	}
	
	// 背景の色を設定 | Set background color
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	while (System::Update()) {
	}
}
