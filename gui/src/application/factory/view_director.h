#ifndef VIEW_DIRECTOR_H__

#define VIEW_DIRECTOR_H__

#include <memory>
#include "creator.h"
#include "view.h"
#include "view_builder.h"

class ViewDirector : public Creator<View, ViewBuilder &>
{
public:
    std::unique_ptr<View> create(ViewBuilder &builder) const override;
};

#endif
