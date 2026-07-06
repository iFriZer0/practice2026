#include <memory>
#include <new>
#include <utility>
#include <QStackedWidget>
#include "qt_main_view_builder.h"
#include "qt_main_view.h"
#include "qt_main_view_builder_factory_error.h"
#include "qt_main_view_builder_memory_error.h"
#include "view.h"
#include "simple_creator_maker.h"
#include "simple_creator.h"
#include "qt_view_rs_485.h"
#include "qt_view_mko.h"
#include "qt_view_pku.h"

std::unique_ptr<View> QtMainViewBuilder::build_view()
{
    QStackedWidget *stacked_widget{create_stacked_widget()};
    std::unique_ptr<View> view;
    try
    {
        view = main_view_solution.make(QT)->create(
                    view_solution.make(RS_485)->create(std::move(stacked_widget)),
                    view_solution.make(MKO)->create(std::move(stacked_widget)),
                    view_solution.make(PKU)->create(std::move(stacked_widget)),
                    std::move(stacked_widget)
        );
    }
    catch (const Error &error)
    {
        throw QtMainViewBuilderFactoryError{error.what(), error.get_first_error(), error.get_data()};
    }
    return view;
}

const Solution<
        View,
        QtMainViewBuilder::MainViews,
        const std::shared_ptr<View> &,
        const std::shared_ptr<View> &,
        const std::shared_ptr<View> &,
        QStackedWidget *const
> QtMainViewBuilder::main_view_solution
{
    std::make_pair(
                QT,
                std::make_shared<SimpleCreatorMaker<View, SimpleCreator<View, QtMainView, const std::shared_ptr<View> &, const std::shared_ptr<View> &, const std::shared_ptr<View> &, QStackedWidget *const>, const std::shared_ptr<View> &, const std::shared_ptr<View> &, const std::shared_ptr<View> &, QStackedWidget *const>>()
    )
};

const Solution<View, QtMainViewBuilder::Views, QStackedWidget *const> QtMainViewBuilder::view_solution
{
    std::make_pair(
                RS_485,
                std::make_shared<SimpleCreatorMaker<View, SimpleCreator<View, QtViewRS485, QStackedWidget *const>, QStackedWidget *const>>()
    ),
    std::make_pair(
                MKO,
                std::make_shared<SimpleCreatorMaker<View, SimpleCreator<View, QtViewMKO, QStackedWidget *const>, QStackedWidget *const>>()
    ),
    std::make_pair(
                PKU,
                std::make_shared<SimpleCreatorMaker<View, SimpleCreator<View, QtViewPKU, QStackedWidget *const>, QStackedWidget *const>>()
    )
};

QStackedWidget *QtMainViewBuilder::create_stacked_widget() const
{
    QStackedWidget *stacked_widget{nullptr};
    try
    {
        stacked_widget = new QStackedWidget{};
    }
    catch (const std::bad_alloc &)
    {
        throw QtMainViewBuilderMemoryError{"Memory allocation failed.", typeid(QtMainViewBuilderMemoryError)};
    }
    return stacked_widget;
}
