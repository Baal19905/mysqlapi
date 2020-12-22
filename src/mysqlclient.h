#ifndef _MYSQL_CLIENT_INTERFACE_H_
#define _MYSQL_CLIENT_INTERFACE_H_
#include "mysql.h"
#include "sqlstruct.h"
#include <string>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <utility>


class mysql_stmt;

class mysql_api
{
public:
    /**
     * @brief 创建mysql_api实例
     *
     * {@param[in] ip server ip}
     * {@param[in] usr 用户名}
     * {@param[in] pwd 密码}
     * {@param[in] db 数据库实例}
     * {@param[in] port server port}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     */
    int32_t create(const std::string &ip, const std::string &usr, const std::string &pwd, const std::string &db, uint16_t port);
    /**
     * @brief 销毁mysql_api实例
     *
     */
    void destroy();
    /**
     * @brief prepare一个sql语句
     *
     * {@param[in] sql}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note 当sql会频繁使用时建议使用此方法，否则使用execute]
     */
    mysql_stmt *prepare_stmt(const std::string &sql);
    /**
     * @brief 设置/取消自动提交
     *
     * {@param[in] mode true设置; false取消}
     * {@return bool}
     * {@retval true succ; false fail}
     * [@note 不调用此方法默认自动提交]
     */
    bool set_commitmode(bool mode);
    /**
     * @brief 被sql影响的行数
     *
     * {@return uint32_t}
     * {@retval 行数}
     */
    uint32_t affect_rows() {return rows_;}
    /**
     * @brief 执行sql
     *
     * {@param[in] sql}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note 支持多statement]
     */
    int32_t execute(const std::string &sql);
    /**
     * @brief 执行sql
     *
     * {@return int32_t}
     * {@retval 0 succ; -1 fail; 1 当前结果集为空}
     * [@note 支持多statement]
     */
    int32_t fetch();
    /**
     * @brief 清空结果集
     */
    void free_result();
    /**
     * @brief 从结果集取1行
     *
     * {@param[in] set_pos 结果集的id，从0开始}
     * {@param[in] field 列缓冲区}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note 调用顺序和select字段顺序必须相同]
     * [@note 支持多statement, 第一个结果集set_pos填0, 第二个填1，以此类推]
     */
    int32_t set_field(uint32_t set_pos, buffer_field *field);
    /**
     * @brief commit
     *
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note 自动提交模式此方法无用]
     */
    int32_t commit();
    /**
     * @brief rollback
     *
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note 自动提交模式此方法无用]
     */
    int32_t rollback();
    /**
     * @brief 错误码
     *
     * {@return uint32_t}
     * {@retval 错误码}
     */
    uint32_t code();
    /**
     * @brief 错误信息
     *
     * {@return const char *}
     * {@retval 错误信息}
     */
    const char *msg();
private:
    MYSQL *hdl_ = {nullptr};
    uint32_t rows_ = {0};
    std::vector<mysql_stmt *> stmts_;
    MYSQL_RES * res_ = {nullptr};
    result_set result_set_;
    buffer_field **current_set_ = {nullptr};
};


class mysql_stmt
{
public:
    /**
     * @brief 创建mysql_stmt实例
     *
     * {@param[in] hdl mysql连接句柄}
     * {@param[in] sql}
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     */
    int32_t create(MYSQL *hdl, const std::string &sql);
    /**
     * @brief 销毁mysql_stmt实例
     */
    void destroy();
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_tiny(const char &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_tiny(const unsigned char &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_short(const int16_t &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_short(const uint16_t &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_long(const int32_t &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_long(const uint32_t &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_longlong(const int64_t &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_longlong(const uint64_t &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_float(const float &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_double(const double &param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     * [@note 日期格式: HH:MM:SS.SSSSSS 或 HH:MM:SS]
     */
    void set_param_time(buffer_field *param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     * [@note 日期格式: YYYYMMDD]
     */
    void set_param_date(buffer_field *param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     * [@note 日期格式: YYYYMMDD_HH:MM:SS.SSSSSS]
     */
    void set_param_datetime(buffer_field *param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     * [@note 日期格式: YYYYMMDD_HH:MM:SS.SSSSSS]
     */
    void set_param_timestamp(buffer_field *param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     * [@note TEXT, CHAR, VARCHAR]
     */
    void set_param_string(buffer_field *param, bool isnull = false);
    /**
     * @brief 设置参数
     *
     * {@param[in] param 参数}
     * {@param[in] isnull 是否为null}
     */
    void set_param_blob(buffer_field *param, bool isnull = false);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_tiny(char &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_tiny(unsigned char &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_short(int16_t &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_short(uint16_t &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_long(int32_t &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_long(uint32_t &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_longlong(int64_t &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_longlong(uint64_t &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_float(float &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_double(double &param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     * [@note 拼写sql时需要使用TIME_FORMAT]
     */
    void set_result_time(buffer_field *param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     * [@note 拼写sql时需要使用DATE_FORMAT]
     */
    void set_result_date(buffer_field *param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     * [@note 拼写sql时需要使用DATE_FORMAT]
     */
    void set_result_datetime(buffer_field *param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     * [@note 拼写sql时需要使用DATE_FORMAT]
     */
    void set_result_timestamp(buffer_field *param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     * [@note TEXT, CHAR, VARCHAR]
     */
    void set_result_string(buffer_field *param);
    /**
     * @brief 设置结果缓冲区
     *
     * {@param[in] param 参数}
     */
    void set_result_blob(buffer_field *param);
    /**
     * @brief 执行
     *
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note 调用此方法前需要先设置参数和结果，如果需要的话]
     */
    int32_t execute();
    /**
     * @brief 从结果集取1行
     *
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     */
    int32_t fetch();
    /**
     * @brief 清空结果集
     */
    void free_result();
    /**
     * @brief 错误码
     *
     * {@return uint32_t}
     * {@retval 错误码}
     */
    uint32_t code();
    /**
     * @brief 错误信息
     *
     * {@return const char *}
     * {@retval 错误信息}
     */
    const char *msg();
    /**
     * @brief 被sql影响的行数
     *
     * {@return uint32_t}
     * {@retval 行数}
     */
    uint32_t affect_rows() {return rows_;}
private:
    /**
     * @brief 恢复stmt
     *
     * {@return int32_t}
     * {@retval 0 succ; -1 fail}
     * [@note 用于自动重连，自动重连成功后，prepare成功的statement会被关闭，通过此接口恢复]
     */
    int32_t recover_stmt();

private:
    binds bind_param_;
    binds bind_result_;
    MYSQL_STMT *stmt_ = {nullptr};
    uint32_t rows_ = {0};
    std::string sql_;
    MYSQL *hdl_ = {nullptr};
    uint64_t current_thread_id_ = {0};
    bool server_gone_ = {false};
    std::vector<std::pair<buffer_field *, MYSQL_TIME *>> time_bridge_;
    std::vector<std::pair<buffer_field *, MYSQL_BIND *>> buf_bridge_;
};


#endif  ///< _MYSQL_CLIENT_INTERFACE_H_