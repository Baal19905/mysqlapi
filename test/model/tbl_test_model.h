#ifndef _TBL_TEST_MODEL_H_
#define _TBL_TEST_MODEL_H_
#include "mysqlclient.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unordered_set>
#include <unordered_map>

struct tbl_test_model_info
{
    int32_t id;
    buffer_field name;
    buffer_field buff;
    buffer_field create_ts;
    buffer_field update_ts;
};

struct tbl_test_model_primary
{
    int32_t id;
};


class tbl_test_model
{
public:
    tbl_test_model();
    ~tbl_test_model();
    void set_mysql(mysql_api *mysql){mysql_ = mysql;}
    void set_dump_path(const char *path)
    {
        dump_file_ = path;
        dump_file_ += "/tbl_test_model.sql";
        mkdir(path, 0755);
    }
    std::string &msg() {return msg_;}
    int32_t code() {return code_;}
public:
    int32_t insert(const std::unordered_set<std::string> &omit_fields = {});
    int32_t select(const std::string &where = "", const std::unordered_set<std::string> &omit_fields = {});
    tbl_test_model_info *fetch();
    int32_t count(const std::string &where = "");
    int32_t del(const std::string &where = "");
    int32_t select_by_primary(int32_t id, const std::unordered_set<std::string> &omit_fields = {});
    tbl_test_model_info *fetch_by_primary();
    int32_t update_by_primary(int32_t id, const std::unordered_set<std::string> &omit_fields = {});
    int32_t del_by_primary(int32_t id, const std::unordered_set<std::string> &omit_fields = {});
    tbl_test_model_info *get_info()
    {
        zero_info();
        return &info_;
    }
    void zero_info();
private:
    char *hex_format(char *binary, int32_t binary_len, char *buf)
    {
        if (binary == nullptr || buf == nullptr || binary_len == 0) {
            strcpy(buf, "null");
            return buf;
        }
        strcpy(buf, "0x");
        char *tmp = buf + 2;
        for (int32_t i=0; i < binary_len; i++, tmp += 2) {
            sprintf(tmp, "%02x", (unsigned char)*(binary++));
        }
        return buf;
    }
    void dump(const char *sql);
    std::string &insert_dump_sql(std::string &sql, const std::unordered_set<std::string> &omit_fields);
    std::string &update_dump_sql_by_primary(std::string &sql, const std::unordered_set<std::string> &omit_fields);
    std::string &del_dump_sql_by_primary(std::string &sql);
private:
    tbl_test_model_info info_;
    mysql_api *mysql_ = {nullptr};
    mysql_stmt *stmt_insert_ = {nullptr};
    mysql_stmt *stmt_select_by_primary_ = {nullptr};
    mysql_stmt *stmt_del_by_primary_ = {nullptr};
    tbl_test_model_primary where_primary_;
    std::vector<buffer_field> buffer_array_;
    std::string msg_;
    int32_t code_;
    std::string dump_file_;
    std::vector<std::pair<std::string, std::string>> all_fields_;
    std::unordered_map<std::string, mysql_stmt *> stmts_;
};
#endif ///< _TBL_TEST_MODEL_H_
