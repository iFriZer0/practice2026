#ifndef QT_MAIN_VIEW_H__

#define QT_MAIN_VIEW_H__

#include "view.h"
#include "main_window.h"

class QtMainView : public View
{
public:
    explicit QtMainView();
    explicit QtMainView(const QtMainView &other);
    explicit QtMainView(QtMainView &&other) noexcept;

    ~QtMainView() = default;

    QtMainView &operator=(const QtMainView &other);
    QtMainView &operator=(QtMainView &&other) noexcept;

    void show() override;
private:
    MainWindow window;
};

#endif
