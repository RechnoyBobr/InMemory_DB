#include "../inc/db.hpp"
#include <iostream>

// An example of using db
int main() {
    memdb::db db;
    auto res = db.execute(
        "create table users({autoincrement, key} id:int32 = 0, "
        "{unique}name:string[20], passwords_hash :bytes[10],banned:bool=false)");
    if (res.is_ok()) {
        std::cout << "Table was created!\n";
        std::cout << "Third column name is: "
                << db.debug_get_table_ith_col_name("users", 2) << "\n";
    }
    return 0;
}
