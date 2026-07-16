#ifndef SOLUTION_MEMORY_ERROR_H__

#define SOLUTION_MEMORY_ERROR_H__

#include <typeinfo>
#include "solution_error.h"

class SolutionMemoryError : public SolutionError
{
public:
    SolutionMemoryError(const char *const message, const std::type_info &first_error) noexcept;

    const void *get_data() const noexcept override;
};

#endif
