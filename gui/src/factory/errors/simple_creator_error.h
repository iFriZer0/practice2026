#ifndef SIMPLE_CREATOR_ERROR_H__

#define SIMPLE_CREATOR_ERROR_H__

#include <any>
#include <typeinfo>
#include "creator_error.h"

class SimpleCreatorError : public CreatorError
{
public:
    SimpleCreatorError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept;
};

#endif
