#include "mysqlclient.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

int32_t mysql_api::create(const std::string &ip, const std::string &usr,
                    const std::string &pwd, const std::string &db, uint16_t port)
{
    hdl_ = mysql_init(nullptr);
    if (nullptr == hdl_) {
        return -1;
    }
    mysql_options(hdl_, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    my_bool reconnect = true;
    mysql_options(hdl_, MYSQL_OPT_RECONNECT, (const char *)&reconnect);
    unsigned long client_flag = CLIENT_MULTI_STATEMENTS | CLIENT_REMEMBER_OPTIONS;
    if (nullptr == mysql_real_connect(hdl_, ip.c_str(), usr.c_str(), pwd.c_str(), db.c_str(), port, nullptr, client_flag)) {
        return -1;
    }
    return result_set_.create();
}

void mysql_api::destroy()
{
    for (auto &e : stmts_) {
        e->destroy();
        delete e;
    }
    stmts_.clear();
    result_set_.destroy();
    if (nullptr != hdl_) {
        mysql_close(hdl_);
    }
}

mysql_stmt *mysql_api::prepare_stmt(const std::string &sql)
{
    mysql_stmt *stmt = new mysql_stmt;
    if (-1 == stmt->create(hdl_, sql)) {
        return nullptr;
    }
    stmts_.push_back(stmt);
    return stmt;
}

bool mysql_api::set_commitmode(bool mode)
{
    ///< Zero for success. Nonzero if an error occurred. 
    return !(bool)mysql_autocommit(hdl_, (my_bool)mode);
}

int32_t mysql_api::execute(const std::string &sql)
{
    result_set_.rewind();
    if (0 != mysql_real_query(hdl_, sql.c_str(), sql.length())) {
        return -1;
    }
    rows_ = mysql_affected_rows(hdl_);
    return 0;
}

int32_t mysql_api::fetch()
{
FETCH:
    if (nullptr == res_) {
        res_ = mysql_store_result(hdl_);
        if (nullptr == res_) {
            ///< multi statment
            int64_t ret = mysql_next_result(hdl_);
            if (ret == 0) {
                goto FETCH;
            }
            return -1;
        }
        if (-1 == result_set_.next(current_set_)) {
            return -1;
        }
    }
    MYSQL_ROW row = mysql_fetch_row(res_);
    if (nullptr == row) {
        ///< 当前结果集没有数据了
        mysql_free_result(res_);
        res_ = nullptr;
        return 1;
    }
    unsigned long *lengths = mysql_fetch_lengths(res_);
    MYSQL_FIELD *fields = mysql_fetch_fields(res_);
    for (uint32_t i = 0; i < (uint32_t)mysql_num_fields(res_); i++) {
        if (row[i] == nullptr) {
            continue;
        }
        if (IS_NUM(fields[i].type)) {
            if (fields[i].type == MYSQL_TYPE_TINY ||
                fields[i].type == MYSQL_TYPE_SHORT ||
                fields[i].type == MYSQL_TYPE_LONG ||
                fields[i].type == MYSQL_TYPE_INT24 ||
                fields[i].type == MYSQL_TYPE_LONGLONG || 
                fields[i].type == MYSQL_TYPE_ENUM ||
                fields[i].type == MYSQL_TYPE_BIT) {
                auto n = atoll(row[i]);
                memcpy(current_set_[i]->buf, &n, current_set_[i]->size);
            }
            else if (fields[i].type == MYSQL_TYPE_DECIMAL ||
                     fields[i].type == MYSQL_TYPE_NEWDECIMAL ||
                     fields[i].type == MYSQL_TYPE_FLOAT ||
                     fields[i].type == MYSQL_TYPE_DOUBLE) {
                if (current_set_[i]->size == sizeof(double)) {
                    auto d = atof(row[i]);
                    memcpy(current_set_[i]->buf, &d, current_set_[i]->size);
                }
                else {
                    float f = atof(row[i]);
                    memcpy(current_set_[i]->buf, &f, current_set_[i]->size);
                }
            }
        }
        else {
            current_set_[i]->len = lengths[i];
            memcpy(current_set_[i]->buf, row[i], current_set_[i]->len);
            current_set_[i]->buf[current_set_[i]->len] = 0;///< 防止字符串没有结束符
        }
    }
    return 0;
}

int32_t mysql_api::set_field(uint32_t set_pos, buffer_field *field)
{
    if (nullptr == field) {
        return -1;
    }
    return result_set_.set_field(set_pos, field);
}

int32_t mysql_api::commit()
{
    return mysql_commit(hdl_);
}

int32_t mysql_api::rollback()
{
    return mysql_rollback(hdl_);
}

uint32_t mysql_api::code()
{
    if (nullptr == hdl_) {
        return 0;
    }
    return (uint32_t)mysql_errno(hdl_);
}

const char *mysql_api::msg()
{
    if (nullptr == hdl_) {
        return 0;
    }
    return mysql_error(hdl_);
}

int32_t mysql_stmt::create(MYSQL *hdl, const std::string &sql)
{
    if (nullptr == hdl) {
        return -1;
    }
    hdl_ = hdl;
    current_thread_id_ = mysql_thread_id(hdl_);
    stmt_ = mysql_stmt_init(hdl);
    if (nullptr == stmt_) {
        return -1;
    }
    sql_ = sql;
    if (0 != mysql_stmt_prepare(stmt_, sql.c_str(), sql.length())) {
        return -1;
    }
    uint32_t param_count = mysql_stmt_param_count(stmt_);
    uint32_t result_count = mysql_stmt_field_count(stmt_);
    if (param_count > 0) {
        if (-1 == bind_param_.create(param_count)) {
            return -1;
        }
    }
    if (result_count > 0) {
        if (-1 == bind_result_.create(result_count)) {
            return -1;
        }
    }
    return 0;
}

void mysql_stmt::destroy()
{
    if (nullptr != stmt_) {
        mysql_stmt_close(stmt_);
        stmt_ = nullptr;
    }
    bind_param_.destroy();
    bind_result_.destroy();
}

void mysql_stmt::set_param_tiny(const char &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    bind_param->is_unsigned = false;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_TINY;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_tiny(const unsigned char &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    bind_param->is_unsigned = true;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_TINY;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_short(const int16_t &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    bind_param->is_unsigned = false;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_SHORT;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_short(const uint16_t &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    bind_param->is_unsigned = true;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_SHORT;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_long(const int32_t &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    bind_param->is_unsigned = false;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_LONG;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_long(const uint32_t &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    bind_param->is_unsigned = true;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_LONG;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_longlong(const int64_t &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    bind_param->is_unsigned = false;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_LONGLONG;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_longlong(const uint64_t &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    bind_param->is_unsigned = true;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_LONGLONG;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_float(const float &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_FLOAT;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_double(const double &param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_DOUBLE;
        bind_param->buffer = (void*)&param;
        bind_param->buffer_length = sizeof(param);
    }
}

void mysql_stmt::set_param_time(buffer_field *param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    uint32_t pos = bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    MYSQL_TIME *time = bind_param_.time_by_pos(pos);
    *(bind_param->is_null) = isnull;
    if (!isnull) {
        util::convert_to_mysql_time(param->buf, param->len, time);
        time_bridge_.push_back(std::make_pair(param, time));
        bind_param->buffer_type = MYSQL_TYPE_TIME;
        bind_param->buffer = time;
        bind_param->buffer_length = sizeof(MYSQL_TIME);
    }
}

void mysql_stmt::set_param_date(buffer_field *param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    uint32_t pos = bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    MYSQL_TIME *time = bind_param_.time_by_pos(pos);
    *(bind_param->is_null) = isnull;
    if (!isnull) {
        util::convert_to_mysql_time(param->buf, param->len, time);
        time_bridge_.push_back(std::make_pair(param, time));
        bind_param->buffer_type = MYSQL_TYPE_DATE;
        bind_param->buffer = time;
        bind_param->buffer_length = sizeof(MYSQL_TIME);
    }
}

void mysql_stmt::set_param_datetime(buffer_field *param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    uint32_t pos = bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    MYSQL_TIME *time = bind_param_.time_by_pos(pos);
    *(bind_param->is_null) = isnull;
    if (!isnull) {
        util::convert_to_mysql_time(param->buf, param->len, time);
        time_bridge_.push_back(std::make_pair(param, time));
        bind_param->buffer_type = MYSQL_TYPE_DATETIME;
        bind_param->buffer = time;
        bind_param->buffer_length = sizeof(MYSQL_TIME);
    }
}

void mysql_stmt::set_param_timestamp(buffer_field *param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    uint32_t pos = bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    MYSQL_TIME *time = bind_param_.time_by_pos(pos);
    *(bind_param->is_null) = isnull;
    if (!isnull) {
        util::convert_to_mysql_time(param->buf, param->len, time);
        time_bridge_.push_back(std::make_pair(param, time));
        bind_param->buffer_type = MYSQL_TYPE_TIMESTAMP;
        bind_param->buffer = time;
        bind_param->buffer_length = sizeof(MYSQL_TIME);
    }
}

///< TEXT, CHAR, VARCHAR
void mysql_stmt::set_param_string(buffer_field *param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_STRING;
        bind_param->buffer = (void*)(param->buf);
        bind_param->buffer_length = param->len;
        bind_param->length = &(param->len);
        buf_bridge_.push_back(std::make_pair(param, bind_param));
    }
}

///< BLOB, BINARY, VARBINARY
void mysql_stmt::set_param_blob(buffer_field *param, bool isnull)
{
    MYSQL_BIND *bind_param = nullptr;
    bind_param_.next(bind_param);
    if (nullptr == bind_param) {
        return;
    }
    *(bind_param->is_null) = isnull;
    if (!isnull) {
        bind_param->buffer_type = MYSQL_TYPE_BLOB;
        bind_param->buffer = (void*)(param->buf);
        bind_param->buffer_length = param->len;
        bind_param->length = &(param->len);
        buf_bridge_.push_back(std::make_pair(param, bind_param));
    }
}

void mysql_stmt::set_result_tiny(char &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->is_unsigned = false;
    bind_result->buffer_type = MYSQL_TYPE_TINY;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_tiny(unsigned char &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->is_unsigned = true;
    bind_result->buffer_type = MYSQL_TYPE_TINY;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_short(int16_t &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->is_unsigned = false;
    bind_result->buffer_type = MYSQL_TYPE_SHORT;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_short(uint16_t &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->is_unsigned = true;
    bind_result->buffer_type = MYSQL_TYPE_SHORT;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_long(int32_t &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->is_unsigned = false;
    bind_result->buffer_type = MYSQL_TYPE_LONG;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_long(uint32_t &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->is_unsigned = true;
    bind_result->buffer_type = MYSQL_TYPE_LONG;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_longlong(int64_t &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->is_unsigned = false;
    bind_result->buffer_type = MYSQL_TYPE_LONGLONG;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_longlong(uint64_t &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->is_unsigned = true;
    bind_result->buffer_type = MYSQL_TYPE_LONGLONG;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_float(float &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->buffer_type = MYSQL_TYPE_FLOAT;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_double(double &param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->buffer_type = MYSQL_TYPE_DOUBLE;
    bind_result->buffer = &param;
    bind_result->buffer_length = sizeof(param);
}

void mysql_stmt::set_result_time(buffer_field *param)
{
    set_result_string(param);
}

void mysql_stmt::set_result_date(buffer_field *param)
{
    set_result_string(param);
}

void mysql_stmt::set_result_datetime(buffer_field *param)
{
    set_result_string(param);
}

void mysql_stmt::set_result_timestamp(buffer_field *param)
{
    set_result_string(param);
}

///< TEXT, CHAR, VARCHAR
void mysql_stmt::set_result_string(buffer_field *param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->buffer_type = MYSQL_TYPE_STRING;
    bind_result->buffer = param->buf;
    bind_result->buffer_length = param->size;
    bind_result->length = &param->len;
}

///< BLOB, BINARY, VARBINARY
void mysql_stmt::set_result_blob(buffer_field *param)
{
    MYSQL_BIND *bind_result = nullptr;
    bind_result_.next(bind_result);
    if (nullptr == bind_result) {
        return;
    }
    bind_result->buffer_type = MYSQL_TYPE_BLOB;
    bind_result->buffer = param->buf;
    bind_result->buffer_length = param->size;
    bind_result->length = &param->len;
}

int32_t mysql_stmt::execute()
{
RECOVER:
    if (server_gone_) {
        if (recover_stmt() != 0) {
            return -1;
        }
        server_gone_ = false;
    }
    if (bind_param_.pos() > 0) {
        for (auto &e : time_bridge_) {
            util::convert_to_mysql_time(e.first->buf, e.first->len, e.second);
        }
        for (auto &e : buf_bridge_) {
            e.second->buffer = (void*)(e.first->buf);
            e.second->buffer_length = e.first->len;
            e.second->length = &(e.first->len);
        }
        mysql_stmt_bind_param(stmt_, bind_param_.head());
    }
    if (bind_result_.pos() > 0) {
        mysql_stmt_bind_result(stmt_, bind_result_.head());
    }
    int32_t ret = mysql_stmt_execute(stmt_);
    if (ret != 0) {
        if (2006 == code() || 2013 == code() || 2003 == code() || 1243 == code()) {
            ///< 已经自动重连一次，仍然失败或者高可用切换，记录状态，下次调用execute时再次出发重连
            server_gone_ = true;
            goto RECOVER;
        }
        return -1;
    }
    mysql_stmt_store_result(stmt_);
    rows_ = mysql_stmt_affected_rows(stmt_);
    return ret;
}

int32_t mysql_stmt::fetch()
{
    int status = mysql_stmt_fetch(stmt_);
    if (status == 1 || status == MYSQL_NO_DATA) {
        mysql_stmt_free_result(stmt_);
        return -1;
    }
    return 0;
}

uint32_t mysql_stmt::code()
{
    if (nullptr == stmt_) {
        return 0;
    }
    return (uint32_t)mysql_stmt_errno(stmt_);
}

const char *mysql_stmt::msg()
{
    if (nullptr == stmt_) {
        return 0;
    }
    return mysql_stmt_error(stmt_);
}

int32_t mysql_stmt::recover_stmt()
{
    mysql_ping(hdl_);
    if (current_thread_id_ != mysql_thread_id(hdl_)) {
        mysql_kill(hdl_, current_thread_id_);
    }
    mysql_stmt_close(stmt_);
    current_thread_id_ = mysql_thread_id(hdl_);
    stmt_ = mysql_stmt_init(hdl_);
    if (nullptr == stmt_) {
        return -1;
    }
    if (0 != mysql_stmt_prepare(stmt_, sql_.c_str(), sql_.length())) {
        return -1;
    }
    return 0;
}