#include <any>
#include <typeinfo>
#include "creator_maker_error.h"
#include "error.h"

CreatorMakerError::CreatorMakerError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : Error(message, first_error, data) {}
