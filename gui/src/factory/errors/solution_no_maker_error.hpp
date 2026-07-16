#include "solution_no_maker_error.h"

template<typename Identifier>
requires std::copyable<Identifier>
SolutionNoMakerError<Identifier>::SolutionNoMakerError(
        const char *const message,
        const std::type_info &first_error,
        const Identifier &identifier
) noexcept
    : SolutionError(message, first_error), identifier{identifier} {}


template<typename Identifier>
requires std::copyable<Identifier>
const void *SolutionNoMakerError<Identifier>::get_data() const noexcept
{
    return &identifier;
}
