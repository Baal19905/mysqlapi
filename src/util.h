#ifndef _UTIL_H_
#define _UTIL_H_
#include <string>
#include "mysql.h"

class util
{
public:
    /**
     * @brief 创建bind队列
     *
     * {@param[in] date}
     * [@note format1: YYYY-MM-DD_HH:MM:SS.SSSSSS]
     * [@note format2: YYYY-MM-DD]
     * [@note format3: HH:MM:SS.SSSSSS]
     * [@note format4: HH:MM:SS]
     */
    util(const std::string &date);
    util(const char *pdate, uint32_t len);
public:
    /**
     * @brief 获取年份
     *
     * {@return uint32_t}
     * {@retval 年份}
     */
    uint32_t get_year();
    /**
     * @brief 获取月份
     *
     * {@return uint32_t}
     * {@retval 月份}
     */
    uint32_t get_month();
    /**
     * @brief 获取日
     *
     * {@return uint32_t}
     * {@retval 日}
     */
    uint32_t get_day();
    /**
     * @brief 获取小时
     *
     * {@return uint32_t}
     * {@retval 小时}
     */
    uint32_t get_hour();
    /**
     * @brief 获取分钟
     *
     * {@return uint32_t}
     * {@retval 分钟}
     */
    uint32_t get_minute();
    /**
     * @brief 获取秒
     *
     * {@return uint32_t}
     * {@retval 秒}
     */
    uint32_t get_second();
    /**
     * @brief 获取毫秒
     *
     * {@return uint32_t}
     * {@retval 毫秒}
     */
    uint32_t get_msecond();
    static void convert_to_mysql_time(char *buf, uint32_t len, MYSQL_TIME *time);
private:
    util(){}
    /**
     * @brief 初始化
     *
     * {@param[in] date}
     * [@note format1: YYYY-MM-DD_HH:MM:SS.SSSSSS]
     * [@note format2: YYYY-MM-DD]
     * [@note format3: HH:MM:SS.SSSSSS]
     * [@note format4: HH:MM:SS]
     */
    void init(const std::string &date);
    /**
     * @brief 判断是否为date格式
     *
     * {@param[in] date}
     * {@return bool}
     * {@retval true|false}
     * [@note format: YYYY-MM-DD]
     */
    bool is_date_format(const std::string &date);
    /**
     * @brief 判断是否为time格式
     *
     * {@param[in] date}
     * {@return bool}
     * {@retval true|false}
     * [@note format: HH:MM:SS]
     */
    bool is_time_format(const std::string &date);
    /**
     * @brief 判断是否为time_with_micro格式
     *
     * {@param[in] date}
     * {@return bool}
     * {@retval true|false}
     * [@note format: HH:MM:SS.SSSSSS]
     */
    bool is_time_with_micro_format(const std::string &date);
    /**
     * @brief 判断是否为datetime格式
     *
     * {@param[in] date}
     * {@return bool}
     * {@retval true|false}
     * [@note format: YYYY-MM-DD_HH:MM:SS.SSSSSS]
     */
    bool is_datetime_format(const std::string &date);
    /**
     * @brief 判断是否为数字字符串
     *
     * {@param[in] str}
     * {@return bool}
     * {@retval true|false}
     */
    bool is_digit(const std::string &str);
private:
    uint32_t year_ = {0};
    uint32_t month_ = {0};
    uint32_t day_ = {0};
    uint32_t hour_ = {0};
    uint32_t minute_ = {0};
    uint32_t second_ = {0};
    uint32_t msecond_ = {0};
};



#endif  ///< _UTIL_H_