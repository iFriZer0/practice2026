#include <typeinfo>
#include "view_director_error.h"
#include "error.h"

ViewDirectorError::ViewDirectorError(const char *const message, const std::type_info &first_error) noexcept
    : Error(message, first_error) {}
