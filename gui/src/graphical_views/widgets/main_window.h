#ifndef MAIN_WINDOW_H__

#define MAIN_WINDOW_H__

#include <memory>
#include <unordered_map>
#include <QWidget>
#include <QMainWindow>
#include <QTabBar>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "view.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(
            const std::shared_ptr<View> &view_rs_485,
            const std::shared_ptr<View> &view_mko,
            const std::shared_ptr<View> &view_pku,
            QStackedWidget *const stacked_widget,
            QWidget *parent = nullptr
    );

    ~MainWindow() = default;
private:
    static constexpr int TAB_RS_485{0};
    static constexpr int TAB_MKO{1};
    static constexpr int TAB_PKU{2};

    QTabBar *tab_bar;
    QStackedWidget *stacked_widget;

    std::unordered_map<int, std::shared_ptr<View>> views;

    QTabBar *create_tab_bar() const;

    QWidget *create_widget() const;

    QVBoxLayout *create_v_box_layout(QWidget *const parent) const;
private slots:
    void switch_tab(int index);
};

#endif
