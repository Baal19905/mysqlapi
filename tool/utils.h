#ifndef _UTILS_H_
#define _UTILS_H_
#include "struct.h"

class utils
{
public:
    static string &upper(string &str)
    {
        transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }
    static string &lower(string &str)
    {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    static const char *get_size(const char *type, char *length)
    {
        ///< 字符串
        if (strncmp("char", type, strlen("char")) == 0 ||
            strncmp("varchar", type, strlen("varchar")) == 0 ||
            strncmp("binary", type, strlen("binary")) == 0 ||
            strncmp("varbinary", type, strlen("varbinary")) == 0) {
            const char *ps = nullptr, *pe = nullptr;
            ps = strstr(type, "(");
            if (ps == nullptr) {
                *length = 0;
            }
            else {
                pe = strstr(++ps, ")");
                if (pe == nullptr) {
                    *length = 0;
                }
            }
            uint16_t len = pe - ps;
            strncpy(length, ps, len);
            length[len] = 0;
            strcat(length, " + 1");
        }
        else if (strncmp("tinytext", type, strlen("tinytext")) == 0 ||
                 strncmp("tinyblob", type, strlen("tinyblob")) == 0) {
            strcpy(length, "255 + 1");
        }
        else if (strncmp("text", type, strlen("text")) == 0 ||
                 strncmp("blob", type, strlen("blob")) == 0) {
            strcpy(length, "65535 + 1");
        }
        else if (strncmp("mediumtext", type, strlen("mediumtext")) == 0 ||
                 strncmp("mediumblob", type, strlen("mediumblob")) == 0) {
            strcpy(length, "16777215 + 1");
        }
        else if (strncmp("longtext", type, strlen("longtext")) == 0 ||
                 strncmp("longblob", type, strlen("longblob")) == 0) {
            strcpy(length, "4294967295 + 1");
        }
        else if (strncmp("date", type, strlen("date")) == 0 ||
                 strncmp("time", type, strlen("time")) == 0 ||
                 strncmp("datetime", type, strlen("datetime")) == 0 ||
                 strncmp("timestamp", type, strlen("timestamp")) == 0 ||
                 strncmp("year", type, strlen("year")) == 0) {
            strcpy(length, "26 + 1");
        }
        else {
            *length = 0;
        }
        return length;
    }
    static const char *get_db_type(char *type)
    {
        char *p = strstr(type, "\(");
        if (p != nullptr) {
            *p = 0;
        }
        return type;
    }
    static const char *type_convert_db_to_c(const char *type)
    {
        if (strncmp("tinyint", type, strlen("tinyint")) == 0 ||
            strncmp("bit", type, strlen("bit")) == 0) {
            if (strstr(type, "unsigned") != nullptr) {
                return "unsigned char";
            }
            return "char";
        }
        else if (strncmp("smallint", type, strlen("smallint")) == 0) {
            if (strstr(type, "unsigned") != nullptr) {
                return "uint16_t";
            }
            return "int16_t";
        }
        else if (strncmp("mediumint", type, strlen("mediumint")) == 0 || 
                 strncmp("int", type, strlen("int")) == 0 || 
                 strncmp("integer", type, strlen("integer")) == 0) {
            if (strstr(type, "unsigned") != nullptr) {
                return "uint32_t";
            }
            return "int32_t";
        }
        else if (strncmp("bigint", type, strlen("bigint")) == 0) {
            if (strstr(type, "unsigned") != nullptr) {
                return "uint64_t";
            }
            return "int64_t";
        }
        else if (strncmp("float", type, strlen("float")) == 0 || 
                 strncmp("decimal", type, strlen("decimal")) == 0) {
            return "float";
        }
        else if (strncmp("double", type, strlen("double")) == 0) {
            return "double";
        }
        else if (strncmp("char", type, strlen("char")) == 0 ||
                 strncmp("varchar", type, strlen("varchar")) == 0 ||
                 strncmp("tinytext", type, strlen("tinytext")) == 0 ||
                 strncmp("text", type, strlen("text")) == 0 ||
                 strncmp("mediumtext", type, strlen("mediumtext")) == 0 ||
                 strncmp("longtext", type, strlen("longtext")) == 0 ||
                 strncmp("date", type, strlen("date")) == 0 ||
                 strncmp("time", type, strlen("time")) == 0 ||
                 strncmp("datetime", type, strlen("datetime")) == 0 ||
                 strncmp("timestamp", type, strlen("timestamp")) == 0 ||
                 strncmp("year", type, strlen("year")) == 0 ||
                 strncmp("tinyblob", type, strlen("tinyblob")) == 0 ||
                 strncmp("blob", type, strlen("blob")) == 0 ||
                 strncmp("mediumblob", type, strlen("mediumblob")) == 0 ||
                 strncmp("longblob", type, strlen("longblob")) == 0 ||
                 strncmp("binary", type, strlen("binary")) == 0 ||
                 strncmp("varbinary", type, strlen("varbinary")) == 0) {
            return "buffer_field";
        }
        else {
            std::cout << "unkown type["  << type << "]" << std::endl;
            return "";
        }
    }
};


#endif  ///< _UTILS_H_