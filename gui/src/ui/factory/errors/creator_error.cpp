#include <typeinfo>
#include <any>
#include "creator_error.h"
#include "error.h"

CreatorError::CreatorError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : Error(message, first_error, data) {}
