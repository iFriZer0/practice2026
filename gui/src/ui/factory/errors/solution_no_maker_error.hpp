#include "solution_no_maker_error.h"

template<typename Identifier>
SolutionNoMakerError<Identifier>::SolutionNoMakerError(
        const char *const message,
        const std::type_info &first_error,
        const Identifier &identifier
) noexcept
    : SolutionError(message, first_error, identifier) {}
