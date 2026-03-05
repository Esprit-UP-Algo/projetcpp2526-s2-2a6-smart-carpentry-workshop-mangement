#include "ajout_fab.h"
#include "ui_ajout_fab.h"
#include <QMessageBox>

Ajout_Fab::Ajout_Fab(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog_Ajout_Fabrication)
{
    ui->setupUi(this);
}

Ajout_Fab::~Ajout_Fab()
{
    delete ui;
}

void Ajout_Fab::on_btn_ajouter_clicked()
{
    QString id_fab   = ui->input_id_fabrication->text();
    QString modele   = ui->input_modele->text();
    QString date     = ui->input_date_debut->date().toString("dd/MM/yyyy");

    // Vérifier que les champs ne sont pas vides
    if (id_fab.isEmpty() || modele.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs !");
        return;
    }

    // Ici vous faites ce que vous voulez avec les données
    qDebug() << "ID:" << id_fab << "| Modèle:" << modele << "| Date:" << date;

    accept(); // Ferme le popup
}
