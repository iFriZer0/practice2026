#include <typeinfo>
#include "simple_creator_memory_error.h"
#include "simple_creator_error.h"

SimpleCreatorMemoryError::SimpleCreatorMemoryError(const char *const message, const std::type_info &first_error) noexcept
    : SimpleCreatorError(message, first_error, nullptr) {}
