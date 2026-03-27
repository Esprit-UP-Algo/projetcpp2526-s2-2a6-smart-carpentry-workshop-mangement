#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QRegularExpression> // Pour vérifier le format de l'email

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // Pour que le mot de passe soit caché par des points ●
    ui->le_password->setEchoMode(QLineEdit::Password);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btn_login_clicked()
{
    QString email = ui->le_email->text();
    QString mdp = ui->le_password->text();

    // --- 1. CONTROLE DE SAISIE (Champs vides) ---
    if(email.isEmpty() || mdp.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    // --- 2. CONTROLE DE SAISIE (Format Email via Regex) ---
    QRegularExpression mailRex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if(!mailRex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Format Incorrect", "Veuillez saisir une adresse mail valide.");
        return;
    }

    // --- 3. VÉRIFICATION DES IDENTIFIANTS (En dur) ---
    if(email == "WoodPilot@esprit.tn" && mdp == "woodpilot") {

        QMessageBox::information(this, "Succès", "Bienvenue sur WoodPilot !");

        // Fermer la fenêtre de login et ouvrir MainWindow
        this->hide();
        w = new MainWindow();
        w->show();

    } else {
        // Message d'erreur si identifiants faux
        QMessageBox::critical(this, "Échec", "Email ou mot de passe incorrect.");
        ui->le_password->clear(); // On vide le mot de passe pour réessayer
    }
}
