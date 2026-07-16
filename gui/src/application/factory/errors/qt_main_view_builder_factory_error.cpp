#include <typeinfo>
#include "qt_main_view_builder_factory_error.h"
#include "view_builder_error.h"

QtMainViewBuilderFactoryError::QtMainViewBuilderFactoryError(const char *const message, const std::type_info &first_error) noexcept
    : ViewBuilderError(message, first_error) {}


const void *QtMainViewBuilderFactoryError::get_data() const noexcept
{
    return nullptr;
}
