#ifndef BOIS_H
#define BOIS_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Bois
{
public:
    Bois();
    Bois(QString nomBois, QString typeBois, double longueur, double largeur,
         double epaisseur, QString etatBois, double prixUnitaire,
         QDate dateEntree, QString fournisseur, QString emplacementStock);

    bool ajouter();
    bool modifier(int id);
    bool supprimer(int id);
    QSqlQueryModel* afficher();
    QSqlQueryModel* rechercher(QString mot);
    QSqlQueryModel* trier(QString colonne);

private:
    QString nomBois, typeBois, etatBois, fournisseur, emplacementStock;
    double longueur, largeur, epaisseur, prixUnitaire;
    QDate dateEntree;
};

#endif // BOIS_H
