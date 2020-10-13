#include "util.h"
#include <ctype.h>
#include <string.h>

#define DATE_FORMAT             "YYYY-MM-DD"
#define TIME_FORMAT             "HH:MM:SS"
#define TIME_WITH_MICRO_FORMAT  "HH:MM:SS.SSSSSS"
#define DATETIME_FORMAT         "YYYY-MM-DD_HH:MM:SS.SSSSSS"

util::util(const std::string &date)
{
    init(date);
}

util::util(const char *pdate, uint32_t len)
{
    std::string date_str(pdate, len);
    init(date_str);
}

void util::init(const std::string &date)
{
    try {
        if (true == is_date_format(date)) {
            std::string year = date.substr(0, 4);
            std::string mon = date.substr(5, 2);
            std::string day = date.substr(8, 2);
            year_ = std::atoi(year.data());
            month_ = std::atoi(mon.data());
            day_ = std::atoi(day.data());
        }
        else if (true == is_time_format(date)) {
            std::string hour = date.substr(0, 2);
            std::string minute = date.substr(3, 2);
            std::string second = date.substr(6);
            hour_ = std::atoi(hour.data());
            minute_ = std::atoi(minute.data());
            second_ = std::atoi(second.data());
        }
        else if (true == is_time_with_micro_format(date)) {
            std::string hour = date.substr(0, 2);
            std::string minute = date.substr(3, 2);
            std::string second = date.substr(6, 2);
            std::string msecond = date.substr(9);
            hour_ = std::atoi(hour.data());
            minute_ = std::atoi(minute.data());
            second_ = std::atoi(second.data());
            msecond_ = std::atoi(msecond.data());
        }
        else if (true == is_datetime_format(date)) {
            std::string year = date.substr(0, 4);
            std::string mon = date.substr(5, 2);
            std::string day = date.substr(8, 2);
            std::string hour = date.substr(11, 2);
            std::string minute = date.substr(14, 2);
            std::string second = date.substr(17,2);
            std::string msecond = date.substr(20);
            year_ = std::atoi(year.data());
            month_ = std::atoi(mon.data());
            day_ = std::atoi(day.data());
            hour_ = std::atoi(hour.data());
            minute_ = std::atoi(minute.data());
            second_ = std::atoi(second.data());
            msecond_ = std::atoi(msecond.data());
        }
    }
    catch (...) {
        ///< do nothing
    }
}

bool util::is_date_format(const std::string &date)
{
    if (date.length() != strlen(DATE_FORMAT)) {
        return false;
    }
    if (date.at(4) != '-' || date.at(7) != '-') {
        return false;
    }
    std::string yyyy = date.substr(0, 4);
    if (true != is_digit(yyyy)) {
        return false;
    }
    std::string mm = date.substr(5, 2);
    if (true != is_digit(mm)) {
        return false;
    }
    std::string dd = date.substr(8, 2);
    if (true != is_digit(dd)) {
        return false;
    }
    return true;
}

bool util::is_time_format(const std::string &date)
{
    if (date.length() != strlen(TIME_FORMAT)) {
        return false;
    }
    if (date.at(2) != ':' || date.at(5) != ':') {
        return false;
    }
    std::string hh = date.substr(0, 2);
    if (true != is_digit(hh)) {
        return false;
    }
    std::string mm = date.substr(3, 2);
    if (true != is_digit(mm)) {
        return false;
    }
    std::string ss = date.substr(6);
    if (true != is_digit(ss)) {
        return false;
    }
    return true;
}

bool util::is_time_with_micro_format(const std::string &date)
{
    if (date.length() != strlen(TIME_WITH_MICRO_FORMAT)) {
        return false;
    }
    if (date.at(2) != ':' || date.at(5) != ':' || date.at(8) != '.') {
        return false;
    }
    std::string hh = date.substr(0, 2);
    if (true != is_digit(hh)) {
        return false;
    }
    std::string mm = date.substr(3, 2);
    if (true != is_digit(mm)) {
        return false;
    }
    std::string ss = date.substr(6, 2);
    if (true != is_digit(ss)) {
        return false;
    }
    std::string ssssss = date.substr(9);
    if (true != is_digit(ssssss)) {
        return false;
    }
    return true;
}

bool util::is_datetime_format(const std::string &date)
{
    if (date.length() != strlen(DATETIME_FORMAT)) {
        return false;
    }
    uint32_t d_len = strlen(DATE_FORMAT);
    if (date.at(d_len) != '_') {
        return false;
    }
    std::string d = date.substr(0, strlen(DATE_FORMAT));
    std::string t = date.substr(d_len + 1);
    if (true != is_date_format(d) || true != is_time_with_micro_format(t)) {
        return false;
    }
    return true;
}

bool util::is_digit(const std::string &str)
{
    for (auto &e : str) {
        if (isdigit(e) == 0) {
            return false;
        }
    }
    return true;
}

uint32_t util::get_year()
{
    return year_;
}

uint32_t util::get_month()
{
    return month_;
}

uint32_t util::get_day()
{
    return day_;
}

uint32_t util::get_hour()
{
    return hour_;
}

uint32_t util::get_minute()
{
    return minute_;
}

uint32_t util::get_second()
{
    return second_;
}

uint32_t util::get_msecond()
{
    return msecond_;
}

void util::convert_to_mysql_time(char *buf, uint32_t len, MYSQL_TIME *time)
{
    util u(buf, len);
    time->year = u.get_year();
    time->month = u.get_month();
    time->day = u.get_day();
    time->hour = u.get_hour();
    time->minute = u.get_minute();
    time->second = u.get_second();
    time->second_part = u.get_msecond();
}