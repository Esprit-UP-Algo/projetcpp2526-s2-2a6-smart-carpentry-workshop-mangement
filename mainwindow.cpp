#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // --- Navigation des 5 modules ---

    // 1. Bois (Page 0)
    connect(ui->btn_bois, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    // 2. Modèle (Page 1)
    connect(ui->btn_modele, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(1);
    });

    // 3. Personnel (Page 2)
    connect(ui->btn_personnel, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(2);
    });

    // 4. Étape (Page 3)
    connect(ui->btn_etape, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(3);
    });

    // 5. Fabrication (Page 4)
    connect(ui->btn_fabrication, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(4);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
