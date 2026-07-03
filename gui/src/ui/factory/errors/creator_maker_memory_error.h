#ifndef CREATOR_MAKER_MEMORY_ERROR_H__

#define CREATOR_MAKER_MEMORY_ERROR_H__

#include <any>
#include <typeinfo>
#include "creator_maker_error.h"

class CreatorMakerMemoryError : public CreatorMakerError
{
public:
    CreatorMakerMemoryError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept;
};

#endif
