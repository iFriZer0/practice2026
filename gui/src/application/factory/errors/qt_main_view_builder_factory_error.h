#ifndef QT_MAIN_VIEW_BUILDER_FACTORY_ERROR_H__

#define QT_MAIN_VIEW_BUILDER_FACTORY_ERROR_H__

#include <typeinfo>
#include "view_builder_error.h"

class QtMainViewBuilderFactoryError : public ViewBuilderError
{
public:
    QtMainViewBuilderFactoryError(const char *const message, const std::type_info &first_error) noexcept;

    const void *get_data() const noexcept override;
};

#endif
