#ifndef CONNEXION_H
#define CONNEXION_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class Connexion
{
public:
    static Connexion& createInstance();
    bool createconnect();
    QSqlDatabase getDatabase() { return db; }  // ✅ AJOUT

private:
    QSqlDatabase db;
    Connexion();
    ~Connexion();
    Connexion(const Connexion&) = delete;
    Connexion& operator=(const Connexion&) = delete;
};
#endif // CONNEXION_H
