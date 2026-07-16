#include <typeinfo>
#include "solution_memory_error.h"
#include "solution_error.h"

SolutionMemoryError::SolutionMemoryError(const char *const message, const std::type_info &first_error) noexcept
    : SolutionError(message, first_error) {}

const void *SolutionMemoryError::get_data() const noexcept
{
    return nullptr;
}
