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
              "FROM TypeBois WHERE nomBois LIKE :mot OR fournisseur LIKE :mot2");
    q.bindValue(":mot",  "%" + mot + "%");
    q.bindValue(":mot2", "%" + mot + "%");
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
