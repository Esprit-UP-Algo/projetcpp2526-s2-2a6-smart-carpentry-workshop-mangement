#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QInputDialog>
#include "SmtpClient-for-Qt/src/smtpclient.h"
#include "SmtpClient-for-Qt/src/mimemessage.h"
#include "SmtpClient-for-Qt/src/mimetext.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
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

    if(email.isEmpty() || mdp.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    QRegularExpression mailRex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if(!mailRex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Format Incorrect", "Veuillez saisir une adresse mail valide.");
        return;
    }

    if(email == "WoodPilot@esprit.tn" && mdp == "woodpilot") {
        QMessageBox::information(this, "Succès", "Bienvenue sur WoodPilot !");
        this->hide();
        w = new MainWindow();
        w->show();
    } else {
        QMessageBox::critical(this, "Échec", "Email ou mot de passe incorrect.");
        ui->le_password->clear();
    }
}

void Login::on_btn_reset_clicked()
{
    bool ok;
    QString emailDestinataire = QInputDialog::getText(
        this, "Réinitialisation du mot de passe",
        "Entrez votre adresse email :",
        QLineEdit::Normal, "", &ok);

    if (!ok || emailDestinataire.isEmpty()) return;

    QRegularExpression mailRex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!mailRex.match(emailDestinataire).hasMatch()) {
        QMessageBox::warning(this, "Format Incorrect", "Adresse email invalide.");
        return;
    }

    if (emailDestinataire != "theinvincus@gmail.com") {
        QMessageBox::critical(this, "Échec", "Cette adresse email n'est pas enregistrée.");
        return;
    }

    QString emailExpediteur = "theinvincus@gmail.com";
    QString motDePasseGmail = "mmjc orrz juyy rczj"; // mot de passe d'application Gmail

    // Créer le client SMTP
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    // Créer le message
    MimeMessage message;
    message.setSender(EmailAddress(emailExpediteur, "WoodPilot"));
    message.addTo(EmailAddress(emailDestinataire, "Utilisateur WoodPilot"));
    message.setSubject("WoodPilot - Récupération de votre mot de passe");

    MimeText texte;
    texte.setText(
        "Bonjour,\n\n"
        "Vous avez demandé la récupération de votre mot de passe WoodPilot.\n\n"
        "Votre mot de passe est : woodpilot\n\n"
        "Pour des raisons de sécurité, ne partagez pas ce mot de passe.\n\n"
        "Cordialement,\n"
        "L'équipe WoodPilot"
        );
    message.addPart(&texte);

    // Connexion au serveur
    smtp.connectToHost();
    if (!smtp.waitForReadyConnected(10000)) {
        QMessageBox::critical(this, "Erreur SMTP", "Impossible de se connecter au serveur email.");
        return;
    }

    // Authentification
    smtp.login(emailExpediteur, motDePasseGmail, SmtpClient::AuthLogin);
    if (!smtp.waitForAuthenticated(10000)) {
        QMessageBox::critical(this, "Erreur SMTP", "Authentification échouée. Vérifiez vos identifiants.");
        smtp.quit();
        return;
    }

    // Envoi
    smtp.sendMail(message);
    if (!smtp.waitForMailSent(10000)) {
        QMessageBox::critical(this, "Erreur", "Échec de l'envoi de l'email.");
        smtp.quit();
        return;
    }

    smtp.quit();
    QMessageBox::information(this, "Email envoyé",
                             "Votre mot de passe a été envoyé à : " + emailDestinataire);
}
