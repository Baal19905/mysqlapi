#ifndef MYSQL_CLIENT_STRUCT_H_
#define MYSQL_CLIENT_STRUCT_H_
#include <stdint.h>
#include "mysql.h"


struct buffer_field
{
    char *buf;
    uint64_t size;
    uint64_t len;
};

struct buffer_array
{
    buffer_field **array;
    uint32_t size;
    uint32_t pos;
};

class binds
{
public:
    /**
     * @brief 创建bind队列
     *
     * {@param[in] size}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note size为0时默认创建128个元素的队列]
     */
    int32_t create(uint32_t size = 0);
    /**
     * @brief bind队列大小
     *
     * {@param[in] size}
     * {@return uint32_t}
     * {@retval 大小}
     */
    uint32_t size();
    /**
     * @brief 改变bind队列大小
     *
     * {@param[in] size}
     * {@return uint32_t}
     * {@retval 0 succ; -1 fail}
     */
    int32_t resize(uint32_t size);
    /**
     * @brief 重置bind队列当前位置
     */
    void rewind();
    /**
     * @brief bind队列头
     *
     * {@param[in] size}
     * {@return MYSQL_BIND *}
     * {@retval 非空 succ; 空 fail}
     */
    MYSQL_BIND *head();
    /**
     * @brief 获取可用的bind元素，并返回此bind元素索引
     *
     * {@param[out] bind}
     * {@return int32_t}
     * {@retval 大于等于0 succ; -1 fail}
     */
    int32_t next(MYSQL_BIND *&bind);
    /**
     * @brief 获取当前索引
     *
     * {@return uint32_t}
     * {@retval 当前索引}
     */
    uint32_t pos();
    /**
     * @brief 根据索引获取bind_time
     *
     * {@param[in] pos}
     * {@return MYSQL_TIME *}
     * {@retval 非空 succ; 空 fail}
     */
    MYSQL_TIME *time_by_pos(uint32_t pos);
    /**
     * @brief 销毁bind队列
     */
    void destroy();
private:
    MYSQL_BIND *bind_ = {nullptr};
    MYSQL_TIME *time_ = {nullptr};
    uint32_t size_ = {0};
    uint32_t pos_ = {0};
};

class result_set
{
public:
    /**
     * @brief 创建结果集
     *
     * {@param[in] size}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note size为0时默认创建1个结果集]
     */
    int32_t create(uint32_t size = 0);
    /**
     * @brief 销毁结果集
     */
    void destroy();
    /**
     * @brief 创建结果集
     *
     * {@param[in] pos}
     * {@param[in] buffer}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note pos为结果集索引，从0开始]
     */
    int32_t set_field(uint32_t pos, buffer_field *buffer);
    /**
     * @brief 重置当前结果集索引
     *
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     */
    int32_t rewind();
    /**
     * @brief 创建结果集
     *
     * {@param[in] pos}
     * {@param[in] buffer}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note pos为结果集索引，从0开始]
     */
    int32_t next(buffer_field **&set);
private:
    /**
     * @brief 初始化结果集内部队列
     *
     * {@param[in] array}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     */
    int32_t init_array(buffer_array *array);
private:
    buffer_array *result_sets_ = {nullptr};
    uint32_t size_ = {0};
    uint32_t result_sets_pos_ = {0};
};


#endif  ///< MYSQL_CLIENT_STRUCT_H_