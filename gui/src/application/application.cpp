#include <iostream>
#include <memory>
#include <utility>
#include <QApplication>
#include "application.h"
#include "qt_main_view_builder.h"
#include "simple_creator.h"
#include "simple_creator_maker.h"
#include "view.h"
#include "view_builder.h"
#include "view_director.h"

int Application::start(int argc, char **argv) noexcept
{
    QApplication application{argc, argv};
    std::unique_ptr<View> view;
    int error;
    try
    {
        view = view_solution.make(BUILT)->create(*builder_solution.make(QT)->create());
    }
    catch (const Error &exception)
    {
        std::cerr << "Не удалось запустить программу: " << exception.what() << std::endl;
        error = ERROR;
    }
    view->show();
    error = application.exec();
    return error;
}

const Solution<ViewBuilder, Application::ViewBuilders> Application::builder_solution
{
    std::make_pair(
                QT,
                std::make_shared<SimpleCreatorMaker<ViewBuilder, SimpleCreator<ViewBuilder, QtMainViewBuilder>>>()
    )
};

const Solution<View, Application::Views, ViewBuilder &> Application::view_solution
{
    std::make_pair(
                BUILT,
                std::make_shared<SimpleCreatorMaker<View, ViewDirector, ViewBuilder &>>()
    )
};
