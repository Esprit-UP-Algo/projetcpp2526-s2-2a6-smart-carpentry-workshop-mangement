#include "modifier_fab.h"
#include "ui_modifier_fab.h"
#include <QMessageBox>

Modifier_Fab::Modifier_Fab(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog_Modifier_Fabrication)
{
    ui->setupUi(this);
}

Modifier_Fab::~Modifier_Fab()
{
    delete ui;
}

// Pré-remplir les champs avec les données sélectionnées
void Modifier_Fab::setData(const QString &id, const QString &modele, const QDate &date)
{
    ui->input_id_fabrication->setText(id);
    ui->input_modele->setText(modele);
    ui->input_date_debut->setDate(date);
}

void Modifier_Fab::on_btn_modifier_clicked()
{
    QString id_fab = ui->input_id_fabrication->text();
    QString modele = ui->input_modele->text();
    QString date   = ui->input_date_debut->date().toString("dd/MM/yyyy");

    if (id_fab.isEmpty() || modele.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs !");
        return;
    }

    qDebug() << "Modifié => ID:" << id_fab << "| Modèle:" << modele << "| Date:" << date;

    accept(); // Ferme le popup
}
