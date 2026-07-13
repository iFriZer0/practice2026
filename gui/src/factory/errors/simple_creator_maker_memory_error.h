#ifndef SIMPLE_CREATOR_MAKER_MEMORY_ERROR_H__

#define SIMPLE_CREATOR_MAKER_MEMORY_ERROR_H__

#include <typeinfo>
#include "simple_creator_maker_error.h"

class SimpleCreatorMakerMemoryError : public SimpleCreatorMakerError
{
public:
    SimpleCreatorMakerMemoryError(const char *const message, const std::type_info &first_error) noexcept;

    const void *get_data() const noexcept override;
};

#endif
