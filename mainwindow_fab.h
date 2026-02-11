#ifndef MAINWINDOW_FAB_H
#define MAINWINDOW_FAB_H

#include <QMainWindow>
#include"ajout.h"
#include"modif.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;

}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_ajouter_clicked();

    void on_btn_modifier_clicked();

    void on_btn_supprimer_clicked();

private:
    Ui::MainWindow *ui;
    ajout *ajoutDialog;
    modif *modifDialog;

};
#endif // MAINWINDOW_FAB_H
