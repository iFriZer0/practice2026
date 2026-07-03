#ifndef QT_MAIN_VIEW_H__

#define QT_MAIN_VIEW_H__

#include <memory>
#include "view.h"
#include "main_window.h"

class QtMainView : public View
{
public:
    explicit QtMainView(
            const std::shared_ptr<View> &view_rs_485,
            const std::shared_ptr<View> &view_mko,
            const std::shared_ptr<View> &view_pku,
            QStackedWidget *const stacked_widget
    );
    QtMainView(const QtMainView &other) = delete;
    QtMainView(QtMainView &&other) = delete;

    ~QtMainView() = default;

    QtMainView &operator=(const QtMainView &other) = delete;
    QtMainView &operator=(QtMainView &&other) = delete;

    void show() override;
private:
    MainWindow window;
};

#endif
