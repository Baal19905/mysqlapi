#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include "tbl_test_model.h"

static mysql_api mysql;
static std::string ip = "localhost";
static std::string usr = "root";
static std::string pwd = "mysql123";
static std::string db = "test";

void print(tbl_test_model_info *info);
void test_all_fields_insert(tbl_test_model &t);
void test_omit_insert(tbl_test_model &t);
void test_all_fields_select(tbl_test_model &t);
void test_omit_select(tbl_test_model &t);
void test_all_fields_update(tbl_test_model &t);
void test_omit_update(tbl_test_model &t);
void test_all_fields_select_by_index(tbl_test_model &t);
void test_omit_select_by_index(tbl_test_model &t);

int main()
{
    tbl_test_model t;
    if (0 != mysql.create(ip, usr, pwd, db, 3306)) {
        return -1;
    }
    t.set_mysql(&mysql);
    t.set_dump_path(".");
    //test_all_fields_insert(t);
    //test_omit_insert(t);
    //test_all_fields_update(t);
    test_omit_update(t);
    test_all_fields_select(t);
    //test_omit_select(t);
    //test_all_fields_select_by_index(t);
    //test_omit_select_by_index(t);
    mysql.destroy();
    return 0;
}

void print(tbl_test_model_info *info)
{
    char buff[4096] = {0};
    for (int i = 0; i < info->buff.len; i++) {
        sprintf(buff, "%s0x%X", buff, (int)(info->buff.buf[i]));
    }
    std::cout << "{" << std::endl;
    std::cout << "    id=[" << info->id << "]" << std::endl;
    std::cout << "    name=[" << info->name.buf << "]" << std::endl;
    std::cout << "    buff=[" << buff << "]" << std::endl;
    std::cout << "    create_ts=[" << info->create_ts.buf << "]" << std::endl;
    std::cout << "    update_ts=[" << info->update_ts.buf << "]" << std::endl;
    std::cout << "}" << std::endl;
}

void test_all_fields_insert(tbl_test_model &t)
{
    auto info = t.get_info();
    for (int i = 0; i < 10; i++) {
        info->id = 1 + i;
        strcpy(info->name.buf, "test");
        info->name.len = strlen(info->name.buf);
        char buf[] = {
            0,1,2,3,4,5,6,7,8,9
        };
        info->buff.len = sizeof(buf);
        memcpy(info->buff.buf, buf, info->buff.len);
        strcpy(info->create_ts.buf, "2020-12-20_13:47:00.123456");
        info->create_ts.len = strlen(info->create_ts.buf);
        strcpy(info->update_ts.buf, "2020-12-20_13:47:00.123456");
        info->update_ts.len = strlen(info->update_ts.buf);
        if (t.insert() != 0) {
            std::cout << "error: [" << t.code() << "][" << t.msg() << std::endl;
        }
    }
}

void test_omit_insert(tbl_test_model &t)
{
    auto info = t.get_info();
    for (int i = 0; i < 10; i++) {
        strcpy(info->name.buf, "test");
        info->name.len = strlen(info->name.buf);
        strcpy(info->create_ts.buf, "2020-12-20_13:47:00.123456");
        info->create_ts.len = strlen(info->create_ts.buf);
        if (t.insert({"id", "buff", "update_ts"}) != 0) {
            std::cout << "error: [" << t.code() << "][" << t.msg() << std::endl;
        }
    }
}

void test_all_fields_select(tbl_test_model &t)
{
    auto info = t.get_info();
    std::string where;
    for (int i = 0; i < 20; i++) {
        where = "where id=" + std::to_string(i + 1);
        if (t.select(where) != 0) {
            std::cout << "error: [" << t.code() << "][" << t.msg() << std::endl;
            return ;
        }
        while (t.fetch() != nullptr) {  // 必须取完所有数据，否则后续sql可能报错
            print(info);
        }
    }
}

void test_omit_select(tbl_test_model &t)
{
    auto info = t.get_info();
    std::string where;
    for (int i = 0; i < 20; i++) {
        where = "where id=" + std::to_string(i + 1);
        if (t.select(where, {"buff", "create_ts"}) != 0) {
            std::cout << "error: [" << t.code() << "][" << t.msg() << std::endl;
            return ;
        }
        while (t.fetch() != nullptr) {  // 必须取完所有数据，否则后续sql可能报错
            print(info);
        }
    }
}

void test_all_fields_update(tbl_test_model &t)
{
    auto info = t.get_info();
    info->id = 1;
    strcpy(info->name.buf, "test_up");
    info->name.len = strlen(info->name.buf);
    char buf[] = {
        9,8,7,6,5,4,3,2,1,0
    };
    info->buff.len = sizeof(buf);
    memcpy(info->buff.buf, buf, info->buff.len);
    strcpy(info->create_ts.buf, "2020-12-19_16:48:00.123456");
    info->create_ts.len = strlen(info->create_ts.buf);
    strcpy(info->update_ts.buf, "2020-12-19_16:48:00.123456");
    info->update_ts.len = strlen(info->update_ts.buf);
    if (t.update_by_primary(1) != 0) {
        std::cout << "error: [" << t.code() << "][" << t.msg() << std::endl;
    }
}

void test_omit_update(tbl_test_model &t)
{
    auto info = t.get_info();
    info->id = 20;
    strcpy(info->name.buf, "test_upoo");
    info->name.len = strlen(info->name.buf);
    char buf[] = {
        9,11,7,6,5,4,3,2,1,0
    };
    info->buff.len = sizeof(buf);
    memcpy(info->buff.buf, buf, info->buff.len);
    strcpy(info->create_ts.buf, "1998-12-20_16:48:00.123456");
    info->create_ts.len = strlen(info->create_ts.buf);
    strcpy(info->update_ts.buf, "1999-12-20_16:48:00.123456");
    info->update_ts.len = strlen(info->update_ts.buf);
    if (t.update_by_primary(20, {"id", "buff", "update_ts"}) != 0) {
        std::cout << "error: [" << t.code() << "][" << t.msg() << std::endl;
    }
}

void test_all_fields_select_by_index(tbl_test_model &t)
{
    auto info = t.get_info();
    if (t.select_by_primary(8) != 0) {
        std::cout << "error: [" << t.code() << "][" << t.msg() << std::endl;
        return ;
    }
    while (t.fetch_by_primary() != nullptr) {  // 必须取完所有数据，否则后续sql可能报错
        print(info);
    }
}

void test_omit_select_by_index(tbl_test_model &t)
{
    auto info = t.get_info();
    if (t.select_by_primary(8, {"buff"}) != 0) {
        std::cout << "error: [" << t.code() << "][" << t.msg() << std::endl;
        return ;
    }
    while (t.fetch_by_primary() != nullptr) {  // 必须取完所有数据，否则后续sql可能报错
        print(info);
    }
}