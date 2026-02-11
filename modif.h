#ifndef MODIF_H
#define MODIF_H

#include <QDialog>

namespace Ui {
class modif;
}

class modif : public QDialog
{
    Q_OBJECT

public:
    explicit modif(QWidget *parent = nullptr);
    ~modif();

private:
    Ui::modif *ui;
};

#endif // MODIF_H
