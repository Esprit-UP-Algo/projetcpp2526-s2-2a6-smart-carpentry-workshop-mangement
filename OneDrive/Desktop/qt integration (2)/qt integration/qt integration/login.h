#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog> // On utilise QDialog
#include "mainwindow.h"

namespace Ui {
class Login;
}

class Login : public QDialog // Doit être QDialog ici
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_btn_signin_clicked();

private:
    Ui::Login *ui;
    MainWindow *mainWin;
};

#endif // LOGIN_H
