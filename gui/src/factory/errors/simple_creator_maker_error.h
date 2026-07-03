#ifndef SIMPLE_CREATOR_MAKER_ERROR_H__

#define SIMPLE_CREATOR_MAKER_ERROR_H__

#include <any>
#include <typeinfo>
#include "creator_maker_error.h"

class SimpleCreatorMakerError : public CreatorMakerError
{
public:
    SimpleCreatorMakerError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept;
};

#endif
