#include "sqliteDatabase.h"

sqliteDatabase::sqliteDatabase(const char* db_file_name)
{
    sqlite3 *db = nullptr;

    if (sqlite3_open(db_file_name, &db))
    {
        std::string error_description = std::string("DB open/create error: ");
        error_description += std::string(sqlite3_errmsg(db));
        throw std::runtime_error(error_description);
    }
    else
    {
        sqliteDatabase::db_handle = db;
    }
}

sqliteDatabase::~sqliteDatabase() noexcept
{
    sqlite3_close(db_handle);
}

sqliteDatabase::sqliteDatabase(sqliteDatabase&& other) noexcept
{
    db_handle = other.db_handle;
    other.db_handle = nullptr;
}

sqliteDatabase& sqliteDatabase::operator=(sqliteDatabase&& other) noexcept
{
    db_handle = other.db_handle;
    other.db_handle = nullptr;
    return *this;
}
