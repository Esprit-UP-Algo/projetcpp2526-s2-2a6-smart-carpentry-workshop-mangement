#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent), // <--- CHANGE CECI : Mets QDialog à la place de QWidget
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // Ton bouton s'appelle btn_signin, c'est parfait
    connect(ui->btn_signin, &QPushButton::clicked, this, &Login::on_btn_signin_clicked);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btn_signin_clicked()
{
    mainWin = new MainWindow();
    mainWin->show();
    this->close();
}
