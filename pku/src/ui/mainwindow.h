#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QWidget {
    Q_OBJECT //для использования сигналов и кнопок

public:
    MainWindow(QWidget *window_boss = nullptr);//типа главное окно
    ~MainWindow();

private slots://функция будет срабатывать, когда пользователь нажмет на кнопку
    void onSendButtonClicked();

private:
    QLineEdit *inputField;     // Поле, куда вводить текст запроса
    QPushButton *sendButton;   // Кнопка отправить
    QLabel *resultLabel;       // Текст ответ
};

#endif // MAINWINDOW_H
