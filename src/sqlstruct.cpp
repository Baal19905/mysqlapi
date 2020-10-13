#include "sqlstruct.h"
#include <stdlib.h>
#include <string.h>


#define DEFAULT_BIND_SIZE           (128)
#define DEFAULT_BUFFARRAY_LEN       (128)

#define BIND_INSIDE_ALLOCATE(bind_elem)\
do {\
    bind_elem.is_null = (my_bool *)calloc(1, sizeof(my_bool));\
    bind_elem.error = (my_bool *)calloc(1, sizeof(my_bool));\
    if (nullptr == bind_elem.is_null ||\
        nullptr == bind_elem.error) {\
        destroy();\
        return -1;\
    }\
} while (0);

int32_t binds::create(uint32_t size)
{
    size_ = 0 == size ? DEFAULT_BIND_SIZE : size;
    pos_ = 0;
    bind_ = (MYSQL_BIND *)calloc(size_, sizeof(MYSQL_BIND));
    if (nullptr == bind_) {
        destroy();
        return -1;
    }
    time_ = (MYSQL_TIME *)calloc(size_, sizeof(MYSQL_TIME));
    if (nullptr == time_) {
        destroy();
        return -1;
    }
    for (uint32_t i = 0;i < size_; i++) {
        BIND_INSIDE_ALLOCATE(bind_[i])
    }
    return 0;
}

uint32_t binds::size()
{
    return size_;
}

int32_t binds::resize(uint32_t size)
{
    if (size < size_) {
        return -1;
    }
    uint32_t diff_size = size - size_;
    void *tmp = realloc(bind_, size * sizeof(MYSQL_BIND));
    if (nullptr == tmp) {
        return -1;
    }
    bind_ = (MYSQL_BIND *)tmp;
    memset(bind_ + size_, 0, diff_size * sizeof(MYSQL_BIND));
    for (uint32_t i = size_;i < size; i++) {
        BIND_INSIDE_ALLOCATE(bind_[i])
    }
    tmp = realloc(time_, size * sizeof(MYSQL_TIME));
    if (nullptr == tmp) {
        return -1;
    }
    time_ = (MYSQL_TIME *)tmp;
    memset(time_ + size_, 0, diff_size * sizeof(MYSQL_TIME));
    size_ = size;
    return 0;
}

void binds::rewind()
{
    pos_ = 0;
}

MYSQL_BIND *binds::head()
{
    if (nullptr == bind_) {
        return nullptr;
    }
    return &(bind_[0]);
}

int32_t binds::next(MYSQL_BIND *&bind)
{ 
    if (pos_ >= size_ || nullptr == bind_) {
        return -1;
    }
    bind = &(bind_[pos_]);
    return pos_++;
}

uint32_t binds::pos()
{
    return pos_;
}

MYSQL_TIME *binds::time_by_pos(uint32_t pos)
{
    if (pos >= size_ || nullptr == time_) {
        return nullptr;
    }
    return &(time_[pos]);
}

void binds::destroy()
{
    if (bind_ != nullptr) {
        for (uint32_t i = 0; i < size_; i++) {
            free(bind_[i].is_null);
            bind_[i].is_null = nullptr;
            free(bind_[i].error);
            bind_[i].error = nullptr;
        }
        free(bind_);
        bind_ = nullptr;
    }
    if (time_ != nullptr) {
        free(time_);
        time_ = nullptr;
    }
}


int32_t result_set::create(uint32_t size)
{
    size_ = 0 == size ? 1 : size;
    result_sets_ = (buffer_array *)calloc(size_, sizeof(buffer_array));
    if (nullptr == result_sets_) {
        destroy();
        return -1;
    }
    for (uint32_t i = 0; i < size_; i++) {
        if (init_array(&(result_sets_[i])) == -1) {
            destroy();
            return -1;
        }
    }
    return 0;
}

void result_set::destroy()
{
    if (result_sets_ != nullptr) {
        for (uint32_t i = 0; i < size_; i++) {
            if (result_sets_[i].array != nullptr) {
                free(result_sets_[i].array);
                result_sets_[i].array = nullptr;
            }
        }
        free(result_sets_);
        result_sets_ = nullptr;
    }
}

int32_t result_set::set_field(uint32_t pos, buffer_field *buffer)
{
    if (pos >= size_) {
        uint32_t new_size = pos + 1;
        void *tmp = realloc(result_sets_, new_size * sizeof(buffer_array));
        if (nullptr == tmp) {
            return -1;
        }
        result_sets_ = (buffer_array *)tmp;
        for (uint32_t i = size_; i < new_size; i++) {
            if (init_array(&(result_sets_[i])) == -1) {
                return -1;
            }
        }
        size_ = new_size;
    }
    auto &set = result_sets_[pos];
    set.array[set.pos] = buffer;
    set.pos++;
    return 0;
}

int32_t result_set::rewind()
{
    for (uint32_t i = 0; i < size_; i++) {
        result_sets_[i].pos = 0;
    }
    result_sets_pos_ = 0;
}

int32_t result_set::next(buffer_field **&set)
{
    if (result_sets_pos_ >= size_ || nullptr == result_sets_) {
        return -1;
    }
    set = result_sets_[result_sets_pos_].array;
    return result_sets_pos_++;
}

int32_t result_set::init_array(buffer_array *array)
{
    array->array = (buffer_field **)calloc(DEFAULT_BUFFARRAY_LEN, sizeof(buffer_field*));
    if (nullptr == array->array) {
        return -1;
    }
    array->size = DEFAULT_BUFFARRAY_LEN;
    array->pos = 0;
    return 0;
}
