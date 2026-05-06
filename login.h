#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "mainwindow.h" // Pour ouvrir la fenêtre principale

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_btn_login_clicked(); // Le slot du bouton
    void on_btn_reset_clicked();

private:
    Ui::Login *ui;
    MainWindow *w; // Pointeur vers la fenêtre principale
};

#endif // LOGIN_H
