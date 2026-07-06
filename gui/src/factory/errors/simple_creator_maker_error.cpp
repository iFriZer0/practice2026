#include <typeinfo>
#include <any>
#include "simple_creator_maker_error.h"
#include "creator_maker_error.h"

SimpleCreatorMakerError::SimpleCreatorMakerError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : CreatorMakerError(message, first_error, data) {}
