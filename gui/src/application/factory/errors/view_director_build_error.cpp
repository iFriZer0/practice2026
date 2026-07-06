#include <typeinfo>
#include <any>
#include "view_director_build_error.h"
#include "view_director_error.h"

ViewDirectorBuildError::ViewDirectorBuildError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : ViewDirectorError(message, first_error, data) {}
