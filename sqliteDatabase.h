#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H

#include <string>
#include <vector>

#include "sqlite3.h"

class sqliteDatabase
{
public:
    sqliteDatabase(const char* db_file_name);
    ~sqliteDatabase() noexcept;
    sqliteDatabase& operator=(sqliteDatabase&& other) noexcept;
    sqliteDatabase(sqliteDatabase&& other) noexcept;

    sqliteDatabase& operator=(const sqliteDatabase& other) = delete;
    sqliteDatabase(const sqliteDatabase& other) = delete;

    template <typename... Args>
    std::vector< std::vector <std::string >>  executeQuery(std::string query, Args... args);

    template<>
    std::vector<std::vector<std::string>> executeQuery(std::string query);
private:
    sqlite3 *db_handle;
};


template <typename... Args>
int bindParameters(sqlite3_stmt * stmt, Args... args)
{
    return bindParameters_rec(stmt, 1, args...);
}

template <typename... Args>
int bindParameters_rec(sqlite3_stmt * stmt, int param_count)
{
    return SQLITE_OK;
}

template <typename... Args>
int bindParameters_rec(sqlite3_stmt * stmt, int param_count, unsigned int param, Args... args)
{
    int res = sqlite3_bind_int64(stmt, param_count, param);
    if (res != SQLITE_OK)
        return res;
    else
        return bindParameters_rec(stmt, param_count + 1, args...);
}

template <typename... Args>
int bindParameters_rec(sqlite3_stmt * stmt, int param_count, unsigned long param, Args... args)
{
    int res = sqlite3_bind_int64(stmt, param_count, param);
    if (res != SQLITE_OK)
        return res;
    else
        return bindParameters_rec(stmt, param_count + 1, args...);
}

template <typename... Args>
int bindParameters_rec(sqlite3_stmt * stmt, int param_count, int param, Args... args)
{
    int res = sqlite3_bind_int(stmt, param_count, param);
    if (res != SQLITE_OK)
        return res;
    else
        return bindParameters_rec(stmt, param_count + 1, args...);
}

template <typename... Args>
int bindParameters_rec(sqlite3_stmt * stmt, int param_count, bool param, Args... args)
{
    int res = sqlite3_bind_int(stmt, param_count, param);
    if (res != SQLITE_OK)
        return res;
    else
        return bindParameters_rec(stmt, param_count + 1, args...);
}

template <typename... Args>
int bindParameters_rec(sqlite3_stmt * stmt, int param_count, const char* param, Args... args)
{
    int res = sqlite3_bind_text(stmt, param_count, param, -1, SQLITE_STATIC);

    if (res != SQLITE_OK)
        return res;
    else
        return bindParameters_rec(stmt, param_count + 1, args...);
}

template <typename... Args>
int bindParameters_rec(sqlite3_stmt * stmt, int param_count, double param, Args... args)
{
    int res = sqlite3_bind_double(stmt, param_count, param);
    if (res != SQLITE_OK)
        return res;
    else
        return bindParameters_rec(stmt, param_count + 1, args...);
}

template <typename... Args>
std::vector< std::vector <std::string >> sqliteDatabase::executeQuery(std::string query, Args... args)
{
    sqlite3_stmt * stmt;
    sqlite3_prepare_v2(db_handle, query.data(), -1, &stmt, NULL);

    int bind_error_code = bindParameters(stmt, args...);

    if (bind_error_code != SQLITE_OK)
        throw std::runtime_error("");
        //return std::vector<std::vector<std::string>>();

    sqlite3_step(stmt);

    int columnCount = sqlite3_column_count(stmt);

    std::vector<std::vector<std::string>> result;
    for (int i = 0; i < columnCount; i++)
        result.push_back(std::vector<std::string>());

    while (sqlite3_column_text(stmt, 0))
    {
        for (int i = 0; i < columnCount; i++)
            result[i].push_back(std::string((char *)sqlite3_column_text(stmt, i)));
        sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

    return result;
}

template<>
std::vector< std::vector < std::string > >  sqliteDatabase::executeQuery(std::string query)
{
    sqlite3_stmt * stmt;
    sqlite3_prepare_v2(db_handle, query.data(), -1, &stmt, NULL);
    sqlite3_step(stmt);

    int columnCount = sqlite3_column_count(stmt);

    std::vector<std::vector<std::string>> result;
    for (int i = 0; i < columnCount; i++)
        result.push_back(std::vector<std::string>());

    while (sqlite3_column_text(stmt, 0))
    {
        for (int i = 0; i < columnCount; i++)
            result[i].push_back(std::string((char *)sqlite3_column_text(stmt, i)));
        sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

    return result;
}


#endif