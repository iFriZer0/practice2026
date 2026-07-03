#include <cstring>
#include <any>
#include <typeinfo>
#include "error.h"

Error::Error(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : first_error{first_error}, data{data}
{
    strncpy(this->message, message, MAXIMUM_SIZE);
}

const char *Error::what() const noexcept
{
    return message;
}

const std::type_info &Error::get_first_error() const noexcept
{
    return first_error;
}

std::any Error::get_data() const noexcept
{
    return data;
}
