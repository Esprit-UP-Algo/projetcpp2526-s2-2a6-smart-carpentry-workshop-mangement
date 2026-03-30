#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "connexion.h"
#include <QMessageBox>
#include <QDebug>
#include <QHeaderView>
#include <QTextDocument>
#include <QPrinter>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- FORCE LA COULEUR DU TEXTE EN NOIR POUR LE TABLEAU ---
    ui->tab_employes->setStyleSheet("QTableView { color: black; background-color: white; }");

    // (Optionnel) Pour que les colonnes prennent toute la largeur :
    ui->tab_employes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    refreshTable();
    // 1. Bois (Page 0)
    connect(ui->btn_bois, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page);
    });

    // 2. Modèle (Page 1)
    connect(ui->btn_modele, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_6);
    });

    // 3. Personnel (Page 2)
    connect(ui->btn_personnel, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_5);
    });

    // 4. Étape (Page 3)
    connect(ui->btn_etape, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_7);
    });

    // 5. Fabrication (Page 4)
    connect(ui->btn_fabrication, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_4);

    });
}
/* connect(ui->btn_personnel, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_5);
    }); */
MainWindow::~MainWindow()
{
    delete ui;
    refreshBoisTable();

}
// Fonction pour rafraîchir le tableau
void MainWindow::refreshTable()
{
    Personnel P;
    ui->tab_employes->setModel(P.afficher());
}

// --- LES SLOTS DES BOUTONS ---

void MainWindow::on_btn_ajouter_3_clicked()
{
    if (!controlDeSaisie()) return; // Arrête la fonction si la saisie est fausse
    // 1. Récupérer les données de l'interface
    int cin = ui->le_cin->text().toInt();
    QString nom = ui->le_nom_2->text();
    QString prenom = ui->le_prenom->text();
    QDate date_n = ui->de_naissance->date();
    QString poste = ui->cb_poste->currentText();
    QDate date_e = ui->de_embauche->date();
    int salaire = ui->le_salaire->text().toInt();
    int tel = ui->le_tel->text().toInt();
    QString rfid = ui->le_rfid->text();

    // 2. Créer un objet Personnel avec ces données
    Personnel P(cin, nom, prenom, date_n, poste, date_e, salaire, tel, rfid);

    // 3. Appeler la méthode ajouter() et vérifier le résultat
    if (P.ajouter()) {
        QMessageBox::information(this, "Succès", "Employé ajouté avec succès !");
        refreshTable(); // Mettre à jour l'affichage
    } else {
        QMessageBox::critical(this, "Erreur", "L'ajout a échoué. Vérifiez si le CIN existe déjà.");
    }
}

void MainWindow::on_btn_modifier_2_clicked()
{
    if (!controlDeSaisie()) return; // Arrête la fonction si la saisie est fausse

    // Même logique que l'ajout
    int cin = ui->le_cin->text().toInt();
    // ... récupérer les autres champs ...
    QString nom = ui->le_nom_2->text();
    QString prenom = ui->le_prenom->text();
    QDate date_n = ui->de_naissance->date();
    QString poste = ui->cb_poste->currentText();
    QDate date_e = ui->de_embauche->date();
    int salaire = ui->le_salaire->text().toInt();
    int tel = ui->le_tel->text().toInt();
    QString rfid = ui->le_rfid->text();


    Personnel P(cin, nom, prenom, date_n, poste, date_e, salaire, tel, rfid);

    if (P.modifier()) {
        QMessageBox::information(this, "Succès", "Données de l'employé mises à jour.");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Erreur", "La modification a échoué.");
    }
}

void MainWindow::on_btn_supprimer_2_clicked()
{
    int cin = ui->le_cin->text().toInt();
    if (cin == 0) {
        QMessageBox::warning(this, "Avertissement", "Veuillez saisir un CIN à supprimer.");
        return;
    }

    Personnel P;
    if (P.supprimer(cin)) {
        QMessageBox::information(this, "Succès", "Employé supprimé.");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Erreur", "La suppression a échoué.");
    }
}

void MainWindow::on_tab_employes_clicked(const QModelIndex &index)
{
        int row = index.row();

        // 2. On extrait les données du modèle colonne par colonne
        // L'ordre (0, 1, 2...) correspond à l'ordre de votre SELECT dans personnel.cpp
        QString cin = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 0)).toString();
        QString nom = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 1)).toString();
        QString prenom = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 2)).toString();
        QDate date_n = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 3)).toDate();
        QString poste = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 4)).toString();
        QDate date_e = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 5)).toDate();
        QString salaire = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 6)).toString();
        QString tel = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 7)).toString();
        QString rfid = ui->tab_employes->model()->data(ui->tab_employes->model()->index(row, 8)).toString();

        // 3. On injecte ces données dans les champs (Widgets) de l'interface
        ui->le_cin->setText(cin);
        ui->le_nom_2->setText(nom);
        ui->le_prenom->setText(prenom);
        ui->de_naissance->setDate(date_n);
        ui->cb_poste->setCurrentText(poste); // Sélectionne automatiquement le bon métier dans la liste
        ui->de_embauche->setDate(date_e);
        ui->le_salaire->setText(salaire);
        ui->le_tel->setText(tel);
        ui->le_rfid->setText(rfid);

}
bool MainWindow::controlDeSaisie()
{
    // 1. Récupération des valeurs
    QString cin = ui->le_cin->text();
    QString nom = ui->le_nom_2->text();
    QString prenom = ui->le_prenom->text();
    QString tel = ui->le_tel->text();
    QString salaire = ui->le_salaire->text();
    QDate dn = ui->de_naissance->date();
    QDate de = ui->de_embauche->date();
    QString poste = ui->cb_poste->currentText();

    // 2. Expressions régulières (Regex)
    QRegularExpression regexCin("^[0-9]{8}$");           // 8 chiffres exacts
    QRegularExpression regexNom("^[A-Za-z ]{2,20}$");    // Lettres, 2 à 20 caractères
    QRegularExpression regexTel("^[0-9]{8}$");           // 8 chiffres (Tunisie)

    // 3. Tests de validation
    if (cin.isEmpty() || nom.isEmpty() || prenom.isEmpty() || tel.isEmpty() || salaire.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs obligatoires doivent être remplis.");
        return false;
    }

    if (!regexCin.match(cin).hasMatch()) {
        QMessageBox::warning(this, "Erreur CIN", "Le CIN doit contenir exactement 8 chiffres.");
        return false;
    }

    if (!regexNom.match(nom).hasMatch() || !regexNom.match(prenom).hasMatch()) {
        QMessageBox::warning(this, "Erreur Nom/Prénom", "Le nom et le prénom ne doivent contenir que des lettres (2-20).");
        return false;
    }

    if (!regexTel.match(tel).hasMatch()) {
        QMessageBox::warning(this, "Erreur Téléphone", "Le numéro de téléphone doit contenir 8 chiffres.");
        return false;
    }

    if (salaire.toInt() <= 0) {
        QMessageBox::warning(this, "Erreur Salaire", "Le salaire doit être un nombre positif.");
        return false;
    }

    // Vérification de l'âge (min 18 ans)
    if (dn.daysTo(QDate::currentDate()) < 18 * 365) {
        QMessageBox::warning(this, "Erreur Date", "L'employé doit être majeur (18 ans minimum).");
        return false;
    }

    // Cohérence des dates (Embauche après Naissance)
    if (de <= dn) {
        QMessageBox::warning(this, "Erreur Date", "La date d'embauche doit être après la date de naissance.");
        return false;
    }

    if (poste == "Poste..." || poste.isEmpty()) {
        QMessageBox::warning(this, "Erreur Poste", "Veuillez sélectionner un poste dans la liste.");
        return false;
    }

    return true; // Si on arrive ici, tout est OK
}
// Rafraîchit le tableau Bois
void MainWindow::refreshBoisTable()
{
    Bois b;
    ui->tab_bois_7->setModel(b.afficher());
    ui->tab_bois_7->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tab_bois_7->horizontalHeader()->setStretchLastSection(true);

    ui->tab_bois_7->setStyleSheet("QTableView { color: black; background-color: white; }");
}

// BOUTON AJOUTER
void MainWindow::on_la_ajouter_7_clicked()
{
    QString nom       = ui->la_nom_7->currentText();
    QString type      = ui->la_type_7->currentText();
    QString etat      = ui->la_etat_7->currentText();
    QString fournisseur = ui->la_fournisseur_7->text();
    QString emplacement = ui->la_emplacement_7->text();
    double prix       = ui->la_prix_7->value();
    double l          = ui->la_l_7->value();
    double w          = ui->la_w_7->value();
    double h          = ui->la_h_7->value();
    QDate date        = ui->la_date_7->date();

    if (nom.isEmpty() || fournisseur.isEmpty() || emplacement.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    Bois b(nom, type, l, w, h, etat, prix, date, fournisseur, emplacement);
    if (b.ajouter()) {
        QMessageBox::information(this, "Succès", "Bois ajouté avec succès !");
        refreshBoisTable();
    } else {
        QMessageBox::critical(this, "Erreur", "L'ajout a échoué.");
    }
}

// BOUTON MODIFIER
void MainWindow::on_la_modifier_7_clicked()
{
    int id = ui->la_id_13->text().toInt();
    if (id == 0) {
        QMessageBox::warning(this, "Erreur", "Sélectionnez un bois dans le tableau d'abord.");
        return;
    }

    QString nom         = ui->la_nom_7->currentText();
    QString type        = ui->la_type_7->currentText();
    QString etat        = ui->la_etat_7->currentText();
    QString fournisseur = ui->la_fournisseur_7->text();
    QString emplacement = ui->la_emplacement_7->text();
    double prix         = ui->la_prix_7->value();
    double l            = ui->la_l_7->value();
    double w            = ui->la_w_7->value();
    double h            = ui->la_h_7->value();
    QDate date          = ui->la_date_7->date();

    Bois b(nom, type, l, w, h, etat, prix, date, fournisseur, emplacement);
    if (b.modifier(id)) {
        QMessageBox::information(this, "Succès", "Bois modifié avec succès !");
        refreshBoisTable();
    } else {
        QMessageBox::critical(this, "Erreur", "La modification a échoué.");
    }
}

// BOUTON SUPPRIMER
void MainWindow::on_la_supprimer_7_clicked()
{
    int id = ui->la_id_13->text().toInt();
    if (id == 0) {
        QMessageBox::warning(this, "Erreur", "Sélectionnez un bois dans le tableau d'abord.");
        return;
    }

    Bois b;
    if (b.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Bois supprimé.");
        refreshBoisTable();
    } else {
        QMessageBox::critical(this, "Erreur", "La suppression a échoué.");
    }
}

// BOUTON CHERCHER
void MainWindow::on_chercher_7_clicked()
{
    QString mot = ui->la_recherche_7->text();
    Bois b;
    ui->tab_bois_7->setModel(b.rechercher(mot));
}


void MainWindow::on_tab_bois_7_clicked(const QModelIndex &index)
{
    int row = index.row();

    QString id          = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 0)).toString();
    QString nom         = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 1)).toString();
    QString type        = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 2)).toString();
    double longueur     = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 3)).toDouble();
    double largeur      = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 4)).toDouble();
    double epaisseur    = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 5)).toDouble();
    QString etat        = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 6)).toString();
    double prix         = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 7)).toDouble();
    QDate date          = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 8)).toDate();
    QString fournisseur = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 9)).toString();
    QString emplacement = ui->tab_bois_7->model()->data(ui->tab_bois_7->model()->index(row, 10)).toString();

    // Remplir les champs
    ui->la_id_13->setText(id);
    ui->la_nom_7->setCurrentText(nom);
    ui->la_type_7->setCurrentText(type);
    ui->la_etat_7->setCurrentText(etat);
    ui->la_fournisseur_7->setText(fournisseur);
    ui->la_emplacement_7->setText(emplacement);
    ui->la_prix_7->setValue(prix);
    ui->la_l_7->setValue(longueur);
    ui->la_w_7->setValue(largeur);
    ui->la_h_7->setValue(epaisseur);
    ui->la_date_7->setDate(date);
}

void MainWindow::on_la_trier_7_clicked()
{
    QString choix = ui->la_tri_7->currentText();
    QString colonne;

    if (choix == "Nom")        colonne = "nomBois";
    else if (choix == "Date")  colonne = "dateEntree";
    else if (choix == "Type")  colonne = "typeBois";
    else if (choix == "Etat")  colonne = "etatBois";
    else if (choix == "Prix")  colonne = "prixUnitaire";

    Bois b;
    ui->tab_bois_7->setModel(b.trier(colonne));
}
void MainWindow::on_la_pdf_7_clicked()
{
    QAbstractItemModel* model = ui->tab_bois_7->model();
    if (!model) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée à exporter.");
        return;
    }

    // Choisir où sauvegarder
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer PDF",
                                                    "TypeBois_Export.pdf",
                                                    "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    // Construire le tableau HTML
    QString html = "<h2 style='text-align:center;'>Liste des Bois - WoodPilot</h2>";
    html += "<table border='1' cellspacing='0' cellpadding='4' width='100%'>";

    // En-têtes
    html += "<tr style='background-color:#5c3317; color:white;'>";
    for (int c = 0; c < model->columnCount(); c++) {
        html += "<th>" + model->headerData(c, Qt::Horizontal).toString() + "</th>";
    }
    html += "</tr>";

    // Données
    for (int r = 0; r < model->rowCount(); r++) {
        html += (r % 2 == 0) ? "<tr>" : "<tr style='background-color:#f5f0eb;'>";
        for (int c = 0; c < model->columnCount(); c++) {
            html += "<td>" + model->data(model->index(r, c)).toString() + "</td>";
        }
        html += "</tr>";
    }
    html += "</table>";

    // Générer le PDF
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageOrientation(QPageLayout::Landscape);

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, "Succès", "PDF exporté avec succès !");
}
