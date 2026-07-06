#ifndef QT_VIEW_MKO_H__

#define QT_VIEW_MKO_H__

#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include "view.h"

class QtViewMKO : public View
{
public:
    explicit QtViewMKO(QStackedWidget *const stacked_widget) noexcept;
    QtViewMKO(const QtViewMKO &other) = delete;
    QtViewMKO(QtViewMKO &&other) = delete;

    ~QtViewMKO() = default;

    QtViewMKO &operator=(const QtViewMKO &other) = delete;
    QtViewMKO &operator=(QtViewMKO &&other) = delete;

    void show() override;
private:
    static constexpr int NOT_FOUND{-1};

    QWidget *central_widget;
    QStackedWidget *stacked_widget;

    QWidget *create_widget() const;

    QVBoxLayout *create_v_box_layout(QWidget *const parent) const;

    QLabel *create_label(const QString &text) const;
};

#endif
