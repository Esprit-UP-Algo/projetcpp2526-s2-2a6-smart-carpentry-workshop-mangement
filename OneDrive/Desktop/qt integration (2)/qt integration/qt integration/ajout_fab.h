#ifndef AJOUT_FAB_H
#define AJOUT_FAB_H

#include <QDialog>

namespace Ui {
class Dialog_Ajout_Fabrication;
}

class Ajout_Fab : public QDialog
{
    Q_OBJECT

public:
    explicit Ajout_Fab(QWidget *parent = nullptr);
    ~Ajout_Fab();

private slots:
    void on_btn_ajouter_clicked();

private:
    Ui::Dialog_Ajout_Fabrication *ui;
};

#endif
