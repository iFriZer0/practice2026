#include <memory>
#include <QStackedWidget>
#include "qt_main_view.h"
#include "view.h"

QtMainView::QtMainView(
        const std::shared_ptr<View> &view_rs_485,
        const std::shared_ptr<View> &view_mko,
        const std::shared_ptr<View> &view_pku,
        QStackedWidget *const stacked_widget

)
    : window{view_rs_485, view_mko, view_pku, stacked_widget} {}

void QtMainView::show()
{
    window.show();
}
