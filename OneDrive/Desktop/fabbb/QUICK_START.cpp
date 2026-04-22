// ============================================================================
// QUICK START - 5 MINUTES D'INTÉGRATION
// ============================================================================

// ════════════════════════════════════════════════════════════════════════════
// ÉTAPE 1: Ajouter à mainwindow.h (section private après class definition)
// ════════════════════════════════════════════════════════════════════════════

/*

private:
    // Structure pour stocker une étape
    struct EtapeData {
        int id;
        int idFabrication;
        QString nom;
        int ordre;
        QDate dateDebut;
        QDate dateFin;
        int tempsEstime;
        int tempsReel;
        QString etat;
    };

    // Variables membres
    QList<EtapeData> m_etapesList;
    QTableWidget *m_tableEtapes = nullptr;
    QWidget *m_timelineContainer = nullptr;

    // Slots & fonctions
    void chargerEtapes(int idFabrication);
    void afficherTimelineEtapes();
    void nettoyerEtapes();
    
    QWidget *creerCarteEtape(const EtapeData &etape, bool estDerniere);
    QColor couleurEtatEtape(const QString &etat);
    QColor couleurTexteContraste(const QColor &couleurFond);
    QString formaterDate(const QVariant &dateValue);
    QString libelleurEtatEtape(const QString &etat);
    QString obtenirEtatEtape(int idEtape) const;

*/

// ════════════════════════════════════════════════════════════════════════════
// ÉTAPE 2: Copier TOUTES les fonctions du fichier etapes_implementation.cpp
// ════════════════════════════════════════════════════════════════════════════

/*
  Copier entre les balises:
  
  // ============================================================================
  // 2. FONCTIONS HELPER - COULEURS ET FORMATAGE
  // ============================================================================
  
  jusqu'à
  
  // ============================================================================
  // 7. FONCTION POUR NETTOYER LES RESSOURCES
  // ============================================================================

  Coller dans mainwindow.cpp (à la fin du fichier)
*/

// ════════════════════════════════════════════════════════════════════════════
// ÉTAPE 3: Initialiser les widgets dans le constructeur MainWindow
// ════════════════════════════════════════════════════════════════════════════

/*

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // ... vos initialisations existantes ...
    
    // AJOUTER CES LIGNES:
    m_tableEtapes = ui->tableEtapes;  // Remplacer tableEtapes par votre widget
    m_timelineContainer = ui->containerTimeline;  // Remplacer par votre widget
    
    // Alternative si widgets créés en code:
    // m_tableEtapes = new QTableWidget(this);
    // ui->someLayout->addWidget(m_tableEtapes);
    
    // m_timelineContainer = new QWidget(this);
    // ui->someLayout->addWidget(m_timelineContainer);
}

*/

// ════════════════════════════════════════════════════════════════════════════
// ÉTAPE 4: Ajouter l'appel à chargerEtapes() dans un slot existant
// ════════════════════════════════════════════════════════════════════════════

/*

EXEMPLE 1: Dans un slot de clic sur un tableau

void MainWindow::on_tableFabrications_cellClicked(int row, int column)
{
    // Récupérer l'ID de la fabrication (supposé colonne 0)
    int idFab = ui->tableFabrications->item(row, 0)->text().toInt();
    
    // AJOUTER CETTE LIGNE:
    chargerEtapes(idFab);
    
    // ... code existant ...
}


EXEMPLE 2: Dans un slot de bouton

void MainWindow::on_btnAfficherEtapes_clicked()
{
    // AJOUTER CES LIGNES:
    int idFab = 42;  // À remplacer par votre logique
    chargerEtapes(idFab);
}


EXEMPLE 3: Dans un slot de combobox

void MainWindow::on_cbFabrications_currentIndexChanged(int index)
{
    if (index >= 0) {
        // AJOUTER CES LIGNES:
        int idFab = ui->cbFabrications->itemData(index).toInt();
        chargerEtapes(idFab);
    }
}

*/

// ════════════════════════════════════════════════════════════════════════════
// ÉTAPE 5: Vérifier la compilation
// ════════════════════════════════════════════════════════════════════════════

/*
Raccourci clavier: Ctrl+K, Ctrl+B (Qt Creator)

Ou: Build → Build Project

La compilation doit retourner 0 erreurs.

Si erreurs:
  1. Vérifier que toutes les fonctions sont bien ajoutées à mainwindow.cpp
  2. Vérifier que la structure EtapeData est bien dans mainwindow.h
  3. Vérifier les includes en haut de mainwindow.cpp/h
  4. Consulter TESTING_GUIDE.cpp → testTableEtape() pour vérifier la DB
*/

// ════════════════════════════════════════════════════════════════════════════
// ÉTAPE 6: Tester
// ════════════════════════════════════════════════════════════════════════════

/*
Lancer l'application: F5 (ou Run)

Points à vérifier:
  ✓ Pas de crash au démarrage
  ✓ Tableau affiche les étapes
  ✓ Colonnes: ID Etape, ID Fab, Nom Etape, Ordre, Date Début, Date Fin, 
              Temps Est., Temps Réel, Statut
  ✓ Colonne "Statut" est colorée (rouge/orange/jaune/vert/rouge foncé)
  ✓ Timeline affiche les cartes d'étapes en horizontal
  ✓ Pas de warnings en console

Si problème: Consulter le fichier TESTING_GUIDE.cpp
*/

// ════════════════════════════════════════════════════════════════════════════
// TABLEAU DES ÉTATS ET COULEURS - POUR RÉFÉRENCE
// ════════════════════════════════════════════════════════════════════════════

/*
┌──────────────────────────────────────────────────────────────────────┐
│ ÉTAT            │ COULEUR        │ RGB              │ HEX            │
├──────────────────────────────────────────────────────────────────────┤
│ NON_PLANIFIEE   │ 🔴 Rouge       │ (220, 53, 69)    │ #DC3545        │
│ PLANIFIEE       │ 🟠 Orange      │ (255, 193, 7)    │ #FFC107        │
│ EN_COURS        │ 🟡 Jaune       │ (255, 235, 59)   │ #FFEB3B        │
│ TERMINEE        │ 🟢 Vert        │ (40, 167, 69)    │ #28A745        │
│ EN_RETARD       │ 🔴 Rouge foncé │ (139, 0, 0)      │ #8B0000        │
└──────────────────────────────────────────────────────────────────────┘
*/

// ════════════════════════════════════════════════════════════════════════════
// REQUÊTE SQL - POUR TESTER EN SQL DEVELOPER
// ════════════════════════════════════════════════════════════════════════════

/*

SELECT
    IDETAPE,
    IDFABRICATION,
    NOMETAPE,
    ORDRE,
    DATEDEBUT,
    DATEFIN,
    TEMPSESTIME,
    TEMPSREEL,
    CASE
        WHEN DATEDEBUT IS NULL 
            THEN 'NON_PLANIFIEE'
        WHEN DATEDEBUT IS NOT NULL 
             AND NVL(TEMPSREEL, 0) = 0 
             AND DATEFIN IS NULL 
            THEN 'PLANIFIEE'
        WHEN DATEDEBUT IS NOT NULL 
             AND NVL(TEMPSREEL, 0) > 0 
             AND DATEFIN IS NULL 
            THEN 'EN_COURS'
        WHEN DATEFIN IS NOT NULL 
             AND NVL(TEMPSREEL, 0) <= NVL(TEMPSESTIME, 0) 
            THEN 'TERMINEE'
        WHEN DATEFIN IS NOT NULL 
             AND NVL(TEMPSREEL, 0) > NVL(TEMPSESTIME, 0) 
            THEN 'EN_RETARD'
        ELSE 'INCONNUE'
    END AS ETAT_ETAPE
FROM ETAPE
WHERE IDFABRICATION = 1  -- Remplacer 1 par votre ID de fabrication
ORDER BY ORDRE ASC;

Exécuter pour vérifier que les données s'affichent correctement
*/

// ════════════════════════════════════════════════════════════════════════════
// TROUBLESHOOTING
// ════════════════════════════════════════════════════════════════════════════

/*
PROBLÈME                          │ SOLUTION
──────────────────────────────────┼─────────────────────────────────
Erreur de compilation             │ Vérifier tous les includes
                                  │ Copier les fonctions complètes
────────────────────────────────  │ ─────────────────────────────────
Tableau vide                       │ Vérifier IDFABRICATION existe en DB
                                  │ Tester la requête SQL en direct
────────────────────────────────  │ ─────────────────────────────────
Couleurs ne s'appliquent pas      │ Vérifier couleurEtatEtape()
                                  │ Vérifier QTableWidgetItem setItem()
────────────────────────────────  │ ─────────────────────────────────
Timeline ne s'affiche pas         │ Vérifier m_timelineContainer init
                                  │ Vérifier afficherTimelineEtapes()
────────────────────────────────  │ ─────────────────────────────────
Crashes au démarrage              │ Vérifier m_tableEtapes != nullptr
                                  │ Ajouter null checks
────────────────────────────────  │ ─────────────────────────────────
Dates pas formatées correctement   │ Vérifier formaterDate()
                                  │ Vérifier le format "dd/MM/yyyy"

*/

// ════════════════════════════════════════════════════════════════════════════
// FICHIERS DE RÉFÉRENCE
// ════════════════════════════════════════════════════════════════════════════

/*
📄 etapes_implementation.cpp
   → Contient TOUTES les fonctions à copier

📄 etapes_header_snippet.h
   → Contient les déclarations pour mainwindow.h

📄 GUIDE_INTEGRATION.cpp
   → Guide détaillé, 14 étapes avec explications

📄 TESTING_GUIDE.cpp
   → Tests automatisés et debugging

📄 SUMMARY.cpp
   → Résumé complet et explications techniques

📄 QUICK_START.cpp
   → Ce fichier (vous êtes ici!)
*/

// ════════════════════════════════════════════════════════════════════════════
// PROCHAINES ÉTAPES OPTIONNELLES
// ════════════════════════════════════════════════════════════════════════════

/*
Après l'intégration de base, vous pouvez ajouter:

1. Bouton "Rafraîchir":
   void MainWindow::on_btnRefresh_clicked() {
       chargerEtapes(m_currentFabId);
   }

2. Recherche/Filtrage:
   void MainWindow::filtrerEtapes(const QString &filtre);

3. Export CSV:
   void MainWindow::exporterEtapesCSV(const QString &chemin);

4. Statistiques:
   void MainWindow::afficherStatistiques();

5. Alertes si en retard:
   if (etape.etat == "EN_RETARD") { /* alerte */ }

6. Cache en mémoire:
   Vérifier si déjà chargé avant requête DB

Pour ces améliorations, consultez GUIDE_INTEGRATION.cpp
*/

// ════════════════════════════════════════════════════════════════════════════
// CONTACT & SUPPORT
// ════════════════════════════════════════════════════════════════════════════

/*
Pour toute question:

1. Consulter GUIDE_INTEGRATION.cpp
2. Consulter TESTING_GUIDE.cpp  
3. Vérifier la base de données avec testTableEtape()
4. Vérifier la requête SQL en SQL Developer
5. Consulter la documentation Qt officielle

Logs:
  - Ouvrir l'onglet "Application Output" en Qt Creator
  - Exécuter executerTousLesTests() pour diagnostic complet

═══════════════════════════════════════════════════════════════════════════════
                               BONNE CHANCE! 🚀
═══════════════════════════════════════════════════════════════════════════════
*/
