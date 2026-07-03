#include <any>
#include <typeinfo>
#include "simple_creator_maker_memory_error.h"
#include "simple_creator_maker_error.h"

SimpleCreatorMakerMemoryError::SimpleCreatorMakerMemoryError(const char *const message, const std::type_info &first_error) noexcept
    : SimpleCreatorMakerError(message, first_error, nullptr) {}
