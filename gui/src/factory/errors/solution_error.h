#ifndef SOLUTION_ERROR_H__

#define SOLUTION_ERROR_H__

#include <any>
#include <typeinfo>
#include "error.h"

class SolutionError : public Error
{
public:
    SolutionError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept;
};

#endif
