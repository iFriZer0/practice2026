#include <typeinfo>
#include "creator_error.h"
#include "error.h"

CreatorError::CreatorError(const char *const message, const std::type_info &first_error) noexcept
    : Error(message, first_error) {}
