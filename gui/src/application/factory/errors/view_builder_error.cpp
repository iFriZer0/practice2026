#include <typeinfo>
#include "view_builder_error.h"
#include "error.h"

ViewBuilderError::ViewBuilderError(const char *const message, const std::type_info &first_error) noexcept
    : Error(message, first_error) {}
