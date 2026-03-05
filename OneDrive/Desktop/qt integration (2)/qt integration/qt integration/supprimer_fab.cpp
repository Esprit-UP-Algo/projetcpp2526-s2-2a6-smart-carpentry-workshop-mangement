#include "supprimer_fab.h"
#include "ui_supprimer_fab.h"

Supprimer_Fab::Supprimer_Fab(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog_Supprimer_Fabrication)
{
    ui->setupUi(this);
}

Supprimer_Fab::~Supprimer_Fab()
{
    delete ui;
}
