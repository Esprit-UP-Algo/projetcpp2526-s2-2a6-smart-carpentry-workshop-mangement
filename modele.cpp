#include "modele.h"
#include "connexion.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ═══════════════════════════════════════════════════════════════════════
//  Macro utilitaire : recupere la connexion via le Singleton
// ═══════════════════════════════════════════════════════════════════════
#define DB Connexion::createInstance().getDatabase()

// ═══════════════════════════════════════════════════════════════════════
//  CONSTRUCTEURS
// ═══════════════════════════════════════════════════════════════════════
Modele::Modele()
    : id(-1), nom(""), type(""), bois(""),
    longueur(""), largeur(""), hauteur(""),
    creePar(""), dateCreation(QDate::currentDate())
{}

Modele::Modele(int id, const QString &nom, const QString &type,
               const QString &bois, const QString &longueur,
               const QString &largeur, const QString &hauteur,
               const QString &creePar, const QDate &dateCreation)
    : id(id), nom(nom), type(type), bois(bois),
    longueur(longueur), largeur(largeur), hauteur(hauteur),
    creePar(creePar), dateCreation(dateCreation)
{}

// ═══════════════════════════════════════════════════════════════════════
//  ajouter()
// ═══════════════════════════════════════════════════════════════════════
bool Modele::ajouter()
{
    QSqlQuery q(DB);
    q.prepare(
        "INSERT INTO HEDI.MODELE_BOIS "
        "(ID_MODELE, NOM, TYPE, TYPE_BOIS, LONGUEUR, LARGEUR, HAUTEUR, CREEPAR, DATECREATION) "
        "VALUES (HEDI.MODELE_BOIS_SEQ.NEXTVAL, :nom, :type, :bois, "
        "        :longueur, :largeur, :hauteur, :creepar, TO_DATE(:date,'YYYY-MM-DD'))");

    q.bindValue(":nom",      nom);
    q.bindValue(":type",     type);
    q.bindValue(":bois",     bois);
    q.bindValue(":longueur", longueur.toDouble());
    q.bindValue(":largeur",  largeur.toDouble());
    q.bindValue(":hauteur",  hauteur.toDouble());
    q.bindValue(":creepar",  creePar);
    q.bindValue(":date",     dateCreation.toString("yyyy-MM-dd"));

    if (!q.exec()) {
        qDebug() << "Modele::ajouter() ERREUR :" << q.lastError().text();
        return false;
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
//  modifier()
// ═══════════════════════════════════════════════════════════════════════
bool Modele::modifier()
{
    QSqlQuery u(DB);
    u.prepare(
        "UPDATE HEDI.MODELE_BOIS SET "
        "NOM=:nom, TYPE=:type, TYPE_BOIS=:bois, "
        "LONGUEUR=:longueur, LARGEUR=:largeur, HAUTEUR=:hauteur, "
        "CREEPAR=:creepar, DATECREATION=TO_DATE(:date,'YYYY-MM-DD') "
        "WHERE ID_MODELE=:id");

    u.bindValue(":nom",      nom);
    u.bindValue(":type",     type);
    u.bindValue(":bois",     bois);
    u.bindValue(":longueur", longueur.toDouble());
    u.bindValue(":largeur",  largeur.toDouble());
    u.bindValue(":hauteur",  hauteur.toDouble());
    u.bindValue(":creepar",  creePar);
    u.bindValue(":date",     dateCreation.toString("yyyy-MM-dd"));
    u.bindValue(":id",       id);

    if (!u.exec()) {
        qDebug() << "Modele::modifier() ERREUR :" << u.lastError().text();
        return false;
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
//  supprimer()
// ═══════════════════════════════════════════════════════════════════════
bool Modele::supprimer(int idASupprimer)
{
    QSqlQuery q(DB);
    q.prepare("DELETE FROM HEDI.MODELE_BOIS WHERE ID_MODELE = :id");
    q.bindValue(":id", idASupprimer);

    if (!q.exec()) {
        qDebug() << "Modele::supprimer() ERREUR :" << q.lastError().text();
        return false;
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
//  afficher()
// ═══════════════════════════════════════════════════════════════════════
QSqlQueryModel* Modele::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(
        "SELECT ID_MODELE, NOM, TYPE, TYPE_BOIS, "
        "       LONGUEUR, LARGEUR, HAUTEUR, CREEPAR, "
        "       TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION "
        "FROM HEDI.MODELE_BOIS ORDER BY ID_MODELE",
        DB);

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Type");
    model->setHeaderData(3, Qt::Horizontal, "Type de Bois");
    model->setHeaderData(4, Qt::Horizontal, "Longueur (cm)");
    model->setHeaderData(5, Qt::Horizontal, "Largeur (cm)");
    model->setHeaderData(6, Qt::Horizontal, "Hauteur (cm)");
    model->setHeaderData(7, Qt::Horizontal, "Cree par");
    model->setHeaderData(8, Qt::Horizontal, "Date Creation");

    if (model->lastError().isValid())
        qDebug() << "Modele::afficher() ERREUR :" << model->lastError().text();

    return model;
}

// ═══════════════════════════════════════════════════════════════════════
//  rechercher()
// ═══════════════════════════════════════════════════════════════════════
QSqlQueryModel* Modele::rechercher(const QString &critere, const QString &valeur)
{
    QSqlQueryModel *model = new QSqlQueryModel();

    QMap<QString,QString> colonnes;
    colonnes["Nom"]  = "NOM";
    colonnes["ID"]   = "ID_MODELE";
    colonnes["Bois"] = "TYPE_BOIS";
    QString colSQL = colonnes.value(critere, "NOM");

    QSqlQuery q(DB);
    q.prepare(QString(
                  "SELECT ID_MODELE, NOM, TYPE, TYPE_BOIS, "
                  "       LONGUEUR, LARGEUR, HAUTEUR, CREEPAR, "
                  "       TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION "
                  "FROM HEDI.MODELE_BOIS "
                  "WHERE UPPER(%1) LIKE UPPER(:val) "
                  "ORDER BY ID_MODELE").arg(colSQL));
    q.bindValue(":val", "%" + valeur + "%");

    if (!q.exec()) {
        qDebug() << "Modele::rechercher() ERREUR :" << q.lastError().text();
        return model;
    }

    model->setQuery(std::move(q));
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Type");
    model->setHeaderData(3, Qt::Horizontal, "Type de Bois");
    model->setHeaderData(4, Qt::Horizontal, "Longueur (cm)");
    model->setHeaderData(5, Qt::Horizontal, "Largeur (cm)");
    model->setHeaderData(6, Qt::Horizontal, "Hauteur (cm)");
    model->setHeaderData(7, Qt::Horizontal, "Cree par");
    model->setHeaderData(8, Qt::Horizontal, "Date Creation");

    return model;
}

// ═══════════════════════════════════════════════════════════════════════
//  trier()
// ═══════════════════════════════════════════════════════════════════════
QSqlQueryModel* Modele::trier(const QString &colonne)
{
    QSqlQueryModel *model = new QSqlQueryModel();

    QMap<QString,QString> colonnes;
    colonnes["Nom"]  = "NOM";
    colonnes["ID"]   = "ID_MODELE";
    colonnes["Bois"] = "TYPE_BOIS";
    QString colSQL = colonnes.value(colonne, "NOM");

    model->setQuery(
        QString("SELECT ID_MODELE, NOM, TYPE, TYPE_BOIS, "
                "       LONGUEUR, LARGEUR, HAUTEUR, CREEPAR, "
                "       TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION "
                "FROM HEDI.MODELE_BOIS ORDER BY %1").arg(colSQL),
        DB);

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Type");
    model->setHeaderData(3, Qt::Horizontal, "Type de Bois");
    model->setHeaderData(4, Qt::Horizontal, "Longueur (cm)");
    model->setHeaderData(5, Qt::Horizontal, "Largeur (cm)");
    model->setHeaderData(6, Qt::Horizontal, "Hauteur (cm)");
    model->setHeaderData(7, Qt::Horizontal, "Cree par");
    model->setHeaderData(8, Qt::Horizontal, "Date Creation");

    return model;
}
