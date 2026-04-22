// ============================================================================
// IMPLEMENTATION COMPLETE : Gestion des Etapes avec Statut Dynamique
// ============================================================================
// Ce fichier contient l'implémentation complète pour charger et afficher
// les étapes (ETAPE) avec calcul dynamique du statut via SQL CASE
// Sans modification du schéma de la base de données
// ============================================================================

// ============================================================================
// 1. REQUÊTE SQL ORACLE - CALCUL DE L'ETAT_ETAPE DYNAMIQUE
// ============================================================================

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
WHERE IDFABRICATION = :idFabrication
ORDER BY ORDRE ASC
*/

// ============================================================================
// 2. FONCTIONS HELPER - COULEURS ET FORMATAGE
// ============================================================================

/**
 * @brief Retourne la couleur associée au statut de l'étape
 * @param etat Le statut de l'étape (NON_PLANIFIEE, PLANIFIEE, EN_COURS, etc.)
 * @return QColor La couleur correspondante au statut
 */
QColor MainWindow::couleurEtatEtape(const QString &etat)
{
    if (etat == "NON_PLANIFIEE") {
        return QColor(220, 53, 69);           // Rouge : #DC3545
    }
    else if (etat == "PLANIFIEE") {
        return QColor(255, 193, 7);           // Orange : #FFC107
    }
    else if (etat == "EN_COURS") {
        return QColor(255, 235, 59);          // Jaune clair : #FFEB3B
    }
    else if (etat == "TERMINEE") {
        return QColor(40, 167, 69);           // Vert : #28A745
    }
    else if (etat == "EN_RETARD") {
        return QColor(139, 0, 0);             // Rouge foncé : #8B0000
    }
    else {
        return QColor(108, 117, 125);         // Gris : #6C757D
    }
}

/**
 * @brief Retourne la couleur de texte en blanc pour meilleure visibilité
 * @param couleurFond La couleur du fond
 * @return QColor Blanc ou noir selon le contraste
 */
QColor MainWindow::couleurTexteContraste(const QColor &couleurFond)
{
    // Calcul de la luminosité (Y = 0.299*R + 0.587*G + 0.114*B)
    int luminosite = (0.299 * couleurFond.red() + 
                      0.587 * couleurFond.green() + 
                      0.114 * couleurFond.blue());
    
    return (luminosite > 128) ? QColor(0, 0, 0) : QColor(255, 255, 255);
}

/**
 * @brief Formate une date QVariant en QString
 * @param dateValue La valeur date de la base de données
 * @return QString La date formatée (format : dd/MM/yyyy)
 */
QString MainWindow::formaterDate(const QVariant &dateValue)
{
    if (dateValue.isNull() || !dateValue.isValid()) {
        return "—";
    }
    
    QDate date = dateValue.toDate();
    if (!date.isValid()) {
        return "—";
    }
    
    return date.toString("dd/MM/yyyy");
}

/**
 * @brief Retourne un label lisible pour le statut
 * @param etat Le statut technique
 * @return QString Le libellé affiché
 */
QString MainWindow::libelleurEtatEtape(const QString &etat)
{
    if (etat == "NON_PLANIFIEE") return "Non Planifiée";
    else if (etat == "PLANIFIEE") return "Planifiée";
    else if (etat == "EN_COURS") return "En Cours";
    else if (etat == "TERMINEE") return "Terminée";
    else if (etat == "EN_RETARD") return "En Retard";
    else return "Inconnue";
}

// ============================================================================
// 3. FONCTION PRINCIPALE - CHARGER LES ETAPES
// ============================================================================

/**
 * @brief Charge les étapes d'une fabrication et les affiche dans le tableau
 * avec color-coding dynamique basé sur le statut calculé
 * 
 * @param idFabrication L'ID de la fabrication dont charger les étapes
 */
void MainWindow::chargerEtapes(int idFabrication)
{
    // Initialiser la liste pour la timeline
    m_etapesList.clear();
    
    // Préparation de la requête SQL avec calcul du statut dynamique
    QSqlQuery query(Connexion::createInstance()->getBdd());
    
    QString sqlQuery = QString(
        "SELECT "
        "    IDETAPE, "
        "    IDFABRICATION, "
        "    NOMETAPE, "
        "    ORDRE, "
        "    DATEDEBUT, "
        "    DATEFIN, "
        "    TEMPSESTIME, "
        "    TEMPSREEL, "
        "    CASE "
        "        WHEN DATEDEBUT IS NULL "
        "            THEN 'NON_PLANIFIEE' "
        "        WHEN DATEDEBUT IS NOT NULL "
        "             AND NVL(TEMPSREEL, 0) = 0 "
        "             AND DATEFIN IS NULL "
        "            THEN 'PLANIFIEE' "
        "        WHEN DATEDEBUT IS NOT NULL "
        "             AND NVL(TEMPSREEL, 0) > 0 "
        "             AND DATEFIN IS NULL "
        "            THEN 'EN_COURS' "
        "        WHEN DATEFIN IS NOT NULL "
        "             AND NVL(TEMPSREEL, 0) <= NVL(TEMPSESTIME, 0) "
        "            THEN 'TERMINEE' "
        "        WHEN DATEFIN IS NOT NULL "
        "             AND NVL(TEMPSREEL, 0) > NVL(TEMPSESTIME, 0) "
        "            THEN 'EN_RETARD' "
        "        ELSE 'INCONNUE' "
        "    END AS ETAT_ETAPE "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :idFabrication "
        "ORDER BY ORDRE ASC "
    );
    
    query.prepare(sqlQuery);
    query.addBindValue(idFabrication);
    
    // Exécution de la requête
    if (!query.exec()) {
        qWarning() << "Erreur lors du chargement des étapes :" << query.lastError().text();
        return;
    }
    
    // Configuration du tableau (supposé existant : m_tableEtapes)
    if (!m_tableEtapes) {
        qWarning() << "Tableau m_tableEtapes non initialisé";
        return;
    }
    
    // Initialisation du tableau
    m_tableEtapes->setColumnCount(9);
    m_tableEtapes->setRowCount(0);
    
    QStringList headers;
    headers << "ID Etape" << "ID Fab" << "Nom Etape" << "Ordre" 
            << "Date Début" << "Date Fin" << "Temps Est." << "Temps Réel" << "Statut";
    
    m_tableEtapes->setHorizontalHeaderLabels(headers);
    m_tableEtapes->horizontalHeader()->setStretchLastSection(false);
    
    // Configuration des largeurs de colonnes
    m_tableEtapes->setColumnWidth(0, 80);   // ID Etape
    m_tableEtapes->setColumnWidth(1, 70);   // ID Fab
    m_tableEtapes->setColumnWidth(2, 150);  // Nom Etape
    m_tableEtapes->setColumnWidth(3, 70);   // Ordre
    m_tableEtapes->setColumnWidth(4, 110);  // Date Début
    m_tableEtapes->setColumnWidth(5, 110);  // Date Fin
    m_tableEtapes->setColumnWidth(6, 90);   // Temps Est.
    m_tableEtapes->setColumnWidth(7, 90);   // Temps Réel
    m_tableEtapes->setColumnWidth(8, 120);  // Statut
    
    m_tableEtapes->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableEtapes->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableEtapes->setAlternatingRowColors(true);
    m_tableEtapes->verticalHeader()->setDefaultSectionSize(32);
    
    // Remplissage du tableau
    int rowIndex = 0;
    
    while (query.next()) {
        m_tableEtapes->insertRow(rowIndex);
        
        // Extraction des données
        int idEtape = query.value(0).toInt();
        int idFab = query.value(1).toInt();
        QString nomEtape = query.value(2).toString();
        int ordre = query.value(3).toInt();
        QDate dateDebut = query.value(4).toDate();
        QDate dateFin = query.value(5).toDate();
        int tempsEstime = query.value(6).toInt();
        int tempsReel = query.value(7).toInt();
        QString etatEtape = query.value(8).toString();
        
        // Création d'une structure pour la timeline
        EtapeData etape;
        etape.id = idEtape;
        etape.idFabrication = idFab;
        etape.nom = nomEtape;
        etape.ordre = ordre;
        etape.dateDebut = dateDebut;
        etape.dateFin = dateFin;
        etape.tempsEstime = tempsEstime;
        etape.tempsReel = tempsReel;
        etape.etat = etatEtape;
        
        m_etapesList.append(etape);
        
        // Insertion dans le tableau
        QTableWidgetItem *item;
        
        // Colonne 0 : ID Etape
        item = new QTableWidgetItem(QString::number(idEtape));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_tableEtapes->setItem(rowIndex, 0, item);
        
        // Colonne 1 : ID Fabrication
        item = new QTableWidgetItem(QString::number(idFab));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_tableEtapes->setItem(rowIndex, 1, item);
        
        // Colonne 2 : Nom Etape
        item = new QTableWidgetItem(nomEtape);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_tableEtapes->setItem(rowIndex, 2, item);
        
        // Colonne 3 : Ordre
        item = new QTableWidgetItem(QString::number(ordre));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_tableEtapes->setItem(rowIndex, 3, item);
        
        // Colonne 4 : Date Début
        item = new QTableWidgetItem(formaterDate(dateDebut));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_tableEtapes->setItem(rowIndex, 4, item);
        
        // Colonne 5 : Date Fin
        item = new QTableWidgetItem(formaterDate(dateFin));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_tableEtapes->setItem(rowIndex, 5, item);
        
        // Colonne 6 : Temps Estimé
        item = new QTableWidgetItem(QString::number(tempsEstime));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_tableEtapes->setItem(rowIndex, 6, item);
        
        // Colonne 7 : Temps Réel
        item = new QTableWidgetItem(QString::number(tempsReel));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_tableEtapes->setItem(rowIndex, 7, item);
        
        // Colonne 8 : Statut (avec color-coding)
        item = new QTableWidgetItem(libelleurEtatEtape(etatEtape));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        
        // Récupération de la couleur pour ce statut
        QColor couleur = couleurEtatEtape(etatEtape);
        QColor couleurTexte = couleurTexteContraste(couleur);
        
        // Application du color-coding à la cellule du statut
        item->setBackground(QBrush(couleur));
        item->setForeground(QBrush(couleurTexte));
        item->setFont(QFont("Arial", 10, QFont::Bold));
        
        // Alignement du texte au centre
        item->setTextAlignment(Qt::AlignCenter);
        
        m_tableEtapes->setItem(rowIndex, 8, item);
        
        // Application du color-coding à la ligne entière (optionnel)
        // Décommentez si vous préférez colorer toute la ligne
        /*
        for (int col = 0; col < 9; ++col) {
            m_tableEtapes->item(rowIndex, col)->setBackground(QBrush(couleur));
            m_tableEtapes->item(rowIndex, col)->setForeground(QBrush(couleurTexte));
        }
        */
        
        rowIndex++;
    }
    
    // Redimensionner les colonnes pour le contenu
    m_tableEtapes->resizeColumnsToContents();
    
    // Construire la timeline visuelle
    afficherTimelineEtapes();
}

// ============================================================================
// 4. STRUCTURE POUR STOCKER LES DONNÉES DES ÉTAPES
// ============================================================================

// À ajouter dans mainwindow.h (section private):
/*
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

// Dans la classe MainWindow:
private:
    QList<EtapeData> m_etapesList;
    QTableWidget *m_tableEtapes;
    QWidget *m_timelineContainer;
*/

// ============================================================================
// 5. TIMELINE VISUELLE - AFFICHAGE DES ÉTAPES COMME CARTES COLORÉES
// ============================================================================

/**
 * @brief Crée et affiche une timeline visuelle des étapes
 * Chaque étape apparaît comme une carte colorée selon son statut
 */
void MainWindow::afficherTimelineEtapes()
{
    // Supposons que m_timelineContainer est un QWidget dans le layout
    if (!m_timelineContainer) {
        qWarning() << "Conteneur timeline non initialisé";
        return;
    }
    
    // Vider le layout existant
    if (m_timelineContainer->layout()) {
        QLayoutItem *item;
        while ((item = m_timelineContainer->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    } else {
        m_timelineContainer->setLayout(new QVBoxLayout());
    }
    
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(m_timelineContainer->layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(m_timelineContainer);
    }
    
    // Créer un conteneur horizontal pour les étapes (scroll si nécessaire)
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    
    QWidget *scrollWidget = new QWidget();
    QHBoxLayout *timelineLayout = new QHBoxLayout(scrollWidget);
    timelineLayout->setSpacing(15);
    timelineLayout->setContentsMargins(10, 10, 10, 10);
    
    // Créer une carte pour chaque étape
    for (int i = 0; i < m_etapesList.size(); ++i) {
        const EtapeData &etape = m_etapesList.at(i);
        QWidget *carteEtape = creerCarteEtape(etape, i == m_etapesList.size() - 1);
        timelineLayout->addWidget(carteEtape);
        
        // Ajouter une flèche entre les étapes (sauf après la dernière)
        if (i < m_etapesList.size() - 1) {
            QLabel *fleche = new QLabel("→");
            fleche->setFont(QFont("Arial", 16, QFont::Bold));
            fleche->setStyleSheet("color: #888888;");
            fleche->setAlignment(Qt::AlignCenter);
            timelineLayout->addWidget(fleche);
        }
    }
    
    timelineLayout->addStretch();
    
    scrollArea->setWidget(scrollWidget);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "    border: 1px solid #CCCCCC;"
        "    background-color: #F5F5F5;"
        "}"
    );
    
    mainLayout->addWidget(scrollArea);
}

/**
 * @brief Crée une carte visuelle pour une étape
 * @param etape Les données de l'étape
 * @param estDerniere Vrai si c'est la dernière étape
 * @return QWidget* Un widget représentant la carte
 */
QWidget *MainWindow::creerCarteEtape(const EtapeData &etape, bool estDerniere)
{
    // Créer le frame principal
    QFrame *carte = new QFrame();
    carte->setStyleSheet(
        "QFrame {"
        "    border: 2px solid #999999;"
        "    border-radius: 8px;"
        "    background-color: white;"
        "    padding: 12px;"
        "}"
    );
    carte->setMinimumWidth(200);
    carte->setMaximumWidth(250);
    carte->setMinimumHeight(180);
    
    QVBoxLayout *layout = new QVBoxLayout(carte);
    layout->setSpacing(6);
    layout->setContentsMargins(8, 8, 8, 8);
    
    // Récupérer la couleur du statut
    QColor couleur = couleurEtatEtape(etape.etat);
    QColor couleurTexte = couleurTexteContraste(couleur);
    
    // Titre avec le numéro d'ordre
    QLabel *titre = new QLabel(QString("Étape %1").arg(etape.ordre));
    titre->setFont(QFont("Arial", 11, QFont::Bold));
    titre->setStyleSheet("color: #333333;");
    layout->addWidget(titre);
    
    // Nom de l'étape
    QLabel *nom = new QLabel(etape.nom);
    nom->setFont(QFont("Arial", 10));
    nom->setWordWrap(true);
    nom->setStyleSheet("color: #555555;");
    layout->addWidget(nom);
    
    // Séparateur
    QFrame *separateur = new QFrame();
    separateur->setFrameShape(QFrame::HLine);
    separateur->setStyleSheet("color: #DDDDDD;");
    layout->addWidget(separateur);
    
    // Dates et temps en petit
    QLabel *dates = new QLabel();
    QString texteDate = QString("Début: %1\nFin: %2")
        .arg(formaterDate(etape.dateDebut))
        .arg(formaterDate(etape.dateFin));
    dates->setText(texteDate);
    dates->setFont(QFont("Arial", 8));
    dates->setStyleSheet("color: #777777;");
    layout->addWidget(dates);
    
    // Durée
    if (etape.tempsReel > 0 || etape.tempsEstime > 0) {
        QLabel *duree = new QLabel();
        QString texteDuree = QString("Est: %1h | Réel: %2h")
            .arg(etape.tempsEstime)
            .arg(etape.tempsReel);
        duree->setText(texteDuree);
        duree->setFont(QFont("Arial", 8));
        duree->setStyleSheet("color: #777777;");
        layout->addWidget(duree);
    }
    
    layout->addStretch();
    
    // Badge du statut en bas
    QLabel *badge = new QLabel(libelleurEtatEtape(etape.etat));
    badge->setAlignment(Qt::AlignCenter);
    badge->setFont(QFont("Arial", 9, QFont::Bold));
    badge->setStyleSheet(QString(
        "QLabel {"
        "    background-color: %1;"
        "    color: %2;"
        "    padding: 6px;"
        "    border-radius: 4px;"
        "    border: 1px solid %3;"
        "}"
    ).arg(couleur.name())
     .arg(couleurTexte.name())
     .arg(couleur.darker(120).name()));
    
    layout->addWidget(badge);
    
    return carte;
}

// ============================================================================
// 6. FONCTION POUR OBTENIR LE STATUT D'UNE ÉTAPE PAR ID
// ============================================================================

/**
 * @brief Retourne le statut d'une étape spécifique
 * @param idEtape L'ID de l'étape
 * @return QString Le statut de l'étape (ou QString vide si non trouvé)
 */
QString MainWindow::obtenirEtatEtape(int idEtape) const
{
    for (const EtapeData &etape : m_etapesList) {
        if (etape.id == idEtape) {
            return etape.etat;
        }
    }
    return QString();
}

// ============================================================================
// 7. FONCTION POUR NETTOYER LES RESSOURCES
// ============================================================================

/**
 * @brief Nettoie la liste des étapes
 */
void MainWindow::nettoyerEtapes()
{
    m_etapesList.clear();
    
    if (m_tableEtapes) {
        m_tableEtapes->setRowCount(0);
    }
    
    if (m_timelineContainer && m_timelineContainer->layout()) {
        QLayoutItem *item;
        while ((item = m_timelineContainer->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    }
}

// ============================================================================
// FIN DE L'IMPLÉMENTATION
// ============================================================================
