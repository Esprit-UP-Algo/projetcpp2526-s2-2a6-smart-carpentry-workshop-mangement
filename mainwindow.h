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
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include <QPrinter>
#include <QFileDialog>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QHeaderView>
#include <QTreeWidget>
#include <QTableWidget>

#include "modele.h"
#include "personnel.h"
#include "bois.h"

// Forward declaration of Ui namespace
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// ─────────────────────────────────────────────────────────────
//  Étape de fabrication (onglet Suivi)
// ─────────────────────────────────────────────────────────────
struct EtapeSuivi {
    int     id;
    QString nom;
    int     ordre;
    int     tempsEstime;
    int     tempsReel;
    QDate   dateDebut;
    QDate   dateFin;
    QString statut;   // "Planifié" | "En cours" | "Non planifié"
    QString cin;      // Optionnel
};

// ─────────────────────────────────────────────────────────────
//  Fabrication (onglet Suivi)
// ─────────────────────────────────────────────────────────────
struct FabricationSuivi {
    int     id;
    QString modele;
    int     quantite;
    QString qualite;
    QDate   dateDebut;
    QList<EtapeSuivi> etapes;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ── Navigation ───────────────────────────────────────────────────────
    void on_btn_personnel_clicked();
    void on_btn_modele_clicked();
    void on_btn_bois_clicked();
    void on_btn_etape_clicked();
    void on_btn_fabrication_clicked();

    // ── Modeles ───────────────────────────────────────────────────────────
    void ligneSelectionnee(int row, int col);
    void on_btn_ajouter_modele_clicked();
    void on_btn_modifier_modele_clicked();
    void on_btn_supprimer_modele_clicked();
    void on_btn_rechercher_modele_clicked();
    void on_btn_tire_clicked();
    void on_btn_export_pdf_modele_clicked();

    // ── Personnel ─────────────────────────────────────────────────────────
    void on_btn_ajouter_3_clicked();
    void on_btn_modifier_2_clicked();
    void on_btn_supprimer_2_clicked();
    void on_tab_employes_clicked(const QModelIndex &index);

    // ── Bois ──────────────────────────────────────────────────────────────
    void on_la_ajouter_7_clicked();
    void on_la_modifier_7_clicked();
    void on_la_supprimer_7_clicked();
    void on_chercher_7_clicked();
    void on_tab_bois_7_clicked(const QModelIndex &index);
    void on_la_trier_7_clicked();
    void on_la_pdf_9_clicked();

    // ── Fabrication CRUD ───────────────────────────────────────────────────
    void on_btn_ajouter_2_clicked();
    void on_btn_modifier_clicked();
    void on_btn_supprimer_clicked();
    void on_btn_rechercher_6_clicked();
    void on_btn_tire_2_clicked();
    void on_btn_export_pdf_modele_2_clicked();
    void on_table_modeles_2_cellClicked(int row, int column);
    void on_rech_7_textChanged(const QString &text);


private:
    Ui::MainWindow *ui;
    int m_idSelectionne = -1;

    // ── Connexion ─────────────────────────────────────────────────────────
    QSqlDatabase db() const;

    // ── Modeles ───────────────────────────────────────────────────────────
    void chargerTableauModeles();
    bool validerFormulaire();
    void reinitialiserFormulaire();
    void afficherStatistiques();

    // ── Personnel ─────────────────────────────────────────────────────────
    void refreshTable();
    void clearFields();
    bool controlDeSaisie();

    // ── Bois ──────────────────────────────────────────────────────────────
    void refreshBoisTable();

    // ── Fabrication helpers ──────────────────────────────────────────────────
    void loadFabrications();
    void generateFabricationId();
    void clearFabricationFields();
    bool validateFabricationFields();
    void filterFabrications(const QString &searchText);
    void sortFabrications(const QString &criteria);
    int  generateId();
    void exportToPDF();

    void afficherTendances();
    void insererRelationsTendances();
    void afficherAnalyseCouts();
    // ── UI helpers ───────────────────────────────────────────────────────────
    void showAnimatedMessageBox(QMessageBox::Icon icon,
                                const QString &title,
                                const QString &text,
                                const QString &informativeText = "");
    bool showAnimatedQuestionBox(const QString &title, const QString &text);

    // ── Données membres ──────────────────────────────────────────────────────
    FabricationSuivi  m_selectedFab;      // Fabrication actuellement sélectionnée
    bool              m_fabSelected;      // Indique si une fabrication est sélectionnée

    // Liste des IDs de toutes les fabrications
    QList<int>        m_allFabIds;        // Liste de tous les IDs de fabrication
    int               m_currentFabIndex;  // Index courant dans la liste (pour navigation)


};

#endif // MAINWINDOW_H
