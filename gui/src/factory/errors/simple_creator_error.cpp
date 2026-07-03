#include <typeinfo>
#include <any>
#include "simple_creator_error.h"
#include "creator_error.h"

SimpleCreatorError::SimpleCreatorError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : CreatorError(message, first_error, data) {}
