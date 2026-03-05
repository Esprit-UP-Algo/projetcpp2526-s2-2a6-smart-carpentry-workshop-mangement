#ifndef MODIFIER_FAB_H
#define MODIFIER_FAB_H

#include <QDialog>

namespace Ui {
class Dialog_Modifier_Fabrication;
}

class Modifier_Fab : public QDialog
{
    Q_OBJECT

public:
    explicit Modifier_Fab(QWidget *parent = nullptr);
    ~Modifier_Fab();

    // Pour pré-remplir les champs avec les données existantes
    void setData(const QString &id, const QString &modele, const QDate &date);

private slots:
    void on_btn_modifier_clicked();

private:
    Ui::Dialog_Modifier_Fabrication *ui;
};

#endif
