#include "mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *window_boss) : QWidget(window_boss) {// Создаем элементы
    inputField = new QLineEdit(this);// поле ввод текста
    inputField->setPlaceholderText("Введите ID платы...");

    sendButton = new QPushButton("Отправить запрос", this);
    resultLabel = new QLabel("Ответ", this);

//расставляю их друг под другом
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(inputField);
    layout->addWidget(sendButton);
    layout->addWidget(resultLabel);
    setLayout(layout);

 //размер и заголовок окна
    setWindowTitle("Тест gRPC");
    resize(350, 150);


    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
}

MainWindow::~MainWindow() {
}

//сработает, когда пользователь нажмет на кнопку
void MainWindow::onSendButtonClicked() {
    QString id = inputField->text(); //беру текст из поля ввода

    if (id.isEmpty()) {
        resultLabel->setText("Ошибка: введите ID!");
    } else {
        resultLabel->setText("Отправка запроса для ID: " + id);
    }
}
