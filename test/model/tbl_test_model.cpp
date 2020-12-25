#include "tbl_test_model.h"
#include <string.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

tbl_test_model::tbl_test_model()
{
    info_.name.buf = new char[100 + 1];
    info_.name.size = 100 + 1;
    info_.buff.buf = new char[1000 + 1];
    info_.buff.size = 1000 + 1;
    info_.create_ts.buf = new char[26 + 1];
    info_.create_ts.size = 26 + 1;
    info_.update_ts.buf = new char[26 + 1];
    info_.update_ts.size = 26 + 1;
    zero_info();
    buffer_array_.resize(1, {0});
    all_fields_.push_back({"id", "int"});
    all_fields_.push_back({"name", "varchar"});
    all_fields_.push_back({"buff", "varbinary"});
    all_fields_.push_back({"create_ts", "datetime"});
    all_fields_.push_back({"update_ts", "timestamp"});
}

tbl_test_model::~tbl_test_model()
{
    delete []info_.name.buf;
    delete []info_.buff.buf;
    delete []info_.create_ts.buf;
    delete []info_.update_ts.buf;
}

void tbl_test_model::zero_info()
{
    memset(&info_.id, 0, sizeof(info_.id));
    memset(info_.name.buf, 0, 100 + 1);
    info_.name.len = 0;
    memset(info_.buff.buf, 0, 1000 + 1);
    info_.buff.len = 0;
    memset(info_.create_ts.buf, 0, 26 + 1);
    info_.create_ts.len = 0;
    memset(info_.update_ts.buf, 0, 26 + 1);
    info_.update_ts.len = 0;
}

int32_t tbl_test_model::insert(const std::unordered_set<std::string> &omit_fields)
{
    std::string fields = "(";
    std::string values = "(";
    for (auto &e : all_fields_) {
        if (omit_fields.find(e.first) == omit_fields.end()) {
            fields += e.first + ",";
            values += "?,";
        }
    }
    fields[fields.length() - 1] = ')';
    values[values.length() - 1] = ')';
    std::string sql = "insert into tbl_test_model " + fields + " values " + values;
    mysql_stmt *stmt = nullptr;
    if ((stmt = stmts_[sql]) == nullptr) {
        stmt =  mysql_->prepare_stmt(sql);
        if (stmt == nullptr) {
            code_ = mysql_->code();
            msg_ = mysql_->msg();
            sql = insert_dump_sql(sql, omit_fields);
            dump(sql.c_str());
            return -1;
        }
        stmts_[sql] = stmt;
        if (omit_fields.find("id") == omit_fields.end()) {
            stmt->set_param_long(info_.id);
        }
        if (omit_fields.find("name") == omit_fields.end()) {
            stmt->set_param_string(&info_.name);
        }
        if (omit_fields.find("buff") == omit_fields.end()) {
            stmt->set_param_blob(&info_.buff);
        }
        if (omit_fields.find("create_ts") == omit_fields.end()) {
            stmt->set_param_datetime(&info_.create_ts);
        }
        if (omit_fields.find("update_ts") == omit_fields.end()) {
            stmt->set_param_timestamp(&info_.update_ts);
        }
    }
    if (stmt->execute() != 0) {
        code_ = stmt->code();
        msg_ = stmt->msg();
        std::string sql;
        sql = insert_dump_sql(sql, omit_fields);
        dump(sql.c_str());
        return -1;
    }
    return 0;
}
int32_t tbl_test_model::select_by_primary(int32_t id, const std::unordered_set<std::string> &omit_fields)
{
    std::string fields;
    for (auto &e : all_fields_) {
        if (omit_fields.find(e.first) == omit_fields.end()) {
            if (e.second == "timestamp" || e.second == "datetime") {
                fields += "date_format(" + e.first + ",'%Y-%m-%d_%H:%i:%s.%f'),";
            }
            else {
                fields += e.first + ",";
            }
        }
    }
    fields.pop_back();
    std::string sql = "select " + fields + " from tbl_test_model where id = ?";
    mysql_stmt *stmt = nullptr;
    if ((stmt = stmts_[sql]) == nullptr) {
        stmt =  mysql_->prepare_stmt(sql);
        if (stmt == nullptr) {
            code_ = mysql_->code();
            msg_ = mysql_->msg();
            return -1;
        }
        stmt_select_by_primary_ = stmt;
        stmts_[sql] = stmt;
        if (omit_fields.find("id") == omit_fields.end()) {
            stmt->set_result_long(info_.id);
        }
        if (omit_fields.find("name") == omit_fields.end()) {
            stmt->set_result_string(&info_.name);
        }
        if (omit_fields.find("buff") == omit_fields.end()) {
            stmt->set_result_blob(&info_.buff);
        }
        if (omit_fields.find("create_ts") == omit_fields.end()) {
            stmt->set_result_datetime(&info_.create_ts);
        }
        if (omit_fields.find("update_ts") == omit_fields.end()) {
            stmt->set_result_timestamp(&info_.update_ts);
        }
        stmt->set_param_long(info_.id);
    }
    info_.id = id;
    if (stmt->execute() != 0) {
        code_ = stmt->code();
        msg_ = stmt->msg();
        return -1;
    }
    return 0;
}

tbl_test_model_info * tbl_test_model::fetch_by_primary()
{
    if (stmt_select_by_primary_ == nullptr) {
        code_ = -1;
        msg_ = "stmt not prepared!";
        return nullptr;
    }
    zero_info();
    if (stmt_select_by_primary_->fetch() != 0) {
        code_ = stmt_select_by_primary_->code();
        msg_ = stmt_select_by_primary_->msg();
        return nullptr;
    }
    return &info_;
}

int32_t tbl_test_model::update_by_primary(int32_t id, const std::unordered_set<std::string> &omit_fields)
{
    std::string fields;
    for (auto &e : all_fields_) {
        if (omit_fields.find(e.first) == omit_fields.end()) {
            fields += e.first + " = ?,";
        }
    }
    fields.pop_back();
    std::string sql = "update tbl_test_model set " + fields + " where id = ?";
    mysql_stmt *stmt = nullptr;
    if ((stmt = stmts_[sql]) == nullptr) {
        stmt =  mysql_->prepare_stmt(sql);
        if (stmt == nullptr) {
            code_ = mysql_->code();
            msg_ = mysql_->msg();
            return -1;
        }
        stmts_[sql] = stmt;
        if (omit_fields.find("id") == omit_fields.end()) {
            stmt->set_param_long(info_.id);
        }
        if (omit_fields.find("name") == omit_fields.end()) {
            stmt->set_param_string(&info_.name);
        }
        if (omit_fields.find("buff") == omit_fields.end()) {
            stmt->set_param_blob(&info_.buff);
        }
        if (omit_fields.find("create_ts") == omit_fields.end()) {
            stmt->set_param_datetime(&info_.create_ts);
        }
        if (omit_fields.find("update_ts") == omit_fields.end()) {
            stmt->set_param_timestamp(&info_.update_ts);
        }
        stmt->set_param_long(where_primary_.id);
    }
    where_primary_.id = id;
    if (stmt->execute() != 0) {
        code_ = stmt->code();
        msg_ = stmt->msg();
        std::string sql;
        sql = update_dump_sql_by_primary(sql, omit_fields);
        dump(sql.c_str());
        return -1;
    }
    return 0;
}

int32_t tbl_test_model::del_by_primary(int32_t id, const std::unordered_set<std::string> &omit_fields)
{
    if (stmt_del_by_primary_ == nullptr) {
        std::string sql = "delete from  tbl_test_model where id = ?";
        stmt_del_by_primary_ =  mysql_->prepare_stmt(sql);
        if (stmt_del_by_primary_ == nullptr) {
            code_ = mysql_->code();
            msg_ = mysql_->msg();
            sql = del_dump_sql_by_primary(sql);
            dump(sql.c_str());
            return -1;
        }
        stmt_del_by_primary_->set_param_long(where_primary_.id);
    }
    where_primary_.id = id;
    if (stmt_del_by_primary_->execute() != 0) {
        code_ = stmt_del_by_primary_->code();
        msg_ = stmt_del_by_primary_->msg();
        std::string sql;
        sql = del_dump_sql_by_primary(sql);
        dump(sql.c_str());
        return -1;
    }
    return 0;
}

int32_t tbl_test_model::select(const std::string &where, const std::unordered_set<std::string> &omit_fields)
{
    std::string fields;
    for (auto &e : all_fields_) {
        if (omit_fields.find(e.first) == omit_fields.end()) {
            if (e.second == "timestamp" || e.second == "datetime") {
                fields += "date_format(" + e.first + ",'%Y-%m-%d_%H:%i:%s.%f'),";
            }
            else {
                fields += e.first + ",";
            }
        }
    }
    fields.pop_back();
    std::string sql = "select " + fields + " from tbl_test_model " + where;
    if (mysql_->execute(sql)) {
        code_ = mysql_->code();
        msg_ = mysql_->msg();
        return -1;
    }
    if (omit_fields.find("id") == omit_fields.end()) {
        buffer_array_[0].buf = (char *)&info_.id;
        buffer_array_[0].size = sizeof(info_.id);
        mysql_->set_field(0, &buffer_array_[0]);
    }
    if (omit_fields.find("name") == omit_fields.end()) {
        mysql_->set_field(0, &info_.name);
    }
    if (omit_fields.find("buff") == omit_fields.end()) {
        mysql_->set_field(0, &info_.buff);
    }
    if (omit_fields.find("create_ts") == omit_fields.end()) {
        mysql_->set_field(0, &info_.create_ts);
    }
    if (omit_fields.find("update_ts") == omit_fields.end()) {
        mysql_->set_field(0, &info_.update_ts);
    }
    return 0;
}

tbl_test_model_info *tbl_test_model::fetch()
{
    zero_info();
    int32_t ret = mysql_->fetch();
    if (ret != 0) {
        if (ret == 1) {
            code_ = 0;
            msg_ = "";
        }
        else {
            code_ = mysql_->code();
            msg_ = mysql_->msg();
        }
        return nullptr;
    }
    return &info_;
}

int32_t tbl_test_model::count(const std::string &where)
{
    std::string sql = "select count(1) from tbl_test_model ";
    sql += where;
    if (mysql_->execute(sql)) {
        code_ = mysql_->code();
        msg_ = mysql_->msg();
        return -1;
    }
    int32_t result = 0;
    buffer_field buffer;
    buffer.buf = (char *)&result;
    buffer.size = sizeof(result);
    mysql_->set_field(0, &buffer);
    if (mysql_->fetch() != 0) {
        code_ = mysql_->code();
        msg_ = mysql_->msg();
        return -1;
    }
    mysql_->free_result();
    return result;
}

int32_t tbl_test_model::del(const std::string &where)
{
    std::string sql = "delete from tbl_test_model ";
    sql += where;
    if (mysql_->execute(sql)) {
        code_ = mysql_->code();
        msg_ = mysql_->msg();
        sql += ";\n";
        dump(sql.c_str());
        return -1;
    }
    return 0;
}

void tbl_test_model::dump(const char *sql)
{
    if (dump_file_ == "") {
        return;
    }
    int fd = open(dump_file_.c_str(), O_RDWR | O_CREAT, 0644);
    if (-1 == fd) {
        ///< doomed
        return ;
    }
    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_END);
    write(fd, sql, strlen(sql));
    flock(fd, LOCK_UN);
    close(fd);
}
std::string &tbl_test_model::insert_dump_sql(std::string &sql, const std::unordered_set<std::string> &omit_fields)
{
    char tmp[4096];
    std::string fields;
    for (auto &e : all_fields_) {
        if (omit_fields.find(e.first) == omit_fields.end()) {
            fields += e.first + ",";
        }
    }
    fields[fields.length() - 1] = ')';
    sql = "insert into tbl_test_model(" + fields + ") values (";
    if (omit_fields.find("id") == omit_fields.end()) {
        sql += std::to_string(info_.id);
        sql += ", ";
    }
    if (omit_fields.find("name") == omit_fields.end()) {
        sql += std::string("'") + info_.name.buf + std::string("'");
        sql += ", ";
    }
    if (omit_fields.find("buff") == omit_fields.end()) {
        sql += hex_format(info_.buff.buf, info_.buff.len, tmp);
        sql += ", ";
    }
    if (omit_fields.find("create_ts") == omit_fields.end()) {
        sql += std::string("'") + info_.create_ts.buf + std::string("'");
        sql += ", ";
    }
    if (omit_fields.find("update_ts") == omit_fields.end()) {
        sql += std::string("'") + info_.update_ts.buf + std::string("'");
    }
    sql += ");\n";
    return sql;
}
std::string &tbl_test_model::update_dump_sql_by_primary(std::string &sql, const std::unordered_set<std::string> &omit_fields)
{
    char tmp[4096];
    sql = "update tbl_test_model set ";
    if (omit_fields.find("id") == omit_fields.end()) {
        sql += "id = " + std::to_string(info_.id);
        sql += ", ";
    }
    if (omit_fields.find("name") == omit_fields.end()) {
        sql += "name = " + std::string("'") + info_.name.buf + std::string("'");
        sql += ", ";
    }
    if (omit_fields.find("buff") == omit_fields.end()) {
        sql += std::string("buff = ") + hex_format(info_.buff.buf, info_.buff.len, tmp);
        sql += ", ";
    }
    if (omit_fields.find("create_ts") == omit_fields.end()) {
        sql += "create_ts = " + std::string("'") + info_.create_ts.buf + std::string("'");
        sql += ", ";
    }
    if (omit_fields.find("update_ts") == omit_fields.end()) {
        sql += "update_ts = " + std::string("'") + info_.update_ts.buf + std::string("'");
    }
    sql += " where ";    sql += "id = " + std::to_string(info_.id);
    sql += ";\n";
    return sql;
}

std::string &tbl_test_model::del_dump_sql_by_primary(std::string &sql)
{
    char tmp[4096];
    sql = "delete from  tbl_test_model where ";
    sql += "id = " + std::to_string(info_.id);
    sql += ";\n";
    return sql;
}

