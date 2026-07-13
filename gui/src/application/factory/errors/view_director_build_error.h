#ifndef VIEW_DIRECTOR_BUILD_ERROR_H__

#define VIEW_DIRECTOR_BUILD_ERROR_H__

#include <typeinfo>
#include "view_director_error.h"

class ViewDirectorBuildError : public ViewDirectorError
{
public:
    ViewDirectorBuildError(const char *const message, const std::type_info &first_error) noexcept;

    const void *get_data() const noexcept override;
};

#endif
