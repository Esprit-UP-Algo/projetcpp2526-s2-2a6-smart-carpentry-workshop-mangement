#include "personnel.h"
#include <QDebug>
#include <QSqlError> // Pour QSqlError

Personnel::Personnel()
{
    cin = 0; salaire = 0; tel = 0;
    nom = ""; prenom = ""; poste = ""; rfid = "";
    date_naissance = QDate(); // Initialisation par défaut
    date_embauche = QDate();  // Initialisation par défaut
}

Personnel::Personnel(int cin, QString nom, QString prenom, QDate date_n, QString poste, QDate date_e, int salaire, int tel, QString rfid)
{
    this->cin = cin;
    this->nom = nom;
    this->prenom = prenom;
    this->date_naissance = date_n;
    this->poste = poste;
    this->date_embauche = date_e;
    this->salaire = salaire;
    this->tel = tel;
    this->rfid = rfid;
}

// AJOUTER
bool Personnel::ajouter()
{
    QSqlQuery query;

    query.prepare("INSERT INTO PERSONNEL (CIN, NOM, PRENOM, DATENAISSANCE, POSTE, "
                  "DATEEMBAUCHE, SALAIRE, NUMTEL, RFID) "
                  "VALUES (:cin, :nom, :prenom, :date_n, :poste, :date_e, :salaire, :tel, :rfid)");

    // Binding des valeurs
    query.bindValue(":cin", cin);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":date_n", date_naissance);   // Attention : utilise date_naissance du membre
    query.bindValue(":poste", poste);
    query.bindValue(":date_e", date_embauche);
    query.bindValue(":salaire", salaire);
    query.bindValue(":tel", tel);
    query.bindValue(":rfid", rfid);

    if (!query.exec()) {
        qDebug() << "ERREUR SQL lors de l'ajout :" << query.lastError().text();
        qDebug() << "Code erreur Oracle :" << query.lastError().nativeErrorCode();
        qDebug() << "CIN tenté :" << cin;
        return false;
    }

    qDebug() << "Ajout réussi pour CIN :" << cin;
    return true;
}
// AFFICHER
QSqlQueryModel * Personnel::afficher()
{
    QSqlQueryModel * model = new QSqlQueryModel();

    // 1. On sélectionne TOUTES les colonnes dans l'ordre souhaité
    model->setQuery("SELECT TO_CHAR(CIN) AS CIN, NOM, PRENOM, DATENAISSANCE, POSTE, "
                    "DATEEMBAUCHE, SALAIRE, TO_CHAR(NUMTEL) AS NUMTEL, RFID FROM PERSONNEL");

    if (model->lastError().isValid()) {
        qDebug() << "ERREUR SQL AFFICHAGE :" << model->lastError().text();
    }

    // 2. On nomme les en-têtes (l'index doit correspondre à l'ordre du SELECT ci-dessus)
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("CIN"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date Naissance"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Poste"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Date Embauche"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Salaire"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Téléphone"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("RFID"));

    return model;
}

// SUPPRIMER
bool Personnel::supprimer(int cin_a_supprimer)
{
    QSqlQuery query;
    query.prepare("DELETE FROM PERSONNEL WHERE CIN = :cin");
    query.bindValue(":cin", cin_a_supprimer);

    if (!query.exec()) {
        qDebug() << "ERREUR SQL SUPPRESSION :" << query.lastError().text();
        return false;
    }
    return true;
}

// MODIFIER
bool Personnel::modifier()
{
    QSqlQuery query;
    QString cin_str = QString::number(cin);

    // AJOUT DE 'RFID=:rfid' dans le UPDATE
    query.prepare("UPDATE PERSONNEL SET NOM=:nom, PRENOM=:prenom, DATENAISSANCE=:date_n, POSTE=:poste, "
                  "DATEEMBAUCHE=:date_e, SALAIRE=:salaire, NUMTEL=:tel, RFID=:rfid WHERE CIN=:cin");

    query.bindValue(":cin", cin_str);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":date_n", date_naissance);
    query.bindValue(":poste", poste);
    query.bindValue(":date_e", date_embauche);
    query.bindValue(":salaire", salaire);
    query.bindValue(":tel", tel);

    // NOUVELLE LIGNE : On relie la valeur rfid
    query.bindValue(":rfid", rfid);

    if (!query.exec()) {
        qDebug() << "ERREUR SQL MODIFICATION :" << query.lastError().text();
        return false;
    }
    return true;
}
