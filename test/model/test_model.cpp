#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include "test_default.h"

static mysql_api mysql;
static std::string ip = "localhost";
static std::string usr = "root";
static std::string pwd = "mysql123";
static std::string db = "test";


int main()
{
    test_default td;
    if (0 != mysql.create(ip, usr, pwd, db, 3306)) {
        return -1;
    }
    td.set_mysql(&mysql);
    auto info = td.get_info();

    info->id = 3;

    if (td.insert({"create_date"}) != 0) {
        printf("error: [%d][%s]\n", td.code(), td.msg().c_str());
    }

    mysql.destroy();
    return 0;
}
