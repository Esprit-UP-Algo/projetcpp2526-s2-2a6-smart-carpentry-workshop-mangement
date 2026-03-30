#include "mainwindow_fab.h"
#include "ui_mainwindow_fab.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    ajoutDialog(nullptr),
    modifDialog(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_btn_ajouter_clicked(){
    ajoutDialog = new ajout(this);
    ajoutDialog->exec();
}

void MainWindow::on_btn_modifier_clicked()
{
    modifDialog = new modif(this);
    modifDialog->exec();
}



