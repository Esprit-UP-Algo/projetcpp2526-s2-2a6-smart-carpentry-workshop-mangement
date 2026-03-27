#include "mainwindow.h"
#include <QApplication>
#include "login.h"
#include "connexion.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. On récupère l'instance unique du Singleton
    // Le constructeur étant privé, c'est la seule façon d'accéder à la connexion
    Connexion& c = Connexion::createInstance();

    // 2. On tente d'établir la connexion à la base de données
    bool test = c.createconnect();

    Login w; // On prépare la fenêtre de démarrage

    if(test)
    {
        // Si la connexion réussit, on affiche l'interface
        w.show();
        QMessageBox::information(nullptr, QObject::tr("Base de données"),
                                 QObject::tr("Connexion réussie.\n"
                                             "Cliquez sur OK pour continuer."), QMessageBox::Ok);
    }
    else
    {
        // Si la connexion échoue, on affiche un message d'erreur critique
        QMessageBox::critical(nullptr, QObject::tr("Erreur de connexion"),
                              QObject::tr("Impossible de se connecter à la base de données.\n"
                                          "Vérifiez votre source ODBC 'WoodPilot' et vos identifiants."),
                              QMessageBox::Cancel);

        return -1; // On arrête le programme car la base est indispensable pour le CRUD
    }

    return a.exec();
}
