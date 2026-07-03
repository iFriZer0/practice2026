#ifndef APPLICATION_H__

#define APPLICATION_H__

#include "solution.h"
#include "view.h"
#include "view_builder.h"

class Application
{
public:
    static int start(int argc, char **argv) noexcept;
private:
    static constexpr int ERROR{1};

    enum ViewBuilders
    {
        QT
    };

    enum Views
    {
        BUILT
    };

    static const Solution<ViewBuilder, ViewBuilders> builder_solution;

    static const Solution<View, Views, ViewBuilder &> view_solution;
};

#endif
