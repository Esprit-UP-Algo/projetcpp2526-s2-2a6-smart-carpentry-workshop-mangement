#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QDate>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QList>
#include <QVariantMap>

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
    double  tempsEstime;   // Changé de int à double pour correspondre au .cpp
    double  tempsReel;     // Changé de int à double
    QDate   dateDebut;
    QDate   dateFin;
    QString statut;        // "TERMINE" | "EN_COURS" | "RETARD" | "NON_COMMENCE"
    QString cin;
    QString personnelNom;
    bool    estRetard = false;
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

// ─────────────────────────────────────────────────────────────
//  Catalogue / génération d'étapes
// ─────────────────────────────────────────────────────────────
struct EtapeInfo {
    QString nom;
    int     temps;
    int     ordre;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Méthodes publiques pour le calcul des statuts
    QString calculerStatutEtape(const QDate &dateDebut,
                                const QDate &dateFin,
                                double tempsEstime);
    QString calculerStatutFabrication(int fabId);
    double calculerProgression(int fabId);
    QString calculerComplexiteFabrication(int idFabrication);
    int compterEtapesFabrication(int idFabrication);
    int calculerTempsEstimeTotal(int idFabrication);
    QString getEtapeActuelle(int fabId);
    QString getProchainEtape(int fabId);

    // Méthodes publiques pour le chatbot
    QString repondreQuestion(const QString &question, int fabId);
    void appendChatMessage(const QString &message);

private slots:
    // ── Navigation ──────────────────────────────────────────
    void on_btn_personnel_clicked();
    void on_btn_modele_clicked();
    void on_btn_bois_clicked();
    void on_btn_etape_clicked();
    void on_btn_fabrication_clicked();

    // ── Fabrication CRUD ────────────────────────────────────
    void on_btn_ajouter_2_clicked();
    void on_btn_modifier_clicked();
    void on_btn_supprimer_clicked();
    void on_btn_rechercher_6_clicked();
    void on_btn_tire_2_clicked();
    void on_btn_export_pdf_modele_2_clicked();
    void on_table_modeles_2_cellClicked(int row, int column);
    void on_rech_7_textChanged(const QString &text);

    // ── Suivi ────────────────────────────────────────────────
    void on_table_fabrications_etapes_cellClicked(int row, int column);
    void on_filtres_suivi_changed();
    void on_btnOptimizer_clicked();
    void on_btnAffectationManuelle_clicked();
    void on_btnAjouterEtape_clicked();
    void on_btnDemarrerEtape_clicked();
    void on_btnTerminerEtape_clicked();

    // ── Chatbot ─────────────────────────────────────────────
    void on_btn_chatbot_clicked();
    void on_chat_status_clicked();
    void on_chat_retard_clicked();
    void on_chat_ratio_clicked();
    void on_chat_terminees_clicked();
    void on_chat_encours_clicked();
    void on_chat_temps_prevu_clicked();
    void on_chat_temps_reel_clicked();
    void on_chat_ratio_inf_clicked();
    void on_chat_demain_clicked();
    void on_chat_statut_fab_clicked();

    // ── Réseau AI ────────────────────────────────────────────
    void onAIReplyReadyRead();
    void onAIReplyFinished();
    void onAIReplyError(QNetworkReply::NetworkError error);

private:
    Ui::MainWindow *ui;

    // ── Setup ─────────────────────────────────────────────────
    void setupSuivi();
    void buildKPICards();
    void ensureStatistiqueWidgets();
    void buildEtapeStatusPieChart();
    void buildFabricationsByModeleBarChart();
    void buildStatsInsightsPanel();
    void buildFiltresSuivi();
    void loadFabricationsSuivi();
    void chargerTableSuiviAvecComplexite();
    void loadFabricationsSuiviFiltre(const QString &filtreStatut,
                                     const QString &filtreQualite,
                                     const QString &filtreId);
    void afficherPanneauDetail(int fabId);

    // ── Fabrication helpers ──────────────────────────────────
    void loadFabrications();
    void refreshFabricationsTable(const QString &searchText = QString());
    void generateFabricationId();
    void clearFabricationFields();
    bool validateFabricationFields();
    void filterFabrications(const QString &searchText);
    void trierFabrications(const QString &champ);
    void sortFabrications(const QString &criteria);
    int  generateId();
    void exportToPDF();

    // ── Suivi / Timeline helpers ─────────────────────────────
    void reloadAllFabIds();
    void loadEtapesCatalogue();
    void buildTimeline(const FabricationSuivi &fab);
    void clearTimeline();
    QWidget* createStepBlock(const EtapeSuivi &etape);
    void afficherDetailsEtape(int idEtape);
    void demarrerEtapeDepuisDialog(int idEtape);
    void terminerEtapeDepuisDialog(int idEtape);
    bool peutDemarrerEtape(int idEtape);
    bool existeEtapeActiveDansFabrication(int idFabrication, int idEtapeCourante = -1);
    QVariantMap chargerDonneesEtape(int idEtape);
    QString calculerEtatEtape(const QVariant &dateDebut,
                              const QVariant &dateFin,
                              int tempsEstime,
                              int tempsReel);
    QString formaterDate(const QVariant &value);
    QString formaterTexte(const QVariant &value);
    QString couleurEtatEtapeBadge(const QString &etat) const;
    void computeTimelineStates(FabricationSuivi &fab,
                               int *completedSteps = nullptr,
                               int *currentIndex = nullptr,
                               int *nextIndex = nullptr,
                               double *progression = nullptr);
    FabricationSuivi getFabricationSuivi(int fabId);
    void mettreAJourTimelineComplexite();
    void applyAIOptimisedTimeline(const QString &aiText);
    void chargerStatistiquesComplexite();
    void verifierAlertes();
    QMap<QString, QList<EtapeInfo>> getCatalogue();
    QMap<QString, QList<EtapeInfo>> getCatalogueParModele();
    QString getNomModelePourFabrication(int idFabrication);
    QList<EtapeInfo> genererEtapesSelonModele(const QString &nomModele);
    void chargerEtapes(int idFabrication);
    int getSelectedEtapeId();
    int getSelectedFabricationId();
    int getNextOrdre(int idFabrication);
    bool etapeExisteDeja(int idFabrication, const QString &nomEtape);
    int tempsCatalogue(const QString &nomEtape);
    int currentSelectedFabricationId() const;
    void refreshSuiviAfterChanges(int fabId);
    int cinPersonnelParDefaut() const;
    bool insererEtapeSiAbsente(int fabId,
                               const EtapeInfo &etape,
                               const QString &cin = QString(),
                               const QDate &dateDebut = QDate(),
                               bool *estNouvelleInsertion = nullptr,
                               QString *detailErreur = nullptr);

    // ── Chatbot helpers (implémentations des réponses) ────────
    QString getChatStatusGlobal();
    QString getChatRetard();
    QString getChatRatioGlobal();
    QString getChatEtapesEnCours();
    QString getChatDemainGlobal();
    QString getChatRatioInfGlobal();
    QString getChatPersonnel(int fabId);
    QString getChatPlusRisquee();
    QString getChatPlusGrandEcart();
    QString getChatProgressionFaible();
    QString getChatNbTerminees();

    QString getFabricationStatusMessage(int fabId);
    QString getDelayMessage(int fabId);
    QString getSlowestStepMessage(int fabId);
    QString getTotalEstimatedMessage(int fabId);
    QString getTotalRealMessage(int fabId);
    QString getCurrentStepMessage(int fabId);
    QString getTermineesTodayMessage(int fabId);
    QString getRatioInfMessage(int fabId);
    QString getDemainMessage(int fabId);

    // ── Complexité & Performance Analysis ────────────────────
    QColor getPredictionColor(const QString &status);
    QString getPredictionIcon(const QString &status);
    QString getPredictionStatus(double ratio);
    QString predireStatutFabrication(int fabId);
    QString couleurComplexiteBadge(const QString &complexite) const;
    double calculatePerformanceRatio(double tempsReel, double tempsEstime);
    void calculateAndDisplayFabricationPrediction(int fabId);

    // ── UI helpers ───────────────────────────────────────────
    void showAnimatedMessageBox(QMessageBox::Icon icon,
                                const QString &title,
                                const QString &text,
                                const QString &informativeText = "");
    bool showAnimatedQuestionBox(const QString &title, const QString &text);

    // ── Données membres ──────────────────────────────────────
    FabricationSuivi  m_selectedFab;
    bool              m_fabSelected;

    QString           m_currentFabricationSearchText;
    QString           currentSortField = QStringLiteral("id_fabrication");
    Qt::SortOrder     currentSortOrder = Qt::AscendingOrder;

    QList<int>        m_allFabIds;
    int               m_currentFabIndex;
    int               m_idEtape = -1;

    QNetworkAccessManager *m_networkManager;
    QNetworkReply         *m_currentReply;
    QString                m_aiAccumulated;

    static const QString ANTHROPIC_API_KEY;
    static const QString ANTHROPIC_API_URL;
    static const QString ANTHROPIC_MODEL;
};

#endif // MAINWINDOW_H
