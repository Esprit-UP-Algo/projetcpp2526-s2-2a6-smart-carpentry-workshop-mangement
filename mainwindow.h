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
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>
#include <QSet>

#include "modele.h"
#include "personnel.h"
#include "bois.h"

// Forward declaration
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// ─────────────────────────────────────────────────────────────
//  Structures de Données (Suivi, Fabrication, Alertes)
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
    QString cin;      
};

struct FabricationSuivi {
    int     id;
    QString modele;
    int     quantite;
    QString qualite;
    QDate   dateDebut;
    QList<EtapeSuivi> etapes;
};

struct EtapeInfo {
    QString nom;
    int     temps;
    int     ordre;
};

struct AlerteInfo {
    QString type;
    QString niveau;
    int     idFabrication;
    int     idEtape;
    QString nomEtape;
    QString message;
    QString cause;
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

    // ── Bois (Intégration Fichier 1 + Fichier 2) ──────────────────────────
    void on_la_ajouter_7_clicked();
    void on_la_modifier_7_clicked();
    void on_la_supprimer_7_clicked();
    void on_chercher_7_clicked();
    void on_tab_bois_7_clicked(const QModelIndex &index);
    void on_la_trier_7_clicked();
    void on_la_pdf_9_clicked();
    void on_poser_7_clicked();             // Provient du fichier 2
    void on_stat_bois_clicked();           // Provient du fichier 2
    void on_calcul_decoupage_7_clicked();  // Provient du fichier 2

    // ── Fabrication & Suivi (Complet) ─────────────────────────────────────
    void on_btn_ajouter_2_clicked();
    void on_btn_modifier_clicked();
    void on_btn_supprimer_clicked();
    void on_btn_rechercher_6_clicked();
    void on_btn_tire_2_clicked();
    void on_btn_export_pdf_modele_2_clicked();
    void on_table_modeles_2_cellClicked(int row, int column);
    void on_rech_7_textChanged(const QString &text);
    void on_btnOptimizer_clicked();
    void on_btnAffectationManuelle_clicked();
    void on_btnAjouterEtape_clicked();
    void on_btnDemarrerEtape_clicked();
    void on_btnTerminerEtape_clicked();
    void on_btnAlertes_clicked();

private:
    Ui::MainWindow *ui;
    int m_idSelectionne = -1;

    // ── Connexion & Système ───────────────────────────────────────────────
    QSqlDatabase db() const;

    // ── Modeles & Analyses ────────────────────────────────────────────────
    void chargerTableauModeles();
    bool validerFormulaire();
    void reinitialiserFormulaire();
    void afficherStatistiques();
    void afficherTendances();
    void insererRelationsTendances();
    void afficherAnalyseCouts();

    // ── Personnel ─────────────────────────────────────────────────────────
    void refreshTable();
    void clearFields();
    bool controlDeSaisie();

    // ── Bois & IA (Intégration Fichier 2) ─────────────────────────────────
    void refreshBoisTable();
    void appelGroq(const QString &question);
    void afficherStatsBois();
    void calculerDecoupage();

    // ── Fabrication : Logique de base ─────────────────────────────────────
    void loadFabrications();
    void generateFabricationId();
    void clearFabricationFields();
    bool validateFabricationFields();
    void filterFabrications(const QString &searchText);
    void sortFabrications(const QString &criteria);
    int  generateId();
    void exportToPDF();

    // ── Fabrication : Suivi, Timeline & Complexité (Fichier 1) ────────────
    void setupSuivi();
    void buildKPICards();
    void buildEtapeStatusPieChart();
    void buildFabricationsByModeleBarChart();
    void buildStatsInsightsPanel();
    void chargerStatistiquesComplexite();
    void buildFiltresSuivi();
    void loadFabricationsSuivi();
    void chargerTableSuiviAvecComplexite();
    void loadFabricationsSuiviFiltre(const QString &filtreStatut, const QString &filtreQualite, const QString &filtreId);
    void afficherPanneauDetail(int fabId);
    void reloadAllFabIds();
    QMap<QString, QList<EtapeInfo>> getCatalogue();
    QMap<QString, QList<EtapeInfo>> getCatalogueParModele();
    QString getNomModelePourFabrication(int idFabrication);
    QList<EtapeInfo> genererEtapesSelonModele(const QString &nomModele);
    void chargerEtapes(int idFabrication);
    int getSelectedFabricationId();
    int getSelectedEtapeId();
    int getNextOrdre(int idFabrication);
    bool etapeExisteDeja(int idFabrication, const QString &nomEtape);
    int tempsCatalogue(const QString &nomEtape);
    int currentSelectedFabricationId() const;
    bool insererEtapeSiAbsente(int fabId, const EtapeInfo &etape, const QString &cin = QString(), const QDate &dateDebut = QDate(), bool *estNouvelleInsertion = nullptr, QString *detailErreur = nullptr);
    void refreshSuiviAfterChanges(int fabId);
    int cinPersonnelParDefaut() const;
    QString calculerEtatEtape(const QVariant &dateDebut, const QVariant &dateFin, int tempsEstime, int tempsReel);
    QString formaterDate(const QVariant &value);
    QString formaterTexte(const QVariant &value);
    QString couleurEtatEtapeBadge(const QString &etat) const;
    FabricationSuivi getFabricationSuivi(int fabId);
    void computeTimelineStates(FabricationSuivi &fab, int *completedSteps = nullptr, int *currentIndex = nullptr, int *nextIndex = nullptr, double *progression = nullptr);
    void clearTimeline();
    QWidget* createStepBlock(const EtapeSuivi &etape);
    void buildTimeline(const FabricationSuivi &fab);
    void afficherDetailsEtape(int idEtape);
    void demarrerEtapeDepuisDialog(int idEtape);
    void terminerEtapeDepuisDialog(int idEtape);
    bool peutDemarrerEtape(int idEtape);
    bool existeEtapeActiveDansFabrication(int idFabrication, int idEtapeCourante = -1);
    QVariantMap chargerDonneesEtape(int idEtape);
    QString calculerComplexiteFabrication(int idFabrication);
    QString predireStatutFabrication(int fabId);
    QColor getPredictionColor(const QString &status);
    QString getPredictionIcon(const QString &status);
    QString getPredictionStatus(double ratio);
    double calculatePerformanceRatio(double tempsReel, double tempsEstime);
    QString couleurComplexiteBadge(const QString &complexite) const;
    double calculerProgression(int fabId);
    QString calculerStatutEtape(const QDate &dateDebut, const QDate &dateFin, double tempsEstime);
    QString calculerStatutFabrication(int fabId);
    int compterEtapesFabrication(int idFabrication);
    int calculerTempsEstimeTotal(int idFabrication);
    QString getEtapeActuelle(int fabId);
    QString getProchainEtape(int fabId);
    void mettreAJourTimelineComplexite();
    void applyAIOptimisedTimeline(const QString &aiText);
    void verifierAlertes();
    QList<AlerteInfo> chargerAlertes();
    void mettreAJourBoutonAlertes();
    void afficherDialogAlertes();
    void verifierAlertesAuDemarrage();
    void allerVersAlerte(int idFabrication, int idEtape);
    void calculateAndDisplayFabricationPrediction(int fabId);
    void loadEtapesCatalogue();

    // ── Arduino ──────────────────────────────────────────────────────────
    void initArduino();
    void envoyerArduino(const QString &message);
    void envoyerBienvenueArduino();
    void envoyerAlertesArduino();
    void envoyerModificationArduino(const QString &module, const QString &operation, int id, const QString &valeur = QString());
    void notifierChangementArduino(const QString &action, const QString &detail);

    // ── UI Helpers ────────────────────────────────────────────────────────
    void showAnimatedMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text, const QString &informativeText = "");
    bool showAnimatedQuestionBox(const QString &title, const QString &text);

    // ── Données membres ──────────────────────────────────────────────────
    FabricationSuivi  m_selectedFab;      
    bool              m_fabSelected;      

    QString           m_currentFabricationSearchText;
    QString           currentSortField = QStringLiteral("id_fabrication");
    Qt::SortOrder     currentSortOrder = Qt::AscendingOrder;

    QList<int>        m_allFabIds;        
    int               m_currentFabIndex;  
    int               m_idEtape = -1;

    QList<AlerteInfo> m_alertesCache;
};

#endif // MAINWINDOW_H