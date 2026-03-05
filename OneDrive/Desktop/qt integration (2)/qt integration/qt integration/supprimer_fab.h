#ifndef SUPPRIMER_FAB_H
#define SUPPRIMER_FAB_H

#include <QDialog>

namespace Ui {
class Dialog_Supprimer_Fabrication;
}

class Supprimer_Fab : public QDialog
{
    Q_OBJECT

public:
    explicit Supprimer_Fab(QWidget *parent = nullptr);
    ~Supprimer_Fab();

private:
    Ui::Dialog_Supprimer_Fabrication *ui;
};

#endif
