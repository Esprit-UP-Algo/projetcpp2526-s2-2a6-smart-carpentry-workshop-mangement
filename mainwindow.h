#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "personnel.h" // <--- Assurez-vous que c'est bien inclus ici
#include "bois.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_ajouter_3_clicked();
    void on_btn_modifier_2_clicked();
    void on_btn_supprimer_2_clicked();
    void on_tab_employes_clicked(const QModelIndex &index);

    // Slots Bois
    void on_la_ajouter_7_clicked();
    void on_la_modifier_7_clicked();
    void on_la_supprimer_7_clicked();
    void on_chercher_7_clicked();
    void on_tab_bois_7_clicked(const QModelIndex &index);
    void on_la_trier_7_clicked();
    void on_la_pdf_7_clicked();

private:
    Ui::MainWindow *ui;
    void refreshTable(); // Déclaration de la fonction
    void refreshBoisTable();
    bool controlDeSaisie(); // <--- Nouvelle fonction

};
#endif // MAINWINDOW_H
