#ifndef CONNEXION_H
#define CONNEXION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class Connexion
{
public:
    // 1. Méthode statique pour récupérer l'instance unique
    static Connexion& createInstance();
    QSqlDatabase getDatabase() { return db; }  // ✅ AJOUT

    // 2. Méthode pour établir la connexion
    bool createconnect();

private:
    // Attribut persistant de la base de données
    QSqlDatabase db;

    // 3. Constructeur et destructeur en PRIVÉ (empêche la création de multiples objets)
    Connexion();
    ~Connexion();

    // 4. Interdire la copie et l'affectation (règle d'or du Singleton)
    Connexion(const Connexion&) = delete;
    Connexion& operator=(const Connexion&) = delete;
};

#endif // CONNEXION_H
