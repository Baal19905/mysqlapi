#ifndef _STRUCT_H_
#define _STRUCT_H_
#include <iostream>
#include <vector>
#include <string.h>
#include <string>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;


struct field_info
{
    string type;
    string name;
    string size;
    string db_type;
};

struct index_info
{
    string name;
    bool uniq;
    vector<field_info> column;
};

struct table_info
{
    string name;
    vector<field_info> fields;
    vector<index_info> indexes;
};

#endif ///< _STRUCT_H_