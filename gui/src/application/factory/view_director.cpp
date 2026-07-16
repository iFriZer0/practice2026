#include <memory>
#include "view_director.h"
#include "view.h"
#include "view_builder.h"
#include "view_builder_error.h"
#include "view_director_build_error.h"

std::unique_ptr<View> ViewDirector::create(ViewBuilder &builder) const
{
    try
    {
        return builder.build_view();
    }
    catch (const ViewBuilderError &error)
    {
        throw ViewDirectorBuildError{error.what(), error.get_first_error()};
    }
}
