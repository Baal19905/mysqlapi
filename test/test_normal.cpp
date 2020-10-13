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

struct test_normal
{
    int32_t id;
    char name[256];
    char ts[64];
    int64_t bi;
    float f;
    double d;
};

static std::string sql = "truncate table test_normal;\
insert into test_normal (id, name, bi, f, d) values (-1, 'name1', 99999, 3.14, 6.28), (2, 'name2', 99999, 3.14, 6.28), (3, 'name3', 99999, 3.14, 6.28), (4, 'name4', 99999, 3.14, 6.28), (5, 'name5', 99999, 3.14, 6.28);\
select id, name, ts, bi, f, d from test_normal;\
select id, ts from test_ts;";

void print_err(std::string mark)
{
    std::cout << mark;
    std::cout <<  " error: [";
    std::cout << mysql.code();
    std::cout << "][";
    std::cout << mysql.msg();
    std::cout << "]" << std::endl;
}

int main()
{
    if (0 != mysql.create(ip, usr, pwd, db, 3306)) {
        print_err("create");
        return -1;
    }
    if (mysql.execute(sql)) {
        print_err("execute");
        return -1;
    }
    test_normal tn;
    buffer_field fields[] = {
        {(char*)&tn.id, sizeof(tn.id)},
        {tn.name, sizeof(tn.name)},
        {tn.ts, sizeof(tn.ts)},
        {(char*)&tn.bi, sizeof(tn.bi)},
        {(char*)&tn.f, sizeof(tn.f)},
        {(char*)&tn.d, sizeof(tn.d)}
    };
    mysql.set_field(0, &fields[0]);
    mysql.set_field(0, &fields[1]);
    mysql.set_field(0, &fields[2]);
    mysql.set_field(0, &fields[3]);
    mysql.set_field(0, &fields[4]);
    mysql.set_field(0, &fields[5]);
    mysql.set_field(1, &fields[0]);
    mysql.set_field(1, &fields[2]);
    while (1) {
        int32_t ret = mysql.fetch();
        if (ret == 1) {
            std::cout << "next result set" << std::endl;
            continue;
        }
        else if (ret == -1)
        {
            print_err("fetch");
            break;
        }
        std::cout << "{" << std::endl;
        std::cout << "    " << tn.id << std::endl;
        std::cout << "    " << tn.name << std::endl;
        std::cout << "    " << tn.ts << std::endl;
        std::cout << "    " << tn.bi << std::endl;
        std::cout << "    " << tn.f << std::endl;
        std::cout << "    " << tn.d << std::endl;
        std::cout << "}" << std::endl;
    }
    
    mysql.destroy();
    return 0;
}