#include <typeinfo>
#include <any>
#include "view_builder_error.h"
#include "error.h"

ViewBuilderError::ViewBuilderError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : Error(message, first_error, data) {}
