#ifndef VIEW_DIRECTOR_ERROR_H__

#define VIEW_DIRECTOR_ERROR_H__

#include <typeinfo>
#include "error.h"

class ViewDirectorError : public Error
{
public:
    ViewDirectorError(const char *const message, const std::type_info &first_error) noexcept;
};

#endif
