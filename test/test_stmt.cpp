#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include "mysqlclient.h"

static mysql_api mysql;
static std::string ip = "170.0.45.2";//"170.0.45.2";
static std::string usr = "baal";
static std::string pwd = "baal";
static std::string db = "baal_test";


struct test_api
{
    int32_t             id       ;
    char                byte     ;
    float               floating ;
    char                t[64]        ;
    char                d[64]        ;
    char                dt[64]       ;
    char                ts[64]       ;
    char                str[64]      ;
    char                bytes[4096];
    char                str_normal[64];
};

void print_err(std::string mark, mysql_stmt *stmt)
{
    std::cout << mark;
    std::cout <<  " error: [";
    std::cout << stmt->code();
    std::cout << "][";
    std::cout << stmt->msg();
    std::cout << "]" << std::endl;
}

int test_insert()
{
    test_api data = {
        1,
        1,
        3.1415926,
        "12:00:00",
        "1949-10-01",
        "1949-10-01_12:00:00.000000",
        "1949-10-01_12:00:00.000123",
        "'abcdefg'",
        {1,2,3,4,5,6,7,8,9},
        "hehehehe"
    };
    std::string sql = "insert into test_api (id,byte,floating,t,d,dt,str,bytes,normal_str) values (?,?,?,?,?,?,?,?,?)";
    mysql_stmt *stmt = mysql.prepare_stmt(sql);
    if (nullptr == stmt) {
        print_err("prepare", stmt);
        stmt->destroy();
        return -1;
    }
    stmt->set_param_long(data.id);
    stmt->set_param_tiny(data.byte);
    stmt->set_param_float(data.floating);
    buffer_field t = {data.t, sizeof(data.t), strlen(data.t)};
    stmt->set_param_time(&t);          ///< HH:MM:SS
    buffer_field d = {data.d, sizeof(data.d), strlen(data.d)};
    stmt->set_param_date(&d);          ///< YYYYMMDD HH:MM:SS
    buffer_field dt = {data.dt, sizeof(data.dt), strlen(data.dt)};
    stmt->set_param_datetime(&dt);      ///< YYYYMMDD HH:MM:SS
    buffer_field str = {data.str, sizeof(data.str), strlen(data.str)};
    stmt->set_param_string(&str);        ///< TEXT, CHAR, VARCHAR
    buffer_field bytes = {data.bytes, sizeof(data.bytes), 10};
    stmt->set_param_blob(&bytes);   ///< BLOB, BINARY, VARBINARY
    buffer_field str_normal = {data.str_normal, sizeof(data.str_normal), strlen(data.str_normal)};
    stmt->set_param_string(&str_normal);        ///< TEXT, CHAR, VARCHAR
    if (stmt->execute() != 0) {
        print_err("execute", stmt);
        stmt->destroy();
        return -1;
    }
    printf("%d succ\n", stmt->affect_rows());
    stmt->destroy();
    return 0;
}

int test_update()
{
    std::string sql = "update test_api set str = ? where id = ?";
    const char *str = "update";
    mysql_stmt *stmt = mysql.prepare_stmt(sql);
    if (nullptr == stmt) {
        print_err("prepare", stmt);
        stmt->destroy();
        return -1;
    }
    buffer_field f = {(char*)str, strlen(str), strlen(str)};
    stmt->set_param_string(&f);        ///< TEXT, CHAR, VARCHAR
    stmt->set_param_long(1);
    if (stmt->execute() != 0) {
        print_err("execute", stmt);
        stmt->destroy();
        return -1;
    }
    printf("%d succ\n", stmt->affect_rows());
    stmt->destroy();
    return 0;
}

int test_select()
{
    std::string sql = "select id,byte,floating,time_format(t,'%H%I%s'),date_format(d,'%Y%m%d'),\
date_format(dt,'%Y%m%d_%H%I%s.%f'),str,bytes,normal_str,date_format(ts,'%Y%m%d_%H%I%s.%f') from test_api";
    
    mysql_stmt *stmt = mysql.prepare_stmt(sql);
    if (nullptr == stmt) {
        print_err("prepare", stmt);
        stmt->destroy();
        return -1;
    }
    test_api data = {0};
    stmt->set_result_long(data.id);
    stmt->set_result_tiny(data.byte);
    stmt->set_result_float(data.floating);
    buffer_field t = {data.t, sizeof(data.t)};
    stmt->set_result_time(&t);          ///< HH:MM:SS
    buffer_field d = {data.d, sizeof(data.d)};
    stmt->set_result_date(&d);          ///< YYYYMMDD HH:MM:SS
    buffer_field dt = {data.dt, sizeof(data.dt)};
    stmt->set_result_datetime(&dt);      ///< YYYYMMDD HH:MM:SS
    buffer_field str = {data.str, sizeof(data.str)};
    stmt->set_result_string(&str);        ///< TEXT, CHAR, VARCHAR
    buffer_field bytes = {data.bytes, sizeof(data.bytes), 10};
    stmt->set_result_blob(&bytes);   ///< BLOB, BINARY, VARBINARY
    buffer_field str_normal = {data.str_normal, sizeof(data.str_normal)};
    stmt->set_result_string(&str_normal);        ///< TEXT, CHAR, VARCHAR
    buffer_field ts = {data.ts, sizeof(data.ts)};
    stmt->set_result_timestamp(&ts);
    if (stmt->execute() != 0) {
        print_err("execute", stmt);
        stmt->destroy();
        return -1;
    }
    int rows = 0;
    while (1) {
        int32_t ret = stmt->fetch();
        if (ret != 0) {
            print_err("fetch", stmt);
            stmt->destroy();
            break;
        }
        //std::cout << "{" << std::endl;
        //std::cout << data.id << std::endl;
        //std::cout << data.byte << std::endl;
        //std::cout << data.floating << std::endl;
        //std::cout << data.t << std::endl;
        //std::cout << data.d << std::endl;
        //std::cout << data.dt << std::endl;
        //std::cout << data.ts << std::endl;
        //std::cout << data.str << std::endl;
        //for (int i = 0; i < 10; i++) {
        //    printf("%d", data.bytes[i]);
        //}
        //std::cout << std::endl;
        //std::cout << data.str_normal << std::endl;
        //std::cout << "}" << std::endl;
        rows++;
    }
    printf("total rows: %d\n", rows);
    stmt->destroy();
    return 0;
}

int test_delete()
{
    std::string sql = "delete from test_api where id = ?";
    const char *str = "update";
    mysql_stmt *stmt = mysql.prepare_stmt(sql);
    if (nullptr == stmt) {
        print_err("prepare", stmt);
        stmt->destroy();
        return -1;
    }
    stmt->set_param_long(1);
    if (stmt->execute() != 0) {
        print_err("execute", stmt);
        stmt->destroy();
        return -1;
    }
    printf("%d succ\n", stmt->affect_rows());
    stmt->destroy();
    return 0;
}


int main()
{
    if (0 != mysql.create(ip, usr, pwd, db, 3306)) {
        return -1;
    }
    for (int i = 0;i < 10; i++) {
        if (0 != test_insert()) {
            return -1;
        }
    }
    if (0 != test_update()) {
        return -1;
    }
    if (0 != test_select()) {
        return -1;
    }
    //if (0 != test_delete()) {
    //    return -1;
    //}
    mysql.destroy();
    return 0;
}