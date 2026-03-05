#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "fab_controller.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    fab_controller f;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_ajouter_2_clicked();

    void on_btn_modifier_3_clicked();

    void on_btn_supprimer_3_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
