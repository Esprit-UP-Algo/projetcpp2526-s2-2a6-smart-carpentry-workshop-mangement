#include "fab_controller.h"
#include <QSqlQuery>
#include <QDebug>

fab_controller::fab_controller() {
    IDFABRICATION = 0;
    IDMODELE = 0;
    DATEDEBUT = QDate();
    DATEFINPREVU = QDate();
    DATEFINREEL = QDate();
    ETATFABRICATION = "";
    TYPE_BOIS = "";

}

fab_controller::fab_controller(int IDFABRICATION, int IDMODELE, QDate DATEDEBUT,
                               QString TYPE_BOIS, QDate DATEFINPREVU, QDate DATEFINREEL,
                               QString ETATFABRICATION)
{
    this->IDFABRICATION = IDFABRICATION;
    this->IDMODELE = IDMODELE;
    this->DATEDEBUT = DATEDEBUT;
    this->TYPE_BOIS = TYPE_BOIS;
    this->DATEFINPREVU = DATEFINPREVU;
    this->DATEFINREEL = DATEFINREEL;
    this->ETATFABRICATION = ETATFABRICATION;
}

// --- Getters ---
int fab_controller::getIDFABRICATION() { return IDFABRICATION; }
int fab_controller::getIDMODELE() { return IDMODELE; }
QDate fab_controller::getDATEDEBUT() { return DATEDEBUT; }
QString fab_controller::getTYPE_BOIS(){return TYPE_BOIS;}
QDate fab_controller::getDATEFINPREVU() { return DATEFINPREVU; }
QDate fab_controller::getDATEFINREEL() { return DATEFINREEL; }
QString fab_controller::getETATFABRICATION() { return ETATFABRICATION; }

// --- Setters ---
void fab_controller::setIDFABRICATION(int id){ IDFABRICATION = id; }
void fab_controller::setIDMODELE(int id){ IDMODELE = id; }
void fab_controller::setDATEDEBUT(QDate d){ DATEDEBUT = d; }
void fab_controller::setTYPE_BOIS(QString t){TYPE_BOIS = t;}
void fab_controller::setDATEFINPREVU(QDate d){ DATEFINPREVU = d; }
void fab_controller::setDATEFINREEL(QDate d){ DATEFINREEL = d; }
void fab_controller::setETATFABRICATION(QString s){ ETATFABRICATION = s; }

// --- CRUD ---
bool fab_controller::add()
{
    QSqlQuery query;
    query.prepare("INSERT INTO FABRICATION(IDFABRICATION, IDMODELE, DATEDEBUT, TYPE_BOIS, DATEFIN, DATEFINREEL, ETATFABRICATION)"
                  " VALUES(:id, :idmodele, :datedebut, :typebois, :datefin, :datefinreel, :etat)");

    query.bindValue(":id", IDFABRICATION);
    query.bindValue(":idmodele", IDMODELE);
    query.bindValue(":datedebut", DATEDEBUT);
    query.bindValue(":typebois", TYPE_BOIS);
    query.bindValue(":datefin", DATEFINPREVU);
    query.bindValue(":datefinreel", DATEFINREEL);
    query.bindValue(":etat", ETATFABRICATION);

    return query.exec();
}

bool fab_controller::modify(int id)
{
    QSqlQuery query;
    query.prepare("UPDATE FABRICATION SET IDMODELE=:idmodele, DATEDEBUT=:datedebut, TYPE_BOIS=:typebois, DATEFIN=:datefin, DATEFINREEL=:datefinreel, ETATFABRICATION=:etat "
                  "WHERE IDFABRICATION=:id");

    query.bindValue(":id", id);
    query.bindValue(":idmodele", IDMODELE);
    query.bindValue(":datedebut", DATEDEBUT);
    query.bindValue(":typebois", TYPE_BOIS);
    query.bindValue(":datefin", DATEFINPREVU);
    query.bindValue(":datefinreel", DATEFINREEL);
    query.bindValue(":etat", ETATFABRICATION);

    return query.exec();
}

bool fab_controller::delete_2(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM FABRICATION WHERE IDFABRICATION=:id");
    query.bindValue(":id", id);
    return query.exec();
}
