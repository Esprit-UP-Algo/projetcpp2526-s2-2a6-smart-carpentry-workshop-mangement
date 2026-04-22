// ============================================================================
// QUICK REFERENCE & TESTING GUIDE
// ============================================================================

// ============================================================================
// QUICK REFERENCE - STATUTS D'ÉTAPES
// ============================================================================

/*
╔════════════════════════════════════════════════════════════════════════════╗
║                      TABLEAU DES ÉTATS DES ÉTAPES                         ║
╠════════════════════════════════════════════════════════════════════════════╣
║ Statut           │ Couleur        │ Condition SQL                          ║
╠═══════════════════╪════════════════╪════════════════════════════════════════╣
║ NON_PLANIFIEE    │ 🔴 Rouge       │ DATEDEBUT IS NULL                      ║
║ PLANIFIEE        │ 🟠 Orange      │ DATEDEBUT NOT NULL AND TEMPSREEL = 0  ║
║                  │                │   AND DATEFIN IS NULL                  ║
║ EN_COURS         │ 🟡 Jaune       │ DATEDEBUT NOT NULL AND TEMPSREEL > 0  ║
║                  │                │   AND DATEFIN IS NULL                  ║
║ TERMINEE         │ 🟢 Vert        │ DATEFIN NOT NULL AND TEMPSREEL <=     ║
║                  │                │   TEMPSESTIME                          ║
║ EN_RETARD        │ 🔴 Rouge foncé │ DATEFIN NOT NULL AND TEMPSREEL >      ║
║                  │                │   TEMPSESTIME                          ║
╚════════════════════════════════════════════════════════════════════════════╝

Diagramme de transition:
    
    NON_PLANIFIEE
        ↓ (DATEDEBUT renseignée)
    PLANIFIEE
        ↓ (TEMPSREEL > 0)
    EN_COURS
        ↓ (DATEFIN renseignée)
    ├─→ TERMINEE (TEMPSREEL <= TEMPSESTIME)
    └─→ EN_RETARD (TEMPSREEL > TEMPSESTIME)
*/

// ============================================================================
// COPIER-COLLER RAPIDE - Intégration minimale
// ============================================================================

/*
1. AJOUTER À mainwindow.h (section private):

    struct EtapeData {
        int id, idFabrication, ordre, tempsEstime, tempsReel;
        QString nom, etat;
        QDate dateDebut, dateFin;
    };

    QList<EtapeData> m_etapesList;
    QTableWidget *m_tableEtapes = nullptr;

2. AJOUTER À mainwindow.cpp (en haut après includes):

    void MainWindow::chargerEtapes(int idFabrication) {
        // ... voir le fichier etapes_implementation.cpp
    }

3. APPELER depuis votre slot:

    void MainWindow::on_monTableau_cellClicked(int row, int col) {
        int idFab = ui->monTableau->item(row, 0)->text().toInt();
        chargerEtapes(idFab);
    }
*/

// ============================================================================
// CODES DE COULEUR - COPIER DANS couleurEtatEtape()
// ============================================================================

/*
NON_PLANIFIEE:  QColor(220, 53, 69)    ou #DC3545 ou 0xDC3545
PLANIFIEE:      QColor(255, 193, 7)    ou #FFC107 ou 0xFFC107
EN_COURS:       QColor(255, 235, 59)   ou #FFEB3B ou 0xFFEB3B
TERMINEE:       QColor(40, 167, 69)    ou #28A745 ou 0x28A745
EN_RETARD:      QColor(139, 0, 0)      ou #8B0000 ou 0x8B0000
INCONNUE:       QColor(108, 117, 125)  ou #6C757D ou 0x6C757D
*/

// ============================================================================
// TESTS & DEBUGGING
// ============================================================================

// TEST 1 : Vérifier la connexion DB
void MainWindow::testConnexionDB()
{
    QSqlDatabase db = Connexion::createInstance()->getBdd();
    
    if (db.isOpen()) {
        qDebug() << "✓ Connexion DB ouverte";
        qDebug() << "  Pilote:" << db.driverName();
        qDebug() << "  Base:" << db.databaseName();
    } else {
        qDebug() << "✗ Connexion DB fermée!";
        qDebug() << "  Erreur:" << db.lastError().text();
    }
}

// TEST 2 : Vérifier le contenu de la table ETAPE
void MainWindow::testTableEtape()
{
    QSqlQuery query(Connexion::createInstance()->getBdd());
    
    if (!query.exec("SELECT COUNT(*) FROM ETAPE")) {
        qDebug() << "✗ Erreur requête ETAPE:" << query.lastError().text();
        return;
    }
    
    if (query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "✓ Table ETAPE contient" << count << "enregistrements";
    }
}

// TEST 3 : Vérifier les colonnes ETAPE
void MainWindow::testColonnesEtape()
{
    QSqlQuery query(Connexion::createInstance()->getBdd());
    
    QString sql = "SELECT IDETAPE, IDFABRICATION, NOMETAPE, ORDRE, "
                  "DATEDEBUT, DATEFIN, TEMPSESTIME, TEMPSREEL "
                  "FROM ETAPE WHERE ROWNUM = 1";
    
    if (!query.exec(sql)) {
        qDebug() << "✗ Erreur requête:" << query.lastError().text();
        return;
    }
    
    if (query.next()) {
        qDebug() << "✓ Toutes les colonnes ETAPE existent";
        qDebug() << "  Exemple:";
        qDebug() << "    IDETAPE:" << query.value(0).type();
        qDebug() << "    NOMETAPE:" << query.value(2).toString();
    }
}

// TEST 4 : Tester la requête SQL avec CASE
void MainWindow::testRequeteSQLCase(int idFabrication = 1)
{
    QSqlQuery query(Connexion::createInstance()->getBdd());
    
    QString sql = QString(
        "SELECT IDETAPE, NOMETAPE, ORDRE, ETAT_ETAPE = "
        "    CASE "
        "        WHEN DATEDEBUT IS NULL THEN 'NON_PLANIFIEE' "
        "        WHEN DATEDEBUT IS NOT NULL AND NVL(TEMPSREEL, 0) = 0 "
        "             AND DATEFIN IS NULL THEN 'PLANIFIEE' "
        "        WHEN DATEDEBUT IS NOT NULL AND NVL(TEMPSREEL, 0) > 0 "
        "             AND DATEFIN IS NULL THEN 'EN_COURS' "
        "        WHEN DATEFIN IS NOT NULL AND NVL(TEMPSREEL, 0) <= NVL(TEMPSESTIME, 0) "
        "            THEN 'TERMINEE' "
        "        WHEN DATEFIN IS NOT NULL AND NVL(TEMPSREEL, 0) > NVL(TEMPSESTIME, 0) "
        "            THEN 'EN_RETARD' "
        "        ELSE 'INCONNUE' "
        "    END "
        "FROM ETAPE WHERE IDFABRICATION = %1 ORDER BY ORDRE"
    ).arg(idFabrication);
    
    if (!query.exec(sql)) {
        qDebug() << "✗ Erreur SQL:" << query.lastError().text();
        qDebug() << "  SQL:" << sql;
        return;
    }
    
    qDebug() << "✓ Requête SQL valide. Résultats:";
    int row = 0;
    while (query.next() && row < 10) {
        qDebug() << QString("  Etape %1 : %2 -> %3")
            .arg(query.value(2).toInt())
            .arg(query.value(1).toString(), -30)
            .arg(query.value(3).toString());
        row++;
    }
}

// TEST 5 : Tester chargerEtapes avec logging
void MainWindow::testChargerEtapesDebug(int idFabrication = 1)
{
    qDebug() << "═══════════════════════════════════════════";
    qDebug() << "TEST: chargerEtapes(" << idFabrication << ")";
    qDebug() << "═══════════════════════════════════════════";
    
    // Enregistrer le temps d'exécution
    QTime timer;
    timer.start();
    
    // Appeler la fonction
    chargerEtapes(idFabrication);
    
    int temps = timer.elapsed();
    
    qDebug() << "";
    qDebug() << "─ Résultats ─────────────────────────────";
    qDebug() << "✓ Étapes chargées:" << m_etapesList.size();
    qDebug() << "✓ Temps exécution:" << temps << "ms";
    qDebug() << "";
    
    if (m_tableEtapes) {
        qDebug() << "✓ Tableau:" << m_tableEtapes->rowCount() << "lignes";
    } else {
        qDebug() << "✗ Tableau non initialisé!";
    }
    
    if (!m_etapesList.isEmpty()) {
        qDebug() << "";
        qDebug() << "─ Détails étapes ────────────────────────";
        for (const EtapeData &etape : m_etapesList) {
            qDebug() << QString("  #%1 | %2 | %3 | %4h/%5h | %6")
                .arg(etape.ordre, 2)
                .arg(etape.nom, -25)
                .arg(etape.etat, -15)
                .arg(etape.tempsEstime, 2)
                .arg(etape.tempsReel, 2)
                .arg(formaterDate(etape.dateDebut));
        }
    }
    
    qDebug() << "";
    qDebug() << "═════════════════════════════════════════════";
}

// TEST 6 : Tester les couleurs
void MainWindow::testCouleurs()
{
    qDebug() << "Tests des couleurs:";
    
    QStringList etats = {"NON_PLANIFIEE", "PLANIFIEE", "EN_COURS", "TERMINEE", "EN_RETARD"};
    
    for (const QString &etat : etats) {
        QColor couleur = couleurEtatEtape(etat);
        QColor texte = couleurTexteContraste(couleur);
        
        qDebug() << QString("%1 -> RGB(%2,%3,%4) #%5 -> Texte: %6")
            .arg(etat, -20)
            .arg(couleur.red(), 3)
            .arg(couleur.green(), 3)
            .arg(couleur.blue(), 3)
            .arg(couleur.name())
            .arg(texte.name());
    }
}

// TEST 7 : Tester le formatage des dates
void MainWindow::testFormateDate()
{
    qDebug() << "Tests du formatage des dates:";
    
    QDate aujourd = QDate::currentDate();
    QDate hier = aujourd.addDays(-1);
    QVariant null;
    
    qDebug() << "Null:" << formaterDate(null);
    qDebug() << "Aujourd'hui:" << formaterDate(aujourd);
    qDebug() << "Hier:" << formaterDate(hier);
    qDebug() << "Invalid:" << formaterDate(QVariant("invalid"));
}

// TEST 8 : Tester la fonction formater date avec données réelles
void MainWindow::testFormateDateReel()
{
    QSqlQuery query(Connexion::createInstance()->getBdd());
    
    if (!query.exec("SELECT DATEDEBUT, DATEFIN FROM ETAPE WHERE ROWNUM = 1")) {
        qDebug() << "✗ Erreur:" << query.lastError().text();
        return;
    }
    
    if (query.next()) {
        qDebug() << "Dates DB:";
        qDebug() << "  Brut - Début:" << query.value(0) << "| Fin:" << query.value(1);
        qDebug() << "  Formaté - Début:" << formaterDate(query.value(0));
        qDebug() << "  Formaté - Fin:" << formaterDate(query.value(1));
    }
}

// ============================================================================
// FONCTION POUR EXÉCUTER TOUS LES TESTS
// ============================================================================

/**
 * @brief Exécute une suite complète de tests
 * À appeler au démarrage pour valider l'implémentation
 */
void MainWindow::executerTousLesTests()
{
    qDebug() << "";
    qDebug() << "╔════════════════════════════════════════════╗";
    qDebug() << "║        SUITE DE TESTS COMPLÈTE            ║";
    qDebug() << "╚════════════════════════════════════════════╝";
    qDebug() << "";
    
    qDebug() << "TEST 1/8: Connexion DB";
    testConnexionDB();
    qDebug() << "";
    
    qDebug() << "TEST 2/8: Table ETAPE";
    testTableEtape();
    qDebug() << "";
    
    qDebug() << "TEST 3/8: Colonnes ETAPE";
    testColonnesEtape();
    qDebug() << "";
    
    qDebug() << "TEST 4/8: Requête SQL CASE";
    testRequeteSQLCase(1);
    qDebug() << "";
    
    qDebug() << "TEST 5/8: chargerEtapes Debug";
    testChargerEtapesDebug(1);
    qDebug() << "";
    
    qDebug() << "TEST 6/8: Couleurs";
    testCouleurs();
    qDebug() << "";
    
    qDebug() << "TEST 7/8: Formatage dates";
    testFormateDate();
    qDebug() << "";
    
    qDebug() << "TEST 8/8: Formatage dates réelles DB";
    testFormateDateReel();
    qDebug() << "";
    
    qDebug() << "╔════════════════════════════════════════════╗";
    qDebug() << "║        FIN DES TESTS                       ║";
    qDebug() << "╚════════════════════════════════════════════╝";
    qDebug() << "";
}

// ============================================================================
// APPEL DES TESTS
// ============================================================================

/*
// Dans MainWindow::MainWindow() ou après le setupUI():
void MainWindow::testImplementation()
{
    // Exécuter tous les tests
    executerTousLesTests();
    
    // Ou tester spécifiquement:
    // testChargerEtapesDebug(42);
}

// Ajouter un bouton de test dans l'UI:
void MainWindow::on_btnTest_clicked()
{
    executerTousLesTests();
}

// Ou ajouter dans le constructeur pour test au démarrage:
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // ... autres initialisations ...
    
    // Tester au démarrage
    QTimer::singleShot(1000, this, &MainWindow::executerTousLesTests);
}
*/

// ============================================================================
// RÉSULTATS ATTENDUS - EXEMPLE
// ============================================================================

/*
═══════════════════════════════════════════
TEST: chargerEtapes( 1 )
═══════════════════════════════════════════

─ Résultats ─────────────────────────────
✓ Étapes chargées: 5
✓ Temps exécution: 142 ms

✓ Tableau: 5 lignes

─ Détails étapes ────────────────────────
  #1 | Découpe                       | NON_PLANIFIEE | 10h/ 0h | 01/01/2026
  #2 | Assemblage                    | PLANIFIEE     | 15h/ 0h | 05/01/2026
  #3 | Soudure                       | EN_COURS      | 12h/ 8h | 08/01/2026
  #4 | Finition                      | TERMINEE      |  8h/ 7h | 10/01/2026
  #5 | Emballage                     | EN_RETARD     |  5h/ 6h | 12/01/2026

═════════════════════════════════════════════

Tests des couleurs:
NON_PLANIFIEE    -> RGB(220, 53, 69)   #dc3545 -> Texte: #ffffff
PLANIFIEE        -> RGB(255,193,  7)   #ffc107 -> Texte: #000000
EN_COURS         -> RGB(255,235, 59)   #ffeb3b -> Texte: #000000
TERMINEE         -> RGB( 40,167, 69)   #28a745 -> Texte: #ffffff
EN_RETARD        -> RGB(139,  0,  0)   #8b0000 -> Texte: #ffffff
*/

// ============================================================================
// FIN DU GUIDE DE TESTING
// ============================================================================
