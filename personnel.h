#ifndef PERSONNEL_H
#define PERSONNEL_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDate> // Pour QDate

class Personnel
{
public:
    Personnel();
    Personnel(int cin, QString nom, QString prenom, QDate date_n, QString poste, QDate date_e, int salaire, int tel, QString rfid);

    // Méthodes CRUD
    bool ajouter();
    QSqlQueryModel * afficher();
    bool supprimer(int cin_a_supprimer);
    bool modifier();

private:
    int cin, salaire, tel;
    QString nom, prenom, poste, rfid;
    QDate date_naissance, date_embauche;
};

#endif // PERSONNEL_H
