#include <any>
#include <typeinfo>
#include "creator_maker_memory_error.h"
#include "creator_maker_error.h"

CreatorMakerMemoryError::CreatorMakerMemoryError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : CreatorMakerError(message, first_error, data) {}
