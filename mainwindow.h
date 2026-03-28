#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDate>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QModelIndex>
#include <QTableView>

#include "modele.h"
#include "personnel.h"

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
    // ── Slots Modeles ─────────────────────────────────────────────────────
    void ligneSelectionnee(int row, int col);
    void on_btn_ajouter_modele_clicked();
    void on_btn_modifier_modele_clicked();
    void on_btn_supprimer_modele_clicked();
    void on_btn_rechercher_modele_clicked();
    void on_btn_tire_clicked();
    void on_btn_export_pdf_modele_clicked();

    // ── Slots Personnel ───────────────────────────────────────────────────
    void on_btn_ajouter_3_clicked();
    void on_btn_modifier_2_clicked();
    void on_btn_supprimer_2_clicked();
    void on_tab_employes_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    // ID du modele selectionne (-1 = aucun)
    int m_idSelectionne = -1;

    // ── Methodes Modeles ──────────────────────────────────────────────────
    QSqlDatabase db() const;
    void chargerTableauModeles();
    bool validerFormulaire();
    void reinitialiserFormulaire();
    void afficherStatistiques();

    // ── Methodes Personnel ────────────────────────────────────────────────
    void refreshTable();
    void clearFields();
    bool controlDeSaisie();
};

#endif // MAINWINDOW_H
