#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "fab_controller.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //F.afficher(ui->tableView);
    // --- Navigation des 5 modules ---

    // 1. Bois (Page 0)
    /*connect(ui->btn_bois, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page);
    });

    // 2. Modèle (Page 1)
    connect(ui->btn_modele, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_6);
    });

    // 3. Personnel (Page 2)
    connect(ui->btn_personnel, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_5);
    });

    // 4. Étape (Page 3)
    connect(ui->btn_etape, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_7);
    });

    // 5. Fabrication (Page 4)
    connect(ui->btn_fabrication, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_4);

    });*/
}
/* connect(ui->btn_personnel, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_5);
    }); */
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_ajouter_2_clicked()
{
    int id = ui->id_fab->text().toInt();
    int idmodele = ui->id_model->text().toInt();

    QDate datedebut = ui->date_deb->date();

    QString type_bois = ui->type_bois->text();
    QDate datefin = ui->date_fin->date();
    QDate datefinreel = ui->date_fin_reel->date();

    QString ETATMAINTENANCE = ui->etat->text();

    fab_controller f(id,idmodele,datedebut,type_bois,datefin,datefinreel,
                     ETATMAINTENANCE);

    bool test = f.add();

    if(test)
    {
        //F.afficher(ui->tableView);
        QMessageBox::information(nullptr,"Success","Ajout effectué.");
    }
    else
    {
        QMessageBox::critical(nullptr,"Error","Ajout non effectué.");
    }
}


void MainWindow::on_btn_modifier_3_clicked()
{
    int id = ui->id_fab->text().toInt();

    f.setIDMODELE(ui->id_model->text().toInt());
    f.setDATEDEBUT(ui->date_deb->date());
    f.setTYPE_BOIS(ui->type_bois->text());
    f.setDATEFINPREVU(ui->date_fin->date());
    f.setDATEFINREEL(ui->date_fin_reel->date());
    f.setETATFABRICATION(ui->etat->text());


    bool test = f.modify(id);

    if(test)
    {
        //F.afficher(ui->tableView);
        QMessageBox::information(nullptr,"Success","Modification effectuée.");
    }
    else
    {
        QMessageBox::critical(nullptr,"Error","Modification non effectuée.");
    }
}


void MainWindow::on_btn_supprimer_3_clicked()
{
    int id = ui->id_fab->text().toInt();

    bool test = f.delete_2(id);

    if(test)
    {
        //F.afficher(ui->tableView);
        QMessageBox::information(nullptr,"Success","Suppression effectuée.");
    }
    else
    {
        QMessageBox::critical(nullptr,"Error","Suppression non effectuée.");
    }
}


