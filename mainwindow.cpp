#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "connexion.h"
#include <QMessageBox>
#include <QDebug>
#include <QHeaderView>
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

