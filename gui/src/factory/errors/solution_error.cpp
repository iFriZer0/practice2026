#include <any>
#include <typeinfo>
#include "solution_error.h"
#include "error.h"

SolutionError::SolutionError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept
    : Error(message, first_error, data) {}
