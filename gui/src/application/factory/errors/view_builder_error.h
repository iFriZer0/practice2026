#ifndef VIEW_BUILDER_ERROR_H__

#define VIEW_BUILDER_ERROR_H__

#include <any>
#include <typeinfo>
#include "error.h"

class ViewBuilderError : public Error
{
public:
    ViewBuilderError(const char *const message, const std::type_info &first_error, const std::any &data) noexcept;
};

#endif
