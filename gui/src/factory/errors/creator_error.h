#ifndef CREATOR_ERROR_H__

#define CREATOR_ERROR_H__

#include <typeinfo>
#include "error.h"

class CreatorError : public Error
{
public:
    CreatorError(const char *const message, const std::type_info &first_error) noexcept;
};

#endif
