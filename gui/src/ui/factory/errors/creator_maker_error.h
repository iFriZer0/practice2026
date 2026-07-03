#ifndef CREATOR_MAKER_ERROR_H__

#define CREATOR_MAKER_ERROR_H__

#include <any>
#include <typeinfo>
#include "error.h"

class CreatorMakerError : public Error
{
public:
    CreatorMakerError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept;
};

#endif
