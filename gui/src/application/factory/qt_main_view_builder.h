#ifndef QT_MAIN_VIEW_BUILDER_H__

#define QT_MAIN_VIEW_BUILDER_H__

#include <memory>
#include <QStackedWidget>
#include "solution.h"
#include "view_builder.h"
#include "view.h"

class QtMainViewBuilder : public ViewBuilder
{
public:
    std::unique_ptr<View> build_view();
private:
    enum MainViews
    {
        QT
    };

    enum Views
    {
        RS_485,
        MKO,
        PKU
    };

    static const Solution<
        View, MainViews, const std::shared_ptr<View> &, const std::shared_ptr<View> &, const std::shared_ptr<View> &, QStackedWidget *const
    > main_view_solution;

    static const Solution<View, Views, QStackedWidget *const> view_solution;

    QStackedWidget *create_stacked_widget() const;
};

#endif
