#include "MainWindow.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QStatusBar>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), selectedEtapeId(-1) {
    
    if (!dbManager.openDatabase()) {
        QMessageBox::critical(this, "Erreur Base de Données", 
            "Échec de l'ouverture de la base de données!");
        return;
    }
    
    setupUI();
    loadEtapes();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    setWindowTitle("WoodPilot - Gestion Des Étapes De Fabrication");
    resize(1200, 750);
    
    // Apply wood-themed stylesheet
    setStyleSheet(StyleSheet::getStyleSheet());
    
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Create tab widget
    tabWidget = new QTabWidget();
    tabWidget->addTab(createGestionTab(), "Gestion Des Étapes");
    tabWidget->addTab(createStatistiquesTab(), "Statistiques");
    
    mainLayout->addWidget(tabWidget);
    
    // Status bar
    statusLabel = new QLabel("Prêt");
    QStatusBar* statusBar = new QStatusBar();
    statusBar->addWidget(statusLabel);
    setStatusBar(statusBar);
    
    setCentralWidget(centralWidget);
}

QWidget* MainWindow::createGestionTab() {
    QWidget* gestionWidget = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(gestionWidget);
    
    // Left side - Form with scroll
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget* formContainer = new QWidget();
    QVBoxLayout* formContainerLayout = new QVBoxLayout(formContainer);
    
    QGroupBox* formGroup = new QGroupBox("Détails de l'Étape");
    QVBoxLayout* formLayout = new QVBoxLayout();
    
    QFormLayout* inputForm = new QFormLayout();
    
    // ID (read-only)
    idEdit = new QLineEdit();
    idEdit->setReadOnly(true);
    idEdit->setPlaceholderText("Auto");
    inputForm->addRow("ID :", idEdit);
    
    // Nom de l'étape (obligatoire)
    nomEdit = new QLineEdit();
    nomEdit->setPlaceholderText("Ex: Découpage de bois");
    inputForm->addRow("Nom de l'étape* :", nomEdit);
    
    // Produit concerné
    produitCombo = new QComboBox();
    produitCombo->addItems({"Table", "Chaise", "Armoire", "Étagère", "Bureau", "Lit", "Autre"});
    produitCombo->setEditable(true);
    inputForm->addRow("Produit :", produitCombo);
    
    // Numéro/Ordre
    ordreEdit = new QLineEdit();
    ordreEdit->setPlaceholderText("1, 2, 3...");
    inputForm->addRow("Ordre (N°) :", ordreEdit);
    
    // Temps estimé (minutes)
    tempsEdit = new QLineEdit();
    tempsEdit->setPlaceholderText("En minutes (ex: 60)");
    inputForm->addRow("Temps estimé (min) :", tempsEdit);
    
    // Date début
    dateDebutEdit = new QDateEdit();
    dateDebutEdit->setDate(QDate::currentDate());
    dateDebutEdit->setCalendarPopup(true);
    dateDebutEdit->setDisplayFormat("dd/MM/yyyy");
    inputForm->addRow("Date de début :", dateDebutEdit);
    
    // Date fin
    dateFinEdit = new QDateEdit();
    dateFinEdit->setDate(QDate::currentDate());
    dateFinEdit->setCalendarPopup(true);
    dateFinEdit->setDisplayFormat("dd/MM/yyyy");
    inputForm->addRow("Date de fin :", dateFinEdit);
    
    // État
    etatCombo = new QComboBox();
    etatCombo->addItems({"Non commencé", "En cours", "Terminé"});
    inputForm->addRow("État :", etatCombo);
    
    formLayout->addLayout(inputForm);
    
    // Description (multiline)
    QLabel* descLabel = new QLabel("Description détaillée :");
    formLayout->addWidget(descLabel);
    descriptionEdit = new QTextEdit();
    descriptionEdit->setPlaceholderText("Détails de l'étape de fabrication...");
    descriptionEdit->setMaximumHeight(100);
    formLayout->addWidget(descriptionEdit);
    
    formLayout->addSpacing(10);
    
    // Buttons
    QHBoxLayout* buttonLayout1 = new QHBoxLayout();
    addButton = new QPushButton("Ajouter");
    addButton->setObjectName("addButton");
    updateButton = new QPushButton("Modifier");
    updateButton->setObjectName("updateButton");
    updateButton->setEnabled(false);
    
    buttonLayout1->addWidget(addButton);
    buttonLayout1->addWidget(updateButton);
    formLayout->addLayout(buttonLayout1);
    
    QHBoxLayout* buttonLayout2 = new QHBoxLayout();
    deleteButton = new QPushButton("Supprimer");
    deleteButton->setObjectName("deleteButton");
    deleteButton->setEnabled(false);
    clearButton = new QPushButton("Effacer");
    
    buttonLayout2->addWidget(deleteButton);
    buttonLayout2->addWidget(clearButton);
    formLayout->addLayout(buttonLayout2);
    
    formGroup->setLayout(formLayout);
    formContainerLayout->addWidget(formGroup);
    formContainerLayout->addStretch();
    
    scrollArea->setWidget(formContainer);
    
    // Right side - Table and Search
    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    
    // Search and Filter section
    QGroupBox* searchGroup = new QGroupBox("Recherche et Filtres");
    QVBoxLayout* searchMainLayout = new QVBoxLayout();
    
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Rechercher...");
    searchButton = new QPushButton("Chercher");
    searchButton->setObjectName("searchButton");
    QPushButton* showAllButton = new QPushButton("Afficher Tout");
    
    searchLayout->addWidget(new QLabel("Recherche:"));
    searchLayout->addWidget(searchEdit, 2);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(showAllButton);
    searchMainLayout->addLayout(searchLayout);
    
    // Filter by product
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterProduitCombo = new QComboBox();
    filterProduitCombo->addItems({"Tous les produits", "Table", "Chaise", "Armoire", "Étagère", "Bureau", "Lit"});
    QPushButton* filterButton = new QPushButton("Filtrer");
    
    filterLayout->addWidget(new QLabel("Filtrer par produit:"));
    filterLayout->addWidget(filterProduitCombo, 1);
    filterLayout->addWidget(filterButton);
    searchMainLayout->addLayout(filterLayout);
    
    searchGroup->setLayout(searchMainLayout);
    rightLayout->addWidget(searchGroup);
    
    // Table
    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(9);
    tableWidget->setHorizontalHeaderLabels({"ID", "Étape", "Produit", "Ordre", "Temps (min)", "Date Début", "Date Fin", "État", "Description"});
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->verticalHeader()->setVisible(false);
    
    // Set column widths
    tableWidget->setColumnWidth(0, 50);
    tableWidget->setColumnWidth(1, 150);
    tableWidget->setColumnWidth(2, 100);
    tableWidget->setColumnWidth(3, 60);
    tableWidget->setColumnWidth(4, 90);
    tableWidget->setColumnWidth(5, 100);
    tableWidget->setColumnWidth(6, 100);
    tableWidget->setColumnWidth(7, 100);
    
    rightLayout->addWidget(tableWidget);
    
    // Add to main layout
    mainLayout->addWidget(scrollArea, 1);
    mainLayout->addWidget(rightWidget, 2);
    
    // Connect signals
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddEtape);
    connect(updateButton, &QPushButton::clicked, this, &MainWindow::onUpdateEtape);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteEtape);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearForm);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchEtapes);
    connect(showAllButton, &QPushButton::clicked, this, &MainWindow::onRefreshTable);
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::onFilterByProduit);
    connect(tableWidget, &QTableWidget::itemSelectionChanged, 
            this, &MainWindow::onTableSelectionChanged);
    
    return gestionWidget;
}

QWidget* MainWindow::createStatistiquesTab() {
    QWidget* statsWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(statsWidget);
    
    QGroupBox* statsGroup = new QGroupBox("Statistiques des Étapes de Fabrication");
    QVBoxLayout* statsLayout = new QVBoxLayout();
    
    QLabel* infoLabel = new QLabel(
        "📊 <b>Statistiques disponibles ici...</b><br><br>"
        "Vous pouvez ajouter:<br>"
        "• Nombre total d'étapes par produit<br>"
        "• Temps total de fabrication<br>"
        "• Étapes en cours vs terminées<br>"
        "• Graphiques de progression<br>"
        "• Délais moyens par étape<br>"
        "• Analyse de performance"
    );
    infoLabel->setStyleSheet("font-size: 12pt; padding: 20px;");
    
    statsLayout->addWidget(infoLabel);
    statsLayout->addStretch();
    
    statsGroup->setLayout(statsLayout);
    layout->addWidget(statsGroup);
    
    return statsWidget;
}

void MainWindow::loadEtapes() {
    tableWidget->setRowCount(0);
    QVector<Etape> etapes = dbManager.readAllEtapes();
    
    for (const Etape& etape : etapes) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(etape.getId())));
        tableWidget->setItem(row, 1, new QTableWidgetItem(etape.getNom()));
        tableWidget->setItem(row, 2, new QTableWidgetItem(etape.getProduit()));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(etape.getOrdre())));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(etape.getTempsEstime())));
        tableWidget->setItem(row, 5, new QTableWidgetItem(etape.getDateDebut().toString("dd/MM/yyyy")));
        tableWidget->setItem(row, 6, new QTableWidgetItem(etape.getDateFin().toString("dd/MM/yyyy")));
        tableWidget->setItem(row, 7, new QTableWidgetItem(etape.getEtat()));
        tableWidget->setItem(row, 8, new QTableWidgetItem(etape.getDescription()));
        
        // Center align
        tableWidget->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 3)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 4)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 7)->setTextAlignment(Qt::AlignCenter);
    }
    
    statusLabel->setText(QString("Total: %1 étape(s)").arg(etapes.size()));
}

void MainWindow::onAddEtape() {
    QString nom = nomEdit->text().trimmed();
    QString produit = produitCombo->currentText().trimmed();
    QString ordreText = ordreEdit->text().trimmed();
    QString tempsText = tempsEdit->text().trimmed();
    QString description = descriptionEdit->toPlainText().trimmed();
    QDate dateDebut = dateDebutEdit->date();
    QDate dateFin = dateFinEdit->date();
    QString etat = etatCombo->currentText();
    
    // Validation
    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom de l'étape est obligatoire!");
        return;
    }
    
    bool ordreOk, tempsOk;
    int ordre = ordreText.toInt(&ordreOk);
    int temps = tempsText.toInt(&tempsOk);
    
    if (!ordreOk) ordre = 0;
    if (!tempsOk) temps = 0;
    
    Etape etape(0, nom, produit, ordre, temps, description, dateDebut, dateFin, etat);
    
    if (dbManager.createEtape(etape)) {
        statusLabel->setText("✓ Étape ajoutée avec succès!");
        loadEtapes();
        clearInputs();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout de l'étape!");
    }
}

void MainWindow::onUpdateEtape() {
    if (selectedEtapeId <= 0) {
        QMessageBox::warning(this, "Erreur", "Aucune étape sélectionnée!");
        return;
    }
    
    QString nom = nomEdit->text().trimmed();
    
    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom de l'étape est obligatoire!");
        return;
    }
    
    QString produit = produitCombo->currentText().trimmed();
    int ordre = ordreEdit->text().toInt();
    int temps = tempsEdit->text().toInt();
    QString description = descriptionEdit->toPlainText().trimmed();
    QDate dateDebut = dateDebutEdit->date();
    QDate dateFin = dateFinEdit->date();
    QString etat = etatCombo->currentText();
    
    Etape etape(selectedEtapeId, nom, produit, ordre, temps, description, dateDebut, dateFin, etat);
    
    if (dbManager.updateEtape(etape)) {
        statusLabel->setText("✓ Étape modifiée avec succès!");
        loadEtapes();
        clearInputs();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification!");
    }
}

void MainWindow::onDeleteEtape() {
    if (selectedEtapeId <= 0) {
        QMessageBox::warning(this, "Erreur", "Aucune étape sélectionnée!");
        return;
    }
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmer la Suppression", 
        "Êtes-vous sûr de vouloir supprimer cette étape?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (dbManager.deleteEtape(selectedEtapeId)) {
            statusLabel->setText("✓ Étape supprimée avec succès!");
            loadEtapes();
            clearInputs();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression!");
        }
    }
}

void MainWindow::onRefreshTable() {
    searchEdit->clear();
    filterProduitCombo->setCurrentIndex(0);
    loadEtapes();
}

void MainWindow::onSearchEtapes() {
    QString searchTerm = searchEdit->text().trimmed();
    
    if (searchTerm.isEmpty()) {
        loadEtapes();
        return;
    }
    
    tableWidget->setRowCount(0);
    QVector<Etape> etapes = dbManager.searchEtapes(searchTerm);
    
    for (const Etape& etape : etapes) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(etape.getId())));
        tableWidget->setItem(row, 1, new QTableWidgetItem(etape.getNom()));
        tableWidget->setItem(row, 2, new QTableWidgetItem(etape.getProduit()));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(etape.getOrdre())));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(etape.getTempsEstime())));
        tableWidget->setItem(row, 5, new QTableWidgetItem(etape.getDateDebut().toString("dd/MM/yyyy")));
        tableWidget->setItem(row, 6, new QTableWidgetItem(etape.getDateFin().toString("dd/MM/yyyy")));
        tableWidget->setItem(row, 7, new QTableWidgetItem(etape.getEtat()));
        tableWidget->setItem(row, 8, new QTableWidgetItem(etape.getDescription()));
        
        tableWidget->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 3)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 4)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 7)->setTextAlignment(Qt::AlignCenter);
    }
    
    statusLabel->setText(QString("Trouvé %1 étape(s)").arg(etapes.size()));
}

void MainWindow::onFilterByProduit() {
    QString produit = filterProduitCombo->currentText();
    
    if (produit == "Tous les produits") {
        loadEtapes();
        return;
    }
    
    tableWidget->setRowCount(0);
    QVector<Etape> etapes = dbManager.getEtapesByProduit(produit);
    
    for (const Etape& etape : etapes) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(etape.getId())));
        tableWidget->setItem(row, 1, new QTableWidgetItem(etape.getNom()));
        tableWidget->setItem(row, 2, new QTableWidgetItem(etape.getProduit()));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(etape.getOrdre())));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(etape.getTempsEstime())));
        tableWidget->setItem(row, 5, new QTableWidgetItem(etape.getDateDebut().toString("dd/MM/yyyy")));
        tableWidget->setItem(row, 6, new QTableWidgetItem(etape.getDateFin().toString("dd/MM/yyyy")));
        tableWidget->setItem(row, 7, new QTableWidgetItem(etape.getEtat()));
        tableWidget->setItem(row, 8, new QTableWidgetItem(etape.getDescription()));
        
        tableWidget->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 3)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 4)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(row, 7)->setTextAlignment(Qt::AlignCenter);
    }
    
    statusLabel->setText(QString("%1: %2 étape(s)").arg(produit).arg(etapes.size()));
}

void MainWindow::onTableSelectionChanged() {
    QList<QTableWidgetItem*> selected = tableWidget->selectedItems();
    
    if (!selected.isEmpty()) {
        populateFormFromTable();
        updateButton->setEnabled(true);
        deleteButton->setEnabled(true);
    } else {
        updateButton->setEnabled(false);
        deleteButton->setEnabled(false);
    }
}

void MainWindow::onClearForm() {
    clearInputs();
}

void MainWindow::clearInputs() {
    idEdit->clear();
    nomEdit->clear();
    produitCombo->setCurrentIndex(0);
    ordreEdit->clear();
    tempsEdit->clear();
    descriptionEdit->clear();
    dateDebutEdit->setDate(QDate::currentDate());
    dateFinEdit->setDate(QDate::currentDate());
    etatCombo->setCurrentIndex(0);
    selectedEtapeId = -1;
    tableWidget->clearSelection();
    updateButton->setEnabled(false);
    deleteButton->setEnabled(false);
}

void MainWindow::populateFormFromTable() {
    int currentRow = tableWidget->currentRow();
    
    if (currentRow >= 0) {
        selectedEtapeId = tableWidget->item(currentRow, 0)->text().toInt();
        idEdit->setText(tableWidget->item(currentRow, 0)->text());
        nomEdit->setText(tableWidget->item(currentRow, 1)->text());
        
        QString produit = tableWidget->item(currentRow, 2)->text();
        int index = produitCombo->findText(produit);
        if (index >= 0) {
            produitCombo->setCurrentIndex(index);
        } else {
            produitCombo->setCurrentText(produit);
        }
        
        ordreEdit->setText(tableWidget->item(currentRow, 3)->text());
        tempsEdit->setText(tableWidget->item(currentRow, 4)->text());
        
        QDate dateDebut = QDate::fromString(tableWidget->item(currentRow, 5)->text(), "dd/MM/yyyy");
        QDate dateFin = QDate::fromString(tableWidget->item(currentRow, 6)->text(), "dd/MM/yyyy");
        dateDebutEdit->setDate(dateDebut);
        dateFinEdit->setDate(dateFin);
        
        QString etat = tableWidget->item(currentRow, 7)->text();
        int etatIndex = etatCombo->findText(etat);
        if (etatIndex >= 0) {
            etatCombo->setCurrentIndex(etatIndex);
        }
        
        descriptionEdit->setPlainText(tableWidget->item(currentRow, 8)->text());
    }
}
