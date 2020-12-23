#include "mysqlclient.h"
#include "struct.h"
#include "utils.h"

#define STRUCT_TAIL     "_info"

void print_err(string mark);
int cache_table_info();
void gen_struct(ofstream &file, table_info &info);
void gen_init_all_fields(ofstream &file, table_info &info);
void gen_head(table_info &info);
void gen_src(table_info &info);
void gen_defination(ofstream &file, table_info &info);
void gen_index_func_name(ofstream &file, index_info &e, string func_head, string name_space = "", bool bsrc = false);
void gen_db_unrelated_reference(ofstream &file, table_info &info);
void gen_insert_reference(ofstream &file, table_info &info);
void gen_indexed_select_reference(ofstream &file, table_info &info);
void gen_indexed_fetch_reference(ofstream &file, table_info &info);
void gen_indexed_update_reference(ofstream &file, table_info &info);
void gen_indexed_delete_reference(ofstream &file, table_info &info);
void gen_select_reference(ofstream &file, table_info &info);
void gen_fetch_reference(ofstream &file, table_info &info);
void gen_count_reference(ofstream &file, table_info &info);
void gen_del_reference(ofstream &file, table_info &info);
void gen_dump_reference(ofstream &file, table_info &info);
void gen_insert_dump_sql_reference(ofstream &file, table_info &info);
void gen_update_dump_sql_reference(ofstream &file, table_info &info);
void gen_del_dump_sql_reference(ofstream &file, table_info &info);


static mysql_api mysql;
static map<string, table_info> tables;
static string prefix = "./";

int main(int argc, char **argv)
{
    if (argc < 5) {
        cout << argv[0]  << "<ip> <usr> <pwd> <db> <prefix>" << endl;
        return -1;
    }
    string ip = argv[1];
    string usr = argv[2];
    string pwd = argv[3];
    string db = argv[4];
    if (argc > 5) {
        prefix = argv[5];
    }
    if (0 != mysql.create(ip, usr, pwd, db, 3306)) {
        cout << "create failed!!! [" << mysql.code() << "][" << mysql.msg() << "]" << endl;
        return -1;
    }
    if (cache_table_info() != 0) {
        cout << "cache table info failed!!!" << endl;
        return -1;
    }
    for (auto &e : tables) {
        gen_head(e.second);
        cout << "gen " << e.first << ".h success" << endl;
        gen_src(e.second);
        cout << "gen " << e.first << ".cpp success" << endl;
    }
    mysql.destroy();
    return 0;
}

int cache_table_info()
{
    char buf[13][256] = {0};
    buffer_field fields[13] = {
        {buf[0], sizeof(buf[0])},
        {buf[1], sizeof(buf[1])},
        {buf[2], sizeof(buf[2])},
        {buf[3], sizeof(buf[3])},
        {buf[4], sizeof(buf[4])},
        {buf[5], sizeof(buf[5])},
        {buf[6], sizeof(buf[6])},
        {buf[7], sizeof(buf[7])},
        {buf[8], sizeof(buf[8])},
        {buf[9], sizeof(buf[9])},
        {buf[10], sizeof(buf[10])},
        {buf[11], sizeof(buf[11])},
        {buf[12], sizeof(buf[12])}
    };
    string sql = "show tables";
    if (mysql.execute(sql)) {
        print_err("execute");
        return -1;
    }
    mysql.set_field(0, &fields[0]);
    while (1) {
        memset(buf, 0, sizeof(buf));
        if (mysql.fetch() != 0) {
            break;
        }
        table_info ti;
        ti.name = fields[0].buf;
        tables.insert({ti.name, ti});
        
    }
    ///< 表字段信息
    for (auto &e : tables) {
        sql = "describe " + e.first;
        if (mysql.execute(sql)) {
            print_err("execute");
            return -1;
        }
        mysql.set_field(0, &fields[0]);
        mysql.set_field(0, &fields[1]);
        mysql.set_field(0, &fields[2]);
        mysql.set_field(0, &fields[3]);
        mysql.set_field(0, &fields[4]);
        mysql.set_field(0, &fields[5]);
        char size[128] = {0};
        while (1) {
            memset(buf, 0, sizeof(buf));
            if (mysql.fetch() != 0) {
                break;
            }
            string tmp = fields[0].buf;
            utils::lower(tmp);
            field_info field = {
                utils::type_convert_db_to_c(fields[1].buf),
                tmp,
                utils::get_size(fields[1].buf, size),
                utils::get_db_type(fields[1].buf)
            };
            e.second.fields.push_back(field);
        }
    }
    ///< 表索引信息
    for (auto &e : tables) {
        sql = "show index from  " + e.first;
        if (mysql.execute(sql)) {
            print_err("execute");
            return -1;
        }
        mysql.set_field(0, &fields[0]);
        mysql.set_field(0, &fields[1]);
        mysql.set_field(0, &fields[2]);
        mysql.set_field(0, &fields[3]);
        mysql.set_field(0, &fields[4]);
        mysql.set_field(0, &fields[5]);
        mysql.set_field(0, &fields[6]);
        mysql.set_field(0, &fields[7]);
        mysql.set_field(0, &fields[8]);
        mysql.set_field(0, &fields[9]);
        mysql.set_field(0, &fields[10]);
        mysql.set_field(0, &fields[11]);
        mysql.set_field(0, &fields[12]);
        index_info index;
        while (1) {
            memset(buf, 0, sizeof(buf));
            if (mysql.fetch() != 0) {
                break;
            }
            if (strcmp(index.name.c_str(), fields[2].buf) != 0) {
                index.name = fields[2].buf;
                index.uniq = !atoi(fields[1].buf);
                e.second.indexes.push_back(index);
            }
            for (auto &f : e.second.fields) {
                string tmp = fields[4].buf;
                utils::lower(tmp);
                if (f.name == tmp) {
                    field_info tmp = f;
                    if (tmp.type == "buffer_field") {
                        tmp.type = "char *";
                    }
                    e.second.indexes[e.second.indexes.size() - 1].column.push_back(tmp);
                    break;
                }
            }
        }
    }
    return 0;
}

void gen_head(table_info &info)
{
    string path = prefix + "/" + utils::lower(info.name) + ".h";
    ofstream file(path, ofstream::out | ofstream::trunc);
    file << "#ifndef _" << utils::upper(info.name) << "_H_" << endl;
    file << "#define _" << utils::upper(info.name) << "_H_" << endl;
    file << "#include \"mysqlclient.h\"" << endl;
    file << "#include <stdint.h>" << endl;
    file << "#include <vector>" << endl;
    file << "#include <string>" << endl;
    file << "#include <sys/types.h>" << endl;
    file << "#include <sys/stat.h>" << endl;
    file << "#include <unordered_set>" << endl;
    file << "#include <unordered_map>" << endl;
    file << endl;
    gen_struct(file, info);
    file << "class " << utils::lower(info.name) << endl;
    file << "{" << endl;
    gen_defination(file, info);
    file << "};" << endl;
    file << "#endif ///< _" << utils::upper(info.name) << "_H_" << endl;
    file.close();
}

void gen_src(table_info &info)
{
    string path = prefix + "/" + utils::lower(info.name) + ".cpp";
    ofstream file(path, ofstream::out | ofstream::trunc);
    file << "#include \""<< utils::lower(info.name) << ".h\"" << endl;
    file << "#include <string.h>" << endl;
    file << "#include <sys/file.h>" << endl;
    file << "#include <fcntl.h>" << endl;
    file << "#include <unistd.h>" << endl;
    file << endl;
    gen_db_unrelated_reference(file, info);
    gen_insert_reference(file, info);
    gen_indexed_select_reference(file, info);
    gen_indexed_fetch_reference(file, info);
    gen_indexed_update_reference(file, info);
    gen_indexed_delete_reference(file, info);
    gen_select_reference(file, info);
    gen_fetch_reference(file, info);
    gen_count_reference(file, info);
    gen_del_reference(file, info);
    gen_dump_reference(file, info);
    gen_insert_dump_sql_reference(file, info);
    gen_update_dump_sql_reference(file, info);
    gen_del_dump_sql_reference(file, info);
    file.close();
}

void gen_insert_dump_sql_reference(ofstream &file, table_info &info)
{
    file << "std::string &" << info.name << "::insert_dump_sql(std::string &sql, const std::unordered_set<std::string> &omit_fields)" << endl;
    file << "{" << endl;
    file << "    char tmp[4096];" << endl;
    file << "    std::string fields;" << endl;
    file << "    for (auto &e : all_fields_) {" << endl;
    file << "        if (omit_fields.find(e) == omit_fields.end()) {" << endl;
    file << "            fields += e + \",\";" << endl;
    file << "        }" << endl;
    file << "    }" << endl;
    file << "    fields[fields.length() - 1] = ')';" << endl;
    file << "    sql = \"insert into " << info.name << "(\" + fields + \") values (\";" << endl;
    for (int i = 0; i < info.fields.size(); i++) {
        file << "    if (omit_fields.find(\"" << info.fields[i].name << "\") == omit_fields.end()) {" << endl;
        if (info.fields[i].db_type == "tinyint") {
            file << "        sql += std::to_string((int)info_." << info.fields[i].name << ");" << endl;
        }
        else if (info.fields[i].db_type == "smallint"||
                 info.fields[i].db_type == "mediumint" ||
                 info.fields[i].db_type == "int" ||
                 info.fields[i].db_type == "integer" ||
                 info.fields[i].db_type == "bigint" ||
                 info.fields[i].db_type == "float" ||
                 info.fields[i].db_type == "double") {
            file << "        sql += std::to_string(info_." << info.fields[i].name << ");" << endl;
        }
        else if (info.fields[i].db_type == "char" ||
                 info.fields[i].db_type == "varchar" ||
                 info.fields[i].db_type == "tinytext" ||
                 info.fields[i].db_type == "text" ||
                 info.fields[i].db_type == "mediumtext" ||
                 info.fields[i].db_type == "longtext" ||
                 info.fields[i].db_type == "date" ||
                 info.fields[i].db_type == "time" ||
                 info.fields[i].db_type == "datetime" ||
                 info.fields[i].db_type == "timestamp") {
            file << "        sql += std::string(\"'\") + info_." << info.fields[i].name << ".buf + std::string(\"'\");" << endl;
        }
        else if (info.fields[i].db_type == "tinyblob" ||
                    info.fields[i].db_type == "blob" ||
                    info.fields[i].db_type == "mediumblob" ||
                    info.fields[i].db_type == "longblob" ||
                    info.fields[i].db_type == "binary" ||
                    info.fields[i].db_type == "varbinary") {
            file << "        sql += hex_format(info_." << info.fields[i].name << ".buf, info_." << info.fields[i].name << ".len, tmp);" << endl;
        }
        if (i < info.fields.size() - 1) {
            file << "        sql += \", \";" << endl;
        }
        file << "    }" << endl;
    }
    file << "    sql += \");\\n\";" << endl;
    file << "    return sql;" << endl;
    file << "}" << endl;
}

void gen_update_dump_sql_reference(ofstream &file, table_info &info)
{
    for (auto &e : info.indexes) {
        file << "std::string &" << info.name << "::update_dump_sql_by_" << e.name << "(std::string &sql, const std::unordered_set<std::string> &omit_fields)" << endl;
        file << "{" << endl;
        file << "    char tmp[4096];" << endl;
        file << "    sql = \"update " << info.name << " set \";" << endl;
        for (int i = 0; i < info.fields.size(); i++) {
            file << "    if (omit_fields.find(\"" << info.fields[i].name << "\") == omit_fields.end()) {" << endl;
            if (info.fields[i].db_type == "tinyint") {
                file << "        sql += \"" << info.fields[i].name << " = \" + std::to_string((int)info_." << info.fields[i].name << ");" << endl;
            }
            else if (info.fields[i].db_type == "smallint"||
                    info.fields[i].db_type == "mediumint" ||
                    info.fields[i].db_type == "int" ||
                    info.fields[i].db_type == "integer" ||
                    info.fields[i].db_type == "bigint" ||
                    info.fields[i].db_type == "float" ||
                    info.fields[i].db_type == "double") {
                file << "        sql += \"" << info.fields[i].name << " = \" + std::to_string(info_." << info.fields[i].name << ");" << endl;
            }
            else if (info.fields[i].db_type == "char" ||
                    info.fields[i].db_type == "varchar" ||
                    info.fields[i].db_type == "tinytext" ||
                    info.fields[i].db_type == "text" ||
                    info.fields[i].db_type == "mediumtext" ||
                    info.fields[i].db_type == "longtext" ||
                    info.fields[i].db_type == "date" ||
                    info.fields[i].db_type == "time" ||
                    info.fields[i].db_type == "datetime" ||
                    info.fields[i].db_type == "timestamp") {
                file << "        sql += \"" << info.fields[i].name << " = \" + std::string(\"'\") + info_." << info.fields[i].name << ".buf + std::string(\"'\");" << endl;
            }
            else if (info.fields[i].db_type == "tinyblob" ||
                        info.fields[i].db_type == "blob" ||
                        info.fields[i].db_type == "mediumblob" ||
                        info.fields[i].db_type == "longblob" ||
                        info.fields[i].db_type == "binary" ||
                        info.fields[i].db_type == "varbinary") {
                file << "        sql += std::string(\"" << info.fields[i].name << " = \") + hex_format(info_." << info.fields[i].name << ".buf, info_." << info.fields[i].name << ".len, tmp);" << endl;
            }
            if (i < info.fields.size() - 1) {
                file << "        sql += \", \";" << endl;
            }file << "    }" << endl;
        }
        file << "    sql += \" where \";";
        for (int i = 0; i < e.column.size(); i++) {
            if (e.column[i].db_type == "tinyint") {
                file << "    sql += \"" << e.column[i].name << " = \" + std::to_string((int)info_." << e.column[i].name << ");" << endl;
            }
            else if (e.column[i].db_type == "smallint"||
                    e.column[i].db_type == "mediumint" ||
                    e.column[i].db_type == "int" ||
                    e.column[i].db_type == "integer" ||
                    e.column[i].db_type == "bigint" ||
                    e.column[i].db_type == "float" ||
                    e.column[i].db_type == "double") {
                file << "    sql += \"" << e.column[i].name << " = \" + std::to_string(info_." << e.column[i].name << ");" << endl;
            }
            else if (e.column[i].db_type == "char" ||
                    e.column[i].db_type == "varchar" ||
                    e.column[i].db_type == "tinytext" ||
                    e.column[i].db_type == "text" ||
                    e.column[i].db_type == "mediumtext" ||
                    e.column[i].db_type == "longtext" ||
                    e.column[i].db_type == "date" ||
                    e.column[i].db_type == "time" ||
                    e.column[i].db_type == "datetime" ||
                    e.column[i].db_type == "timestamp") {
                file << "    sql += \"" << e.column[i].name << " = \" + std::string(\"'\") + info_." << e.column[i].name << ".buf + std::string(\"'\");" << endl;
            }
            else if (e.column[i].db_type == "tinyblob" ||
                        e.column[i].db_type == "blob" ||
                        e.column[i].db_type == "mediumblob" ||
                        e.column[i].db_type == "longblob" ||
                        e.column[i].db_type == "binary" ||
                        e.column[i].db_type == "varbinary") {
                file << "    sql += std::string(\"" << e.column[i].name << " = \") + hex_format(info_." << e.column[i].name << ".buf, info_." << e.column[i].name << ".len, tmp);" << endl;
            }
            if (i < e.column.size() - 1) {
                file << "    sql += \" and \";" << endl;
            }
        }
        file << "    sql += \";\\n\";" << endl;
        file << "    return sql;" << endl;
        file << "}" << endl;
        file << endl;
    }
}

void gen_del_dump_sql_reference(ofstream &file, table_info &info)
{
    for (auto &e : info.indexes) {
        file << "std::string &" << info.name << "::del_dump_sql_by_" << e.name << "(std::string &sql)" << endl;
        file << "{" << endl;
        file << "    char tmp[4096];" << endl;
        file << "    sql = \"delete from  " << info.name << " where \";" << endl;
        for (int i = 0; i < e.column.size(); i++) {
            if (e.column[i].db_type == "tinyint") {
                file << "    sql += \"" << e.column[i].name << " = \" + std::to_string((int)info_." << e.column[i].name << ");" << endl;
            }
            else if (e.column[i].db_type == "smallint"||
                    e.column[i].db_type == "mediumint" ||
                    e.column[i].db_type == "int" ||
                    e.column[i].db_type == "integer" ||
                    e.column[i].db_type == "bigint" ||
                    e.column[i].db_type == "float" ||
                    e.column[i].db_type == "double") {
                file << "    sql += \"" << e.column[i].name << " = \" + std::to_string(info_." << e.column[i].name << ");" << endl;
            }
            else if (e.column[i].db_type == "char" ||
                    e.column[i].db_type == "varchar" ||
                    e.column[i].db_type == "tinytext" ||
                    e.column[i].db_type == "text" ||
                    e.column[i].db_type == "mediumtext" ||
                    e.column[i].db_type == "longtext" ||
                    e.column[i].db_type == "date" ||
                    e.column[i].db_type == "time" ||
                    e.column[i].db_type == "datetime" ||
                    e.column[i].db_type == "timestamp") {
                file << "    sql += \"" << e.column[i].name << " = \" + std::string(\"'\") + info_." << e.column[i].name << ".buf + std::string(\"'\");" << endl;
            }
            else if (e.column[i].db_type == "tinyblob" ||
                        e.column[i].db_type == "blob" ||
                        e.column[i].db_type == "mediumblob" ||
                        e.column[i].db_type == "longblob" ||
                        e.column[i].db_type == "binary" ||
                        e.column[i].db_type == "varbinary") {
                file << "    sql += std::string(\"" << e.column[i].name << " = \") + hex_format(info_." << e.column[i].name << ".buf, info_." << e.column[i].name << ".len, tmp);" << endl;
            }
            if (i < e.column.size() - 1) {
                file << "    sql += \" and \";" << endl;
            }
        }
        file << "    sql += \";\\n\";" << endl;
        file << "    return sql;" << endl;
        file << "}" << endl;
        file << endl;
    }
}

void gen_dump_reference(ofstream &file, table_info &info)
{
    file << "void " << info.name << "::dump(const char *sql)" << endl;
    file << "{" << endl;
    file << "    if (dump_file_ == \"\") {" << endl;
    file << "        return;" << endl;
    file << "    }" << endl;
    file << "    int fd = open(dump_file_.c_str(), O_RDWR | O_CREAT, 0644);" << endl;
    file << "    if (-1 == fd) {" << endl;
    file << "        ///< doomed" << endl;
    file << "        return ;" << endl;
    file << "    }" << endl;
    file << "    flock(fd, LOCK_EX);" << endl;
    file << "    lseek(fd, 0, SEEK_END);" << endl;
    file << "    write(fd, sql, strlen(sql));" << endl;
    file << "    flock(fd, LOCK_UN);" << endl;
    file << "    close(fd);" << endl;
    file << "}" << endl;
}

void gen_del_reference(ofstream &file, table_info &info)
{
    file << "int32_t " << info.name << "::del(const std::string &where)" << endl;
    file << "{" << endl;
    file << "    std::string sql = \"delete from " << info.name << " \";" << endl;;
    file << "    sql += where;" << endl;
    file << "    if (mysql_->execute(sql)) {" << endl;
    file << "        code_ = mysql_->code();" << endl;
    file << "        msg_ = mysql_->msg();" << endl;
    file << "        sql += \";\\n\";" << endl;
    file << "        dump(sql.c_str());" << endl;
    file << "        return -1;" << endl;
    file << "    }" << endl;
    file << "    return 0;" << endl;
    file << "}" << endl;
    file << endl;
}

void gen_count_reference(ofstream &file, table_info &info)
{
    file << "int32_t " << info.name << "::count(const std::string &where)" << endl;
    file << "{" << endl;
    file << "    std::string sql = \"select count(1) from " << info.name << " \";" << endl;;
    file << "    sql += where;" << endl;
    file << "    if (mysql_->execute(sql)) {" << endl;
    file << "        code_ = mysql_->code();" << endl;
    file << "        msg_ = mysql_->msg();" << endl;
    file << "        return -1;" << endl;
    file << "    }" << endl;
    file << "    int32_t result = 0;" << endl;
    file << "    buffer_field buffer;" << endl;
    file << "    buffer.buf = (char *)&result;" << endl;
    file << "    buffer.size = sizeof(result);" << endl;
    file << "    mysql_->set_field(0, &buffer);" << endl;
    file << "    if (mysql_->fetch() != 0) {" << endl;
    file << "        code_ = mysql_->code();" << endl;
    file << "        msg_ = mysql_->msg();" << endl;
    file << "        return -1;" << endl;
    file << "    }" << endl;
    file << "    mysql_->free_result();" << endl;
    file << "    return result;" << endl;
    file << "}" << endl;
    file << endl;
}

void gen_fetch_reference(ofstream &file, table_info &info)
{
    file << utils::lower(info.name) << STRUCT_TAIL << " *" << info.name << "::fetch()" << endl;
    file << "{" << endl;
    file << "    zero_info();" << endl;
    file << "    int32_t ret = mysql_->fetch();" << endl;
    file << "    if (ret != 0) {" << endl;
    file << "        if (ret == 1) {" << endl;
    file << "            code_ = 0;" << endl;
    file << "            msg_ = \"\";" << endl;
    file << "        }" << endl;
    file << "        else {" << endl;
    file << "            code_ = mysql_->code();" << endl;
    file << "            msg_ = mysql_->msg();" << endl;
    file << "        }" << endl;
    file << "        return nullptr;" << endl;
    file << "    }" << endl;
    file << "    return &info_;" << endl;
    file << "}" << endl;
    file << endl;
}

void gen_select_reference(ofstream &file, table_info &info)
{
    file << "int32_t " << info.name << "::select(const std::string &where, const std::unordered_set<std::string> &omit_fields)" << endl;
    file << "{" << endl;
    file << "    std::string fields;" << endl;
    file << "    for (auto &e : all_fields_) {" << endl;
    file << "        if (omit_fields.find(e) == omit_fields.end()) {" << endl;
    file << "            fields += e + \",\";" << endl;
    file << "        }" << endl;
    file << "    }" << endl;
    file << "    fields.pop_back();" << endl;
    file << "    std::string sql = \"select \" + fields + \" from " << info.name << " \" + where;" << endl;
    file << "    if (mysql_->execute(sql)) {" << endl;
    file << "        code_ = mysql_->code();" << endl;
    file << "        msg_ = mysql_->msg();" << endl;
    file << "        return -1;" << endl;
    file << "    }" << endl;
    int ref = 0;
    for (auto &e : info.fields) {
        file << "    if (omit_fields.find(\"" << e.name << "\") == omit_fields.end()) {" << endl;
        if (e.type != "buffer_field") {
            file << "        buffer_array_[" << ref << "].buf = (char *)&info_." << e.name << ";" << endl;
            file << "        buffer_array_[" << ref << "].size = sizeof(info_." << e.name << ");" << endl;
            file << "        mysql_->set_field(0, &buffer_array_[" << ref++ << "]);" << endl;
        }
        else {
            file << "        mysql_->set_field(0, &info_." << e.name << ");" << endl;
        }
        file << "    }" << endl;
    }
    file << "    return 0;" << endl;
    file << "}" << endl;
    file << endl;
}

void gen_insert_reference(ofstream &file, table_info &info)
{
    file << "int32_t " << info.name << "::insert(const std::unordered_set<std::string> &omit_fields)" << endl;
    file << "{" << endl;
    file << "    std::string fields = \"(\";" << endl;
    file << "    std::string values = \"(\";" << endl;
    file << "    for (auto &e : all_fields_) {" << endl;
    file << "        if (omit_fields.find(e) == omit_fields.end()) {" << endl;
    file << "            fields += e + \",\";" << endl;
    file << "            values += \"?,\";" << endl;
    file << "        }" << endl;
    file << "    }" << endl;
    file << "    fields[fields.length() - 1] = ')';" << endl;
    file << "    values[values.length() - 1] = ')';" << endl;
    file << "    std::string sql = \"insert into " << info.name << " \" + fields + \" values \" + values;" << endl;
    file << "    mysql_stmt *stmt = nullptr;" << endl;
    file << "    if ((stmt = stmts_[sql]) == nullptr) {" << endl;
    file << "        stmt =  mysql_->prepare_stmt(sql);" << endl;
    file << "        if (stmt == nullptr) {" << endl;
    file << "            code_ = mysql_->code();" << endl;
    file << "            msg_ = mysql_->msg();" << endl;
    file << "            sql = insert_dump_sql(sql, omit_fields);" << endl;
    file << "            dump(sql.c_str());" << endl;
    file << "            return -1;" << endl;
    file << "        }" << endl;
    file << "        stmts_[sql] = stmt;" << endl;
    for (auto &f : info.fields) {
        file << "        if (omit_fields.find(\"" << f.name << "\") == omit_fields.end()) {" << endl;
        if (f.db_type == "tinyint") {
            file << "            stmt->set_param_tiny(info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "smallint") {
            file << "            stmt->set_param_short(info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "mediumint" ||
                    f.db_type == "int" ||
                    f.db_type == "integer") {
            file << "            stmt->set_param_long(info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "bigint") {
            file << "            stmt->set_param_longlong(info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "float") {
            file << "            stmt->set_param_float(info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "double") {
            file << "            stmt->set_param_double(info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "char" ||
                    f.db_type == "varchar" ||
                    f.db_type == "tinytext" ||
                    f.db_type == "text" ||
                    f.db_type == "mediumtext" ||
                    f.db_type == "longtext") {
            file << "            stmt->set_param_string(&info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "tinyblob" ||
                    f.db_type == "blob" ||
                    f.db_type == "mediumblob" ||
                    f.db_type == "longblob" ||
                    f.db_type == "binary" ||
                    f.db_type == "varbinary") {
            file << "            stmt->set_param_blob(&info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "date") {
            file << "            stmt->set_param_date(&info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "time") {
            file << "            stmt->set_param_time(&info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "datetime") {
            file << "            stmt->set_param_datetime(&info_." << f.name << ");" << endl;
        }
        else if (f.db_type == "timestamp") {
            file << "            stmt->set_param_timestamp(&info_." << f.name << ");" << endl;
        }
        file << "        }" << endl;
    }
    file << "    }" << endl;
    file << "    if (stmt->execute() != 0) {" << endl;
    file << "        code_ = stmt->code();" << endl;
    file << "        msg_ = stmt->msg();" << endl;
    file << "        std::string sql;" << endl;
    file << "        sql = insert_dump_sql(sql, omit_fields);" << endl;
    file << "        dump(sql.c_str());" << endl;
    file << "        return -1;" << endl;
    file << "    }" << endl;
    file << "    return 0;" << endl;
    file << "}" << endl;
}

void check_param(ofstream &file, index_info &indexes)
{
    vector<field_info> column;
    for (auto &e : indexes.column) {
        if (e.type == "char *") {
            column.push_back(e);
        }
    }
    for (int i = 0; i < column.size(); i++) {
        if (i == 0) {
            file << "    if (" << column[i].name << " == nullptr";
        }
        else {
            file << "||" << column[i].name << " == nullptr";
        }
    }
    if (column.size() > 0) {
        file << ") {" << endl;
        file << "        return -1;" << endl;
        file << "    }" << endl;
    }
}

void gen_indexed_select_reference(ofstream &file, table_info &info)
{
    for (auto &e : info.indexes) {
        gen_index_func_name(file, e, "select_by_", info.name, true);
        file << endl << "{" << endl;
        check_param(file, e);
        file << "    std::string fields;" << endl;
        file << "    for (auto &e : all_fields_) {" << endl;
        file << "        if (omit_fields.find(e) == omit_fields.end()) {" << endl;
        file << "            fields += e + \",\";" << endl;
        file << "        }" << endl;
        file << "    }" << endl;
        file << "    fields.pop_back();" << endl;
        file << "    std::string sql = \"select \" + fields + \" from " << info.name << " where ";
        for (int i = 0; i < e.column.size(); i++) {
            file << e.column[i].name << " = ?";
            if (i < e.column.size() - 1) {
                file << " and ";
            }
        }
        file << "\";" << endl;
        file << "    mysql_stmt *stmt = nullptr;" << endl;
        file << "    if ((stmt = stmts_[sql]) == nullptr) {" << endl;
        file << "        stmt =  mysql_->prepare_stmt(sql);" << endl;
        file << "        if (stmt == nullptr) {" << endl;
        file << "            code_ = mysql_->code();" << endl;
        file << "            msg_ = mysql_->msg();" << endl;
        file << "            return -1;" << endl;
        file << "        }" << endl;
        file << "        stmt_select_by_" << e.name << "_ = stmt;" << endl;
        file << "        stmts_[sql] = stmt;" << endl;
        for (auto &f : info.fields) {
            file << "        if (omit_fields.find(\"" << f.name << "\") == omit_fields.end()) {" << endl;
            if (f.db_type == "tinyint") {
                file << "            stmt->set_result_tiny(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "smallint") {
                file << "            stmt->set_result_short(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "mediumint" ||
                     f.db_type == "int" ||
                     f.db_type == "integer") {
                file << "            stmt->set_result_long(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "bigint") {
                file << "            stmt->set_result_longlong(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "float") {
                file << "            stmt->set_result_float(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "double") {
                file << "            stmt->set_result_double(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "char" ||
                     f.db_type == "varchar" ||
                     f.db_type == "tinytext" ||
                     f.db_type == "text" ||
                     f.db_type == "mediumtext" ||
                     f.db_type == "longtext") {
                file << "            stmt->set_result_string(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "tinyblob" ||
                     f.db_type == "blob" ||
                     f.db_type == "mediumblob" ||
                     f.db_type == "longblob" ||
                    f.db_type == "binary" ||
                    f.db_type == "varbinary") {
                file << "            stmt->set_result_blob(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "date") {
                file << "            stmt->set_result_date(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "time") {
                file << "            stmt->set_result_time(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "datetime") {
                file << "            stmt->set_result_datetime(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "timestamp") {
                file << "            stmt->set_result_timestamp(&info_." << f.name << ");" << endl;
            }
            file << "        }" << endl;
        }
        for (auto &f : e.column) {
            if (f.db_type == "tinyint") {
                file << "        stmt->set_param_tiny(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "smallint") {
                file << "        stmt->set_param_short(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "mediumint" ||
                     f.db_type == "int" ||
                     f.db_type == "integer") {
                file << "        stmt->set_param_long(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "bigint") {
                file << "        stmt->set_param_longlong(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "float") {
                file << "        stmt->set_param_float(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "double") {
                file << "        stmt->set_param_double(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "char" ||
                     f.db_type == "varchar" ||
                     f.db_type == "tinytext" ||
                     f.db_type == "text" ||
                     f.db_type == "mediumtext" ||
                     f.db_type == "longtext") {
                file << "        stmt->set_param_string(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "tinyblob" ||
                     f.db_type == "blob" ||
                     f.db_type == "mediumblob" ||
                     f.db_type == "longblob" ||
                    f.db_type == "binary" ||
                    f.db_type == "varbinary") {
                file << "        stmt->set_param_blob(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "date") {
                file << "        stmt->set_param_date(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "time") {
                file << "        stmt->set_param_time(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "datetime") {
                file << "        stmt->set_param_datetime(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "timestamp") {
                file << "        stmt->set_param_timestamp(&info_." << f.name << ");" << endl;
            }
        }
        file << "    }" << endl;
        for (auto &f : e.column) {
            if (f.type != "char *") {
                file << "    info_." << f.name << " = " << f.name << ";" << endl;
            }
            else {
                file << "    strncpy(info_." << f.name << ".buf, " << f.name << ", info_. " << f.name << ".size - 1);" << endl;
                file << "    info_." << f.name << ".len = strlen(info_." << f.name << ".buf);" << endl;
            }
        }
        file << "    if (stmt->execute() != 0) {" << endl;
        file << "        code_ = " << "stmt->code();" << endl;
        file << "        msg_ = " << "stmt->msg();" << endl;
        file << "        return -1;" << endl;
        file << "    }" << endl;
        file << "    return 0;" << endl;
        file << "}" << endl;
        file << endl;
    }
}

void gen_indexed_fetch_reference(ofstream &file, table_info &info)
{
    for (auto &e : info.indexes) {
        file << utils::lower(info.name) << STRUCT_TAIL << " * " << utils::lower(info.name) << "::fetch_by_" << e.name << "()" << endl;
        file << "{" << endl;
        file << "    if (stmt_select_by_" << e.name << "_ == nullptr) {" << endl;
        file << "        code_ = -1;" << endl;
        file << "        msg_ = \"stmt not prepared!\";" << endl;
        file << "        return nullptr;" << endl;
        file << "    }" << endl;
        file << "    zero_info();" << endl;
        file << "    if (stmt_select_by_" << e.name << "_->fetch() != 0) {" << endl;
        file << "        code_ = " << "stmt_select_by_" << e.name << "_->code();" << endl;
        file << "        msg_ = " << "stmt_select_by_" << e.name << "_->msg();" << endl;
        file << "        return nullptr;" << endl;
        file << "    }" << endl;
        file << "    return &info_;" << endl;
        file << "}" << endl;
        file << endl;
    }
}

void gen_indexed_update_reference(ofstream &file, table_info &info)
{
    for (auto &e : info.indexes) {
        gen_index_func_name(file, e, "update_by_", info.name, true);
        file << endl << "{" << endl;
        check_param(file, e);
        file << "    std::string fields;" << endl;
        file << "    for (auto &e : all_fields_) {" << endl;
        file << "        if (omit_fields.find(e) == omit_fields.end()) {" << endl;
        file << "            fields += e + \" = ?,\";" << endl;
        file << "        }" << endl;
        file << "    }" << endl;
        file << "    fields.pop_back();" << endl;
        file << "    std::string sql = \"update " << info.name << " set \" + fields + \"";
        file << " where ";
        for (int i = 0; i < e.column.size(); i++) {
            file << e.column[i].name << " = ?";
            if (i < e.column.size() - 1) {
                file << " and ";
            }
        }
        file << "\";" << endl;
        file << "    mysql_stmt *stmt = nullptr;" << endl;
        file << "    if ((stmt = stmts_[sql]) == nullptr) {" << endl;
        file << "        stmt =  mysql_->prepare_stmt(sql);" << endl;
        file << "        if (stmt == nullptr) {" << endl;
        file << "            code_ = mysql_->code();" << endl;
        file << "            msg_ = mysql_->msg();" << endl;
        file << "            return -1;" << endl;
        file << "        }" << endl;
        file << "        stmts_[sql] = stmt;" << endl;
        for (auto &f : info.fields) {
            file << "        if (omit_fields.find(\"" << f.name << "\") == omit_fields.end()) {" << endl;
            if (f.db_type == "tinyint") {
                file << "            stmt->set_param_tiny(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "smallint") {
                file << "            stmt->set_param_short(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "mediumint" ||
                     f.db_type == "int" ||
                     f.db_type == "integer") {
                file << "            stmt->set_param_long(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "bigint") {
                file << "            stmt->set_param_longlong(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "float") {
                file << "            stmt->set_param_float(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "double") {
                file << "            stmt->set_param_double(info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "char" ||
                     f.db_type == "varchar" ||
                     f.db_type == "tinytext" ||
                     f.db_type == "text" ||
                     f.db_type == "mediumtext" ||
                     f.db_type == "longtext") {
                file << "            stmt->set_param_string(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "tinyblob" ||
                     f.db_type == "blob" ||
                     f.db_type == "mediumblob" ||
                     f.db_type == "longblob" ||
                    f.db_type == "binary" ||
                    f.db_type == "varbinary") {
                
                file << "            stmt->set_param_blob(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "date") {
                file << "            stmt->set_param_date(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "time") {
                file << "            stmt->set_param_time(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "datetime") {
                file << "            stmt->set_param_datetime(&info_." << f.name << ");" << endl;
            }
            else if (f.db_type == "timestamp") {
                file << "            stmt->set_param_timestamp(&info_." << f.name << ");" << endl;
            }
            file << "        }" << endl;
        }
        for (auto &f : e.column) {
            if (f.db_type == "tinyint") {
                file << "        stmt->set_param_tiny(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "smallint") {
                file << "        stmt->set_param_short(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "mediumint" ||
                     f.db_type == "int" ||
                     f.db_type == "integer") {
                file << "        stmt->set_param_long(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "bigint") {
                file << "        stmt->set_param_longlong(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "float") {
                file << "        stmt->set_param_float(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "double") {
                file << "        stmt->set_param_double(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "char" ||
                     f.db_type == "varchar" ||
                     f.db_type == "tinytext" ||
                     f.db_type == "text" ||
                     f.db_type == "mediumtext" ||
                     f.db_type == "longtext") {
                file << "        stmt->set_param_string(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "tinyblob" ||
                     f.db_type == "blob" ||
                     f.db_type == "mediumblob" ||
                     f.db_type == "longblob" ||
                    f.db_type == "binary" ||
                    f.db_type == "varbinary") {
                file << "        stmt->set_param_blob(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "date") {
                file << "        stmt->set_param_date(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "time") {
                file << "        stmt->set_param_time(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "datetime") {
                file << "        stmt->set_param_datetime(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "timestamp") {
                file << "        stmt->set_param_timestamp(&where_" << e.name << "_." << f.name << ");" << endl;
            }
        }
        file << "    }" << endl;
        for (auto &f : e.column) {
            if (f.type != "char *") {
                file << "    where_" << e.name << "_." << f.name << " = " << f.name << ";" << endl;
            }
            else {
                file << "    where_" << e.name << "_." << f.name << ".buf = " << f.name << ";" << endl;
                file << "    where_" << e.name << "_." << f.name << ".len = strlen(" << f.name << ");" << endl;
                file << "    where_" << e.name << "_." << f.name << ".size = where_" << e.name << "_." << f.name << ".len + 1;" << endl;
            }
        }
        file << "    if (stmt->execute() != 0) {" << endl;
        file << "        code_ = " << "stmt->code();" << endl;
        file << "        msg_ = " << "stmt->msg();" << endl;
        file << "        std::string sql;" << endl;
        file << "        sql = update_dump_sql_by_" << e.name << "(sql, omit_fields);" << endl;
        file << "        dump(sql.c_str());" << endl;
        file << "        return -1;" << endl;
        file << "    }" << endl;
        file << "    return 0;" << endl;
        file << "}" << endl;
        file << endl;
    }
}

void gen_indexed_delete_reference(ofstream &file, table_info &info)
{
    for (auto &e : info.indexes) {
        gen_index_func_name(file, e, "del_by_", info.name, true);
        file << endl << "{" << endl;
        check_param(file, e);
        file << "    if (stmt_del_by_" << e.name << "_ == nullptr) {" << endl;
        file << "        std::string sql = \"delete from  " << info.name;
        file << " where ";
        for (int i = 0; i < e.column.size(); i++) {
            file << e.column[i].name << " = ?";
            if (i < e.column.size() - 1) {
                file << " and ";
            }
        }
        file << "\";" << endl;
        file << "        stmt_del_by_" << e.name << "_ =  mysql_->prepare_stmt(sql);" << endl;
        file << "        if (stmt_del_by_" << e.name << "_ == nullptr) {" << endl;
        file << "            code_ = mysql_->code();" << endl;
        file << "            msg_ = mysql_->msg();" << endl;
        file << "            sql = del_dump_sql_by_" << e.name << "(sql);" << endl;
        file << "            dump(sql.c_str());" << endl;
        file << "            return -1;" << endl;
        file << "        }" << endl;
        
        for (auto &f : e.column) {
            if (f.db_type == "tinyint") {
                file << "        stmt_del_by_" << e.name << "_->set_param_tiny(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "smallint") {
                file << "        stmt_del_by_" << e.name << "_->set_param_short(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "mediumint" ||
                     f.db_type == "int" ||
                     f.db_type == "integer") {
                file << "        stmt_del_by_" << e.name << "_->set_param_long(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "bigint") {
                file << "        stmt_del_by_" << e.name << "_->set_param_longlong(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "float") {
                file << "        stmt_del_by_" << e.name << "_->set_param_float(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "double") {
                file << "        stmt_del_by_" << e.name << "_->set_param_double(where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "char" ||
                     f.db_type == "varchar" ||
                     f.db_type == "tinytext" ||
                     f.db_type == "text" ||
                     f.db_type == "mediumtext" ||
                     f.db_type == "longtext") {
                file << "        stmt_del_by_" << e.name << "_->set_param_string(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "tinyblob" ||
                     f.db_type == "blob" ||
                     f.db_type == "mediumblob" ||
                     f.db_type == "longblob" ||
                    f.db_type == "binary" ||
                    f.db_type == "varbinary") {
                file << "        stmt_del_by_" << e.name << "_->set_param_blob(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "date") {
                file << "        stmt_del_by_" << e.name << "_->set_param_date(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "time") {
                file << "        stmt_del_by_" << e.name << "_->set_param_time(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "datetime") {
                file << "        stmt_del_by_" << e.name << "_->set_param_datetime(&where_" << e.name << "_." << f.name << ");" << endl;
            }
            else if (f.db_type == "timestamp") {
                file << "        stmt_del_by_" << e.name << "_->set_param_timestamp(&where_" << e.name << "_." << f.name << ");" << endl;
            }
        }
        file << "    }" << endl;
        for (auto &f : e.column) {
            if (f.type != "char *") {
                file << "    where_" << e.name << "_." << f.name << " = " << f.name << ";" << endl;
            }
            else {
                file << "    where_" << e.name << "_." << f.name << ".buf = " << f.name << ";" << endl;
                file << "    where_" << e.name << "_." << f.name << ".len = strlen(" << f.name << ");" << endl;
                file << "    where_" << e.name << "_." << f.name << ".size = where_" << e.name << "_." << f.name << ".len + 1;" << endl;
            }
        }
        file << "    if (stmt_del_by_" << e.name << "_->execute() != 0) {" << endl;
        file << "        code_ = " << "stmt_del_by_" << e.name << "_->code();" << endl;
        file << "        msg_ = " << "stmt_del_by_" << e.name << "_->msg();" << endl;
        file << "        std::string sql;" << endl;
        file << "        sql = del_dump_sql_by_" << e.name << "(sql);" << endl;
        file << "        dump(sql.c_str());" << endl;
        file << "        return -1;" << endl;
        file << "    }" << endl;
        file << "    return 0;" << endl;
        file << "}" << endl;
        file << endl;
    }
}

void gen_db_unrelated_reference(ofstream &file, table_info &info)
{
    ///< 构造函数
    file << utils::lower(info.name) << "::" << utils::lower(info.name) << "()" << endl;
    file << "{" << endl;
    for (auto &e : info.fields) {
        if (e.size.length() != 0) {
            file << "    info_." << e.name << ".buf = new char[" << e.size << "];" << endl;
            file << "    info_." << e.name << ".size = " << e.size << ";" << endl;
        }
    }
    file << "    zero_info();" << endl;
    int ref = 0;
    for (auto &e : info.fields) {
        if (e.type != "buffer_field") {
            ref++;
        }
    }
    file << "    buffer_array_.resize(" << ref << ", {0});" << endl;
    gen_init_all_fields(file, info);
    file << "}" << endl;
    file << endl;
    ///< 析构函数
    file << utils::lower(info.name) << "::~" << utils::lower(info.name) << "()" << endl;
    file << "{" << endl;
    for (auto &e : info.fields) {
        if (e.size.length() != 0) {
            file << "    delete []info_." << e.name << ".buf;" << endl;
        }
    }
    file << "}" << endl;
    file << endl;
    ///< zero_info
    file << "void " << utils::lower(info.name) << "::zero_info()" << endl;
    file << "{" << endl;
    for (auto &e : info.fields) {
        if (e.size.length() != 0) {
            file << "    memset(info_." << e.name << ".buf, 0, " << e.size << ");" << endl;
            file << "    info_." << e.name << ".len = 0;" << endl;
        }
        else {
            file << "    memset(&info_." << e.name << ", 0, sizeof(info_." << e.name << "));" << endl;
        }
    }
    file << "}" << endl;
    file << endl;
}

void gen_struct(ofstream &file, table_info &info)
{
    file << "struct " << utils::lower(info.name) << STRUCT_TAIL << endl;
    file << "{" << endl;
    for (auto &e : info.fields) {
        file << "    " << e.type << " " << e.name << ";" << endl;
    }
    file << "};" << endl << endl;;
    for (auto &e : info.indexes) {
        file << "struct " << utils::lower(info.name) << "_" << utils::lower(e.name) << endl;
        file << "{" << endl;
        for (auto &f : e.column) {
            if (f.type != "char *") {
                file << "    " << f.type << " " << f.name << ";" << endl;
            }
            else {
                file << "    buffer_field " << f.name << ";" << endl;
            }
        }
        file << "};" << endl << endl;
    }
    file << endl;
}

void gen_init_all_fields(ofstream &file, table_info &info)
{
    for (auto &e : info.fields) {
        file << "    all_fields_.push_back(\"" << e.name << "\");" << endl;
    }
}

void gen_defination(ofstream &file, table_info &info)
{
    file << "public:" << endl;
    file << "    " << utils::lower(info.name) << "();" << endl;
    file << "    ~" << utils::lower(info.name) << "();" << endl;
    file << "    void set_mysql(mysql_api *mysql){mysql_ = mysql;}" << endl;
    file << "    void set_dump_path(const char *path)" << endl;
    file << "    {" << endl;
    file << "        dump_file_ = path;" << endl;
    file << "        dump_file_ += \"/" << utils::lower(info.name) << ".sql\";" << endl;
    file << "        mkdir(path, 0755);" << endl;
    file << "    }" << endl;
    file << "    std::string &msg() {return msg_;}" << endl;
    file << "    int32_t code() {return code_;}" << endl;
    file << "public:" << endl;
    file << "    int32_t insert(const std::unordered_set<std::string> &omit_fields = {});" << endl;
    file << "    int32_t select(const std::string &where = \"\", const std::unordered_set<std::string> &omit_fields = {});" << endl;
    file << "    " << utils::lower(info.name) << STRUCT_TAIL << " *fetch();" << endl;
    file << "    int32_t count(const std::string &where = \"\");" << endl;
    file << "    int32_t del(const std::string &where = \"\");" << endl;
    for (auto &e : info.indexes) {
        gen_index_func_name(file, e, "select_by_");
        file << ";" << endl;
        file << "    " << utils::lower(info.name) << STRUCT_TAIL << " *fetch_by_" << e.name << "();" << endl;
        gen_index_func_name(file, e, "update_by_");
        file << ";" << endl;
        gen_index_func_name(file, e, "del_by_");
        file << ";" << endl;
    }
    file << "    " << utils::lower(info.name) << STRUCT_TAIL << " *get_info()" << endl;
    file << "    {" << endl;
    file << "        zero_info();" << endl;
    file << "        return &info_;" << endl;
    file << "    }" << endl;
    file << "    void zero_info();" << endl;
    file << "private:" << endl;
    file << "    char *hex_format(char *binary, int32_t binary_len, char *buf)" << endl;
    file << "    {" << endl;
    file << "        if (binary == nullptr || buf == nullptr || binary_len == 0) {" << endl;
    file << "            strcpy(buf, \"null\");" << endl;
    file << "            return buf;" << endl;
    file << "        }" << endl;
    file << "        strcpy(buf, \"0x\");" << endl;
    file << "        char *tmp = buf + 2;" << endl;
    file << "        for (int32_t i=0; i < binary_len; i++, tmp += 2) {" << endl;
    file << "            sprintf(tmp, \"%02x\", (unsigned char)*(binary++));" << endl;
    file << "        }" << endl;
    file << "        return buf;" << endl;
    file << "    }" << endl;
    file << "    void dump(const char *sql);" << endl;
    file << "    std::string &insert_dump_sql(std::string &sql, const std::unordered_set<std::string> &omit_fields);" << endl;
    for (auto &e : info.indexes) {
        file << "    std::string &update_dump_sql_by_" << e.name << "(std::string &sql, const std::unordered_set<std::string> &omit_fields);" << endl;
        file << "    std::string &del_dump_sql_by_" << e.name << "(std::string &sql);" << endl;
    }
    file << "private:" << endl;
    file << "    " << utils::lower(info.name) << STRUCT_TAIL << " info_;" << endl;
    file << "    mysql_api *mysql_ = {nullptr};" << endl;
    file << "    mysql_stmt *stmt_insert_" << " = {nullptr};" << endl;
    for (auto &e : info.indexes) {
        file << "    mysql_stmt *stmt_select_by_" << e.name << "_ = {nullptr};" << endl;
        file << "    mysql_stmt *stmt_del_by_" << e.name << "_ = {nullptr};" << endl;
        file << "    " << info.name << "_" << e.name << " where_" << e.name << "_;"<< endl;
    }
    file << "    std::vector<buffer_field> buffer_array_;" << endl;
    file << "    std::string msg_;" << endl;
    file << "    int32_t code_;" << endl;
    file << "    std::string dump_file_;" << endl;
    file << "    std::vector<std::string> all_fields_;" << endl;
    file << "    std::unordered_map<std::string, mysql_stmt *> stmts_;" << endl;
}

void gen_index_func_name(ofstream &file, index_info &e, string func_head, string name_space, bool bsrc)
{
    if (name_space.length() == 0) {
        file << "    int32_t " << func_head << utils::lower(e.name) << "(";
    }
    else
    {
        file << "int32_t " << name_space << "::" << func_head << utils::lower(e.name) << "(";
    }
    for (int32_t i = 0; i < e.column.size(); i++) {
        file << e.column[i].type;
        if (e.column[i].type != "char *") {
            file  << " ";
        }
        file << e.column[i].name;
        if (e.column[i].db_type == "tinyblob" ||
            e.column[i].db_type == "blob" ||
            e.column[i].db_type == "mediumblob" ||
            e.column[i].db_type == "longblob" ||
            e.column[i].db_type == "binary" ||
            e.column[i].db_type == "varbinary") {
            file << ", int32_t " << e.column[i].name << "_len";
        }
        if (i < e.column.size() - 1) {
            file << ", ";
        }
    }
    if (bsrc) {
        file << ", const std::unordered_set<std::string> &omit_fields)";
    } 
    else {
        file << ", const std::unordered_set<std::string> &omit_fields = {})";
    }
    
}

void print_err(string mark)
{
    cout << mark;
    cout <<  " error: [";
    cout << mysql.code();
    cout << "][";
    cout << mysql.msg();
    cout << "]" << endl;
}