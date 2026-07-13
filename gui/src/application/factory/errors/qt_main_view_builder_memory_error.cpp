#include <typeinfo>
#include "qt_main_view_builder_memory_error.h"
#include "view_builder_error.h"

QtMainViewBuilderMemoryError::QtMainViewBuilderMemoryError(const char *const message, const std::type_info &first_error) noexcept
    : ViewBuilderError(message, first_error) {}

const void *QtMainViewBuilderMemoryError::get_data() const noexcept
{
    return nullptr;
}
