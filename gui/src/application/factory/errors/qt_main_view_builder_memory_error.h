#ifndef QT_MAIN_VIEW_BUILDER_MEMORY_ERROR_H__

#define QT_MAIN_VIEW_BUILDER_MEMORY_ERROR_H__

#include <typeinfo>
#include "view_builder_error.h"

class QtMainViewBuilderMemoryError : public ViewBuilderError
{
public:
    QtMainViewBuilderMemoryError(const char *const message, const std::type_info &first_error) noexcept;
};

#endif
