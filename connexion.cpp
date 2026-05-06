#include "connexion.h"

// 1. Constructeur (Privé)
Connexion::Connexion()
{
    // On initialise le driver ODBC une seule fois ici
    db = QSqlDatabase::addDatabase("QODBC");
}

// 2. Destructeur (Privé)
Connexion::~Connexion()
{
    if (db.isOpen()) {
        db.close(); // Libère proprement les ressources à la fermeture
    }
}

// 3. Méthode statique pour récupérer l'instance unique (Le cœur du Singleton)
Connexion& Connexion::createInstance()
{
    static Connexion instance; // L'instance est créée la première fois qu'on appelle cette fonction
    return instance;           // Et elle reste vivante pendant tout le programme
}

// 4. Établissement de la connexion
bool Connexion::createconnect()
{
    // Utilisez les identifiants que nous avons configurés précédemment
    db.setDatabaseName("WoodPilot");
    db.setUserName("hedi");
    db.setPassword("123");

    if (db.open()) {
        qDebug() << "Connexion établie avec succès !";
        return true;
    } else {
        qDebug() << "Échec de la connexion :" << db.lastError().text();
        return false;
    }
}
