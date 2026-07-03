#ifndef SOLUTION_NO_MAKER_ERROR_H__

#define SOLUTION_NO_MAKER_ERROR_H__

#include <typeinfo>
#include "solution_error.h"

template<typename Identifier>
class SolutionNoMakerError : public SolutionError
{
public:
    SolutionNoMakerError(const char *const message, const std::type_info &first_error, const Identifier &identifier) noexcept;
};

#include "solution_no_maker_error.hpp"

#endif
