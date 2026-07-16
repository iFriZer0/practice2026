#include <typeinfo>
#include "view_director_build_error.h"
#include "view_director_error.h"

ViewDirectorBuildError::ViewDirectorBuildError(const char *const message, const std::type_info &first_error) noexcept
    : ViewDirectorError(message, first_error) {}

const void *ViewDirectorBuildError::get_data() const noexcept
{
    return nullptr;
}
