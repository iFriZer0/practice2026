#ifndef VIEW_BUILDER_H__

#define VIEW_BUILDER_H__

#include <memory>
#include "view.h"

class ViewBuilder
{
public:
    virtual ~ViewBuilder() = default;

    virtual std::unique_ptr<View> build_view() = 0;
};

#endif
