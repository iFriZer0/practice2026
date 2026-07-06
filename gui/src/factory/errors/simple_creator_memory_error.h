#ifndef SIMPLE_CREATOR_MEMORY_ERROR_H__

#define SIMPLE_CREATOR_MEMORY_ERROR_H__

#include <typeinfo>
#include "simple_creator_error.h"

class SimpleCreatorMemoryError : public SimpleCreatorError
{
public:
    SimpleCreatorMemoryError(const char *const message, const std::type_info &first_error) noexcept;
};

#endif
