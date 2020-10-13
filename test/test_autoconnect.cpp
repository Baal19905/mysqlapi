#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
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

int test_select(mysql_stmt *stmt)
{
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
        return -1;
    }
    int rows = 0;
    while (1) {
        int32_t ret = stmt->fetch();
        if (ret != 0) {
            //print_err("fetch", stmt);
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
    return 0;
}

int main()
{
    if (0 != mysql.create(ip, usr, pwd, db, 3306)) {
        //print_err("create");
        return -1;
    }
    std::string sql = "select id,byte,floating,time_format(t,'%H%I%s'),date_format(d,'%Y%m%d'),\
date_format(dt,'%Y%m%d_%H%I%s.%f'),str,bytes,normal_str,date_format(ts,'%Y%m%d_%H%I%s.%f') from test_api";
    
    mysql_stmt *stmt = mysql.prepare_stmt(sql);
    if (nullptr == stmt) {
        //print_err("prepare");
        return -1;
    }
    while (1) {
        test_select(stmt);
        sleep(1);
    }
    mysql.destroy();
    return 0;
}