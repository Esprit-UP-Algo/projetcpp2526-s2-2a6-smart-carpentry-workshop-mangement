#include "Etape.h"

Etape::Etape() 
    : id(0), nom(""), produit(""), ordre(0), tempsEstime(0), 
      description(""), dateDebut(QDate()), dateFin(QDate()), etat("Non commencé") {
}

Etape::Etape(int id, const QString& nom, const QString& produit, int ordre,
             int tempsEstime, const QString& description, const QDate& dateDebut,
             const QDate& dateFin, const QString& etat)
    : id(id), nom(nom), produit(produit), ordre(ordre), tempsEstime(tempsEstime),
      description(description), dateDebut(dateDebut), dateFin(dateFin), etat(etat) {
}
