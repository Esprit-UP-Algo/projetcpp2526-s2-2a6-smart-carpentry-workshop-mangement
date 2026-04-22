// ============================================================================
// SNIPPET POUR mainwindow.h
// À ajouter dans la classe MainWindow
// ============================================================================

// ============================================================================
// 1. STRUCTURES
// ============================================================================

/**
 * @brief Structure pour stocker les données d'une étape
 */
struct EtapeData {
    int id;                    // ID de l'étape
    int idFabrication;         // ID de la fabrication parent
    QString nom;               // Nom de l'étape
    int ordre;                 // Numéro d'ordre dans la fabrication
    QDate dateDebut;           // Date de début
    QDate dateFin;             // Date de fin
    int tempsEstime;           // Temps estimé (heures)
    int tempsReel;             // Temps réel (heures)
    QString etat;              // Statut calculé (NON_PLANIFIEE, PLANIFIEE, EN_COURS, TERMINEE, EN_RETARD)
};

// ============================================================================
// 2. DÉCLARATIONS DE FONCTIONS (à ajouter dans la section private slots/methods)
// ============================================================================

private slots:
    /**
     * Charge les étapes d'une fabrication et les affiche
     */
    void chargerEtapes(int idFabrication);
    
    /**
     * Affiche la timeline visuelle des étapes
     */
    void afficherTimelineEtapes();
    
    /**
     * Nettoie les données et widgets des étapes
     */
    void nettoyerEtapes();

private:
    /**
     * Crée une carte visuelle pour une étape
     */
    QWidget *creerCarteEtape(const EtapeData &etape, bool estDerniere);
    
    /**
     * Retourne la couleur associée à un statut d'étape
     */
    QColor couleurEtatEtape(const QString &etat);
    
    /**
     * Retourne la couleur de texte appropriée pour le contraste
     */
    QColor couleurTexteContraste(const QColor &couleurFond);
    
    /**
     * Formate une date QVariant en chaîne lisible
     */
    QString formaterDate(const QVariant &dateValue);
    
    /**
     * Retourne le libellé lisible du statut d'une étape
     */
    QString libelleurEtatEtape(const QString &etat);
    
    /**
     * Retourne le statut d'une étape spécifique par ID
     */
    QString obtenirEtatEtape(int idEtape) const;

// ============================================================================
// 3. VARIABLES MEMBRES (à ajouter dans la section private)
// ============================================================================

private:
    /**
     * Liste des étapes actuellement chargées
     */
    QList<EtapeData> m_etapesList;
    
    /**
     * Pointeur vers le tableau des étapes (QTableWidget)
     */
    QTableWidget *m_tableEtapes = nullptr;
    
    /**
     * Conteneur pour la timeline visuelle
     */
    QWidget *m_timelineContainer = nullptr;

// ============================================================================
// 4. INCLUDES NÉCESSAIRES (à vérifier en haut du fichier mainwindow.h)
// ============================================================================

/*
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QFrame>
#include <QDate>
#include <QColor>
#include <QBrush>
#include <QFont>
#include <QSqlQuery>
#include <QSqlError>
*/

// ============================================================================
// FIN DU SNIPPET POUR mainwindow.h
// ============================================================================
