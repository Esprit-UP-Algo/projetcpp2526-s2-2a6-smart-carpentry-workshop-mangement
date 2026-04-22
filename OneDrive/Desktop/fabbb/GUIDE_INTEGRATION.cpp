// ============================================================================
// GUIDE D'INTÉGRATION - STATUTS DYNAMIQUES D'ÉTAPES
// ============================================================================
// Date: 2026
// Framework: Qt 6.7.3 + Oracle 11g
// Pattern: Calcul dynamique sans modification du schéma
// ============================================================================

// ============================================================================
// ÉTAPE 1 : AJOUTER LES DÉCLARATIONS À mainwindow.h
// ============================================================================

/*
1. Ajouter la structure EtapeData après les includes:

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

2. Ajouter les variables membres dans la section private:

private:
    QList<EtapeData> m_etapesList;
    QTableWidget *m_tableEtapes = nullptr;
    QWidget *m_timelineContainer = nullptr;

3. Ajouter les déclarations de slots/fonctions privées:

private slots:
    void chargerEtapes(int idFabrication);
    void afficherTimelineEtapes();
    void nettoyerEtapes();

private:
    QWidget *creerCarteEtape(const EtapeData &etape, bool estDerniere);
    QColor couleurEtatEtape(const QString &etat);
    QColor couleurTexteContraste(const QColor &couleurFond);
    QString formaterDate(const QVariant &dateValue);
    QString libelleurEtatEtape(const QString &etat);
    QString obtenirEtatEtape(int idEtape) const;
*/

// ============================================================================
// ÉTAPE 2 : INITIALISER LES WIDGETS DANS LE CONSTRUCTEUR OU SETUP
// ============================================================================

/*
// Dans le constructeur MainWindow::MainWindow() ou dans une fonction setupUI():

// Supposons que vous avez créé un QTableWidget dans votre UI
m_tableEtapes = new QTableWidget();
ui->layoutEtapes->addWidget(m_tableEtapes);  // ou dans votre layout

// Conteneur pour la timeline (peut être dans un autre onglet)
m_timelineContainer = new QWidget();
QVBoxLayout *containerLayout = new QVBoxLayout(m_timelineContainer);
ui->layoutTimeline->addWidget(m_timelineContainer);

// Initialiser la liste (elle sera remplie lors de chargerEtapes)
m_etapesList.clear();
*/

// ============================================================================
// ÉTAPE 3 : APPELER chargerEtapes() QUAND UNE FABRICATION EST SÉLECTIONNÉE
// ============================================================================

/*
Exemple 1: Dans un slot de sélection de tableau
void MainWindow::on_tableFabrications_cellClicked(int row, int column)
{
    // Récupérer l'ID de la fabrication (supposé être en colonne 0)
    int idFab = ui->tableFabrications->item(row, 0)->text().toInt();
    
    // Charger et afficher les étapes
    chargerEtapes(idFab);
}

Exemple 2: Dans un slot de double-clic
void MainWindow::on_btnAfficherEtapes_clicked()
{
    int idFab = 42;  // À remplacer par votre logique de récupération
    chargerEtapes(idFab);
}

Exemple 3: À partir d'une combobox
void MainWindow::on_cbFabrications_currentIndexChanged(int index)
{
    if (index >= 0) {
        int idFab = ui->cbFabrications->itemData(index).toInt();
        chargerEtapes(idFab);
    }
}
*/

// ============================================================================
// ÉTAPE 4 : UTILISER LES DONNÉES CHARGÉES
// ============================================================================

/*
// Accéder aux données chargées dans m_etapesList:

for (const EtapeData &etape : m_etapesList) {
    qDebug() << "Étape" << etape.ordre 
             << etape.nom 
             << "État:" << etape.etat;
}

// Obtenir l'état d'une étape spécifique:
QString etat = obtenirEtatEtape(5);  // ID de l'étape = 5

// Afficher la couleur pour un état:
QColor couleur = couleurEtatEtape("EN_COURS");
*/

// ============================================================================
// ÉTAPE 5 : GESTION DE LA BASE DE DONNÉES
// ============================================================================

/*
POINT IMPORTANT:
- Aucune modification du schéma n'est nécessaire
- La colonne ETAT_ETAPE est calculée dynamiquement en SQL
- Les valeurs possibles:
  * NON_PLANIFIEE : Pas encore commencée (DATEDEBUT = NULL)
  * PLANIFIEE     : Programmée mais non débutée
  * EN_COURS      : En execution (temps réel > 0, pas de fin)
  * TERMINEE      : Complétée dans les temps
  * EN_RETARD     : Complétée mais en dépassement horaire

Table ETAPE originale (inchangée):
- IDETAPE
- CIN
- IDFABRICATION
- NOMETAPE
- ORDRE
- TEMPSESTIME
- TEMPSREEL
- DATEDEBUT
- DATEFIN

Pas de colonne ETAT ajoutée = Flexibilité maximale
*/

// ============================================================================
// ÉTAPE 6 : COULEURS ET THÈME
// ============================================================================

/*
Mapping des couleurs:

NON_PLANIFIEE → RGB(220, 53, 69)    // Rouge vif (#DC3545)
PLANIFIEE     → RGB(255, 193, 7)    // Orange (#FFC107)
EN_COURS      → RGB(255, 235, 59)   // Jaune clair (#FFEB3B)
TERMINEE      → RGB(40, 167, 69)    // Vert (#28A745)
EN_RETARD     → RGB(139, 0, 0)      // Rouge foncé (#8B0000)

Pour personnaliser, modifiez dans couleurEtatEtape():

    QColor MainWindow::couleurEtatEtape(const QString &etat)
    {
        if (etat == "TERMINEE") {
            return QColor(40, 167, 69);  // Votre couleur ici
        }
        // ...
    }
*/

// ============================================================================
// ÉTAPE 7 : AFFICHAGE - TABLEAU
// ============================================================================

/*
Le tableau affiche 9 colonnes:
1. ID Etape          (80px)
2. ID Fab            (70px)
3. Nom Etape         (150px, flexible)
4. Ordre             (70px)
5. Date Début        (110px)
6. Date Fin          (110px)
7. Temps Estimé      (90px)
8. Temps Réel        (90px)
9. Statut            (120px, COLOR-CODED)

Caractéristiques:
- Lignes non éditables
- Couleurs alternées pour meilleure lisibilité
- Hauteur de ligne: 32px
- La colonne Statut a fond coloré + texte blanc/noir selon contraste
- Sélection par ligne (pas par cellule)
*/

// ============================================================================
// ÉTAPE 8 : AFFICHAGE - TIMELINE
// ============================================================================

/*
La timeline affiche chaque étape comme une CARTE:

┌─────────────────┐      ┌─────────────────┐      ┌─────────────────┐
│   Étape 1       │  →   │   Étape 2       │  →   │   Étape 3       │
│ Nom de l'étape  │      │ Nom de l'étape  │      │ Nom de l'étape  │
│                 │      │                 │      │                 │
│ Début: 01/01... │      │ Début: 05/01... │      │ Début: 10/01... │
│ Fin: —          │      │ Fin: 04/01...   │      │ Fin: 09/01...   │
│                 │      │                 │      │                 │
│                 │      │                 │      │                 │
│ Planifiée       │      │  Terminée       │      │    En Retard    │
│  (Orange)       │      │   (Vert)        │      │  (Rouge foncé)  │
└─────────────────┘      └─────────────────┘      └─────────────────┘

Caractéristiques:
- Horizontal scrollable
- Cartes de 200-250px de large
- Couleur du badge basée sur l'état
- Dates formatées (DD/MM/YYYY)
- Temps en heures
- Flèches entre les étapes (→)
*/

// ============================================================================
// ÉTAPE 9 : OPTIMISATIONS POSSIBLES
// ============================================================================

/*
1. CACHE DE REQUÊTE:
   - Garder en mémoire les m_etapesList pour accès rapide
   - Recharger seulement si fabrication change

2. PAGINATION:
   - Si tableau très grand, paginer le chargement
   - chargerEtapes(int idFab, int page = 0, int pageSize = 50)

3. RECHERCHE/FILTRAGE:
   - Ajouter un QLineEdit pour filtrer par nom d'étape
   - Implémenter une fonction filterEtapes(const QString &filtre)

4. EXPORT:
   - Exporter les données vers CSV/PDF
   - void exporterEtapesCSV(const QString &chemin)

5. MISE À JOUR EN TEMPS RÉEL:
   - Si TEMPSREEL change en DB, mettre à jour le statut
   - Ajouter un timer QTimer pour recharger périodiquement

6. STATISTIQUES:
   - Afficher résumé: total étapes, complétées, en retard
   - calculateEtapesStatistics() → QMap<QString, int>
*/

// ============================================================================
// ÉTAPE 10 : GESTION D'ERREURS
// ============================================================================

/*
La fonction chargerEtapes() gère déjà:

1. Erreur de connexion DB:
   if (!query.exec()) {
       qWarning() << "Erreur lors du chargement des étapes :" 
                  << query.lastError().text();
       return;
   }

2. Widget non initialisé:
   if (!m_tableEtapes) {
       qWarning() << "Tableau m_tableEtapes non initialisé";
       return;
   }

3. Date invalide:
   if (dateValue.isNull() || !dateValue.isValid()) {
       return "—";
   }

Pour améliorer la robustesse, vous pouvez ajouter:

   - Afficher un message d'erreur au utilisateur (QMessageBox)
   - Logger les erreurs dans un fichier
   - Implémenter un retry automatique
*/

// ============================================================================
// ÉTAPE 11 : EXEMPLE COMPLET D'UTILISATION
// ============================================================================

/*
// Dans mainwindow.cpp

void MainWindow::on_btnChargerFabrication_clicked()
{
    // 1. Récupérer l'ID de la fabrication sélectionnée
    QTableWidget *table = ui->tableFabrications;
    if (table->currentRow() < 0) {
        QMessageBox::warning(this, "Attention", "Sélectionnez une fabrication");
        return;
    }
    
    int idFab = table->item(table->currentRow(), 0)->text().toInt();
    
    // 2. Charger les étapes
    chargerEtapes(idFab);
    
    // 3. Les données sont maintenant disponibles dans m_etapesList
    qDebug() << "Étapes chargées:" << m_etapesList.size();
    
    // 4. Accéder aux étapes
    for (const EtapeData &etape : m_etapesList) {
        qDebug() << "Étape" << etape.ordre << ":" 
                 << etape.nom << "("  << etape.etat << ")";
    }
}

void MainWindow::on_tableEtapes_cellClicked(int row, int column)
{
    if (row >= 0 && row < m_etapesList.size()) {
        const EtapeData &etape = m_etapesList.at(row);
        
        qDebug() << "Étape cliquée:" << etape.nom;
        qDebug() << "État:" << etape.etat;
        qDebug() << "Date début:" << etape.dateDebut.toString("dd/MM/yyyy");
        qDebug() << "Temps réel:" << etape.tempsReel << "heures";
        
        // Faire quelque chose avec cette étape...
    }
}

void MainWindow::nettoyerEtapesAvantNouvelleFabrication()
{
    // Nettoyer avant charger une nouvelle fabrication
    nettoyerEtapes();
    
    // m_etapesList est vidée
    // m_tableEtapes et m_timelineContainer sont vidés
}
*/

// ============================================================================
// ÉTAPE 12 : NOTES TECHNIQUES
// ============================================================================

/*
AVANTAGES DE CETTE APPROCHE:

✓ Pas de modification du schéma DB
✓ Calcul du statut complètement en SQL
✓ Performance: requête unique, pas de boucles
✓ Flexibilité: règles métier dans le SQL, facile à modifier
✓ Scalabilité: les états restent cohérents en DB
✓ Maintenabilité: code bien structuré, commenté
✓ UI riche: couleurs, carte visuelle, tableau détaillé

LIMITATIONS:

✗ Les statuts sont calculés au moment du SELECT (pas de notification temps réel)
✗ Si TEMPSREEL change en DB, vous devez recharger pour voir le nouvel état
✗ Pas d'historique des changements d'état (optionnel à implémenter)

FUTURS AMÉLIOREMENTS:

→ Ajouter un historique ETAPE_HISTORIQUE (optionnel)
→ Implémenter un système de notification (signaux Qt)
→ Ajouter une barre de progression globale
→ Implémenter drag-drop pour réordonner les étapes
→ Ajouter notes/commentaires par étape
*/

// ============================================================================
// ÉTAPE 13 : REQUÊTE SQL ALTERNATIVE (INDEX)
// ============================================================================

/*
Pour améliorer la performance avec de nombreuses étapes:

CREATE INDEX idx_etape_idfab ON ETAPE(IDFABRICATION, ORDRE);
CREATE INDEX idx_etape_datedebut ON ETAPE(DATEDEBUT, DATEFIN);

Cela accélère la recherche par IDFABRICATION et le tri par ORDRE.
*/

// ============================================================================
// ÉTAPE 14 : TEST DANS SQL DEVELOPER
// ============================================================================

/*
Avant d'intégrer, testez directement en SQL:

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
WHERE IDFABRICATION = 42
ORDER BY ORDRE ASC;

Exécutez avec IDFABRICATION = 42 (ou votre ID de test)
Vérifiez que les statuts sont corrects pour chaque étape.
*/

// ============================================================================
// FIN DU GUIDE D'INTÉGRATION
// ============================================================================
