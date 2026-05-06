#include "bois.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QVariant>

Bois::Bois() {}

Bois::Bois(QString nomBois, QString typeBois, double longueur, double largeur,
           double epaisseur, QString etatBois, double prixUnitaire,
           QDate dateEntree, QString fournisseur, QString emplacementStock)
{
    this->nomBois = nomBois;
    this->typeBois = typeBois;
    this->longueur = longueur;
    this->largeur = largeur;
    this->epaisseur = epaisseur;
    this->etatBois = etatBois;
    this->prixUnitaire = prixUnitaire;
    this->dateEntree = dateEntree;
    this->fournisseur = fournisseur;
    this->emplacementStock = emplacementStock;
}

bool Bois::ajouter()
{
    QSqlQuery q;
    q.prepare("INSERT INTO TypeBois (nomBois, typeBois, longueur, largeur, epaisseur, "
              "etatBois, prixUnitaire, dateEntree, fournisseur, emplacementStock) "
              "VALUES (:nom, :type, :lon, :lar, :ep, :etat, :prix, :date, :fourn, :empl)");
    q.bindValue(":nom",  nomBois);
    q.bindValue(":type", typeBois);
    q.bindValue(":lon",  longueur);
    q.bindValue(":lar",  largeur);
    q.bindValue(":ep",   epaisseur);
    q.bindValue(":etat", etatBois);
    q.bindValue(":prix", prixUnitaire);
    q.bindValue(":date", dateEntree);
    q.bindValue(":fourn",fournisseur);
    q.bindValue(":empl", emplacementStock);
    return q.exec();
}

bool Bois::modifier(int id)
{
    QSqlQuery q;
    q.prepare("UPDATE TypeBois SET nomBois=:nom, typeBois=:type, longueur=:lon, "
              "largeur=:lar, epaisseur=:ep, etatBois=:etat, prixUnitaire=:prix, "
              "dateEntree=:date, fournisseur=:fourn, emplacementStock=:empl "
              "WHERE idbois=:id");
    q.bindValue(":nom",  nomBois);
    q.bindValue(":type", typeBois);
    q.bindValue(":lon",  longueur);
    q.bindValue(":lar",  largeur);
    q.bindValue(":ep",   epaisseur);
    q.bindValue(":etat", etatBois);
    q.bindValue(":prix", prixUnitaire);
    q.bindValue(":date", dateEntree);
    q.bindValue(":fourn",fournisseur);
    q.bindValue(":empl", emplacementStock);
    q.bindValue(":id",   id);
    return q.exec();
}

bool Bois::supprimer(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM TypeBois WHERE idbois=:id");
    q.bindValue(":id", id);
    return q.exec();
}

QSqlQueryModel* Bois::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT idbois, nomBois, typeBois, longueur, largeur, "
                    "epaisseur, etatBois, prixUnitaire, dateEntree, "
                    "fournisseur, emplacementStock FROM TypeBois");
    return model;
}

QSqlQueryModel* Bois::rechercher(QString mot)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery q;
    q.prepare("SELECT idbois, nomBois, typeBois, longueur, largeur, epaisseur, "
              "etatBois, prixUnitaire, dateEntree, fournisseur, emplacementStock "
              "FROM TypeBois WHERE "
              "UPPER(nomBois) LIKE UPPER(:mot) OR "
              "UPPER(typeBois) LIKE UPPER(:mot2) OR "
              "UPPER(etatBois) LIKE UPPER(:mot3) OR "
              "UPPER(fournisseur) LIKE UPPER(:mot4) OR "
              "UPPER(emplacementStock) LIKE UPPER(:mot5) OR "
              "TO_CHAR(prixUnitaire) LIKE :mot6 OR "
              "TO_CHAR(longueur) LIKE :mot7 OR "
              "TO_CHAR(largeur) LIKE :mot8 OR "
              "TO_CHAR(epaisseur) LIKE :mot9 OR "
              "TO_CHAR(idbois) LIKE :mot10 OR "
              "TO_CHAR(dateEntree, 'DD/MM/YYYY') LIKE :mot11");
    QString pattern = "%" + mot + "%";
    for (int i = 1; i <= 11; i++)
        q.bindValue(":mot" + (i > 1 ? QString::number(i) : ""), pattern);
    q.exec();
    model->setQuery(std::move(q));
    return model;
}

QSqlQueryModel* Bois::trier(QString colonne)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT idbois, nomBois, typeBois, longueur, largeur, "
                    "epaisseur, etatBois, prixUnitaire, dateEntree, "
                    "fournisseur, emplacementStock FROM TypeBois ORDER BY " + colonne);
    return model;
}
