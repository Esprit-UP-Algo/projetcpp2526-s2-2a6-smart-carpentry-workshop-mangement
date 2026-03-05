#include "connexion.h"
#include <QSqlDatabase>
#include <QDebug>

connexion::connexion() {
    // constructeur vide
}

bool connexion::createconnect() {
    bool test = false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("woodpilot");
    db.setUserName("youssef");
    db.setPassword("1234");

    if (db.open()) {
        test = true;
        qDebug() << "Connexion réussie.";
    } else {
        qDebug() << "Erreur de connexion:" << db.lastError().text();
    }

    return test;
}
