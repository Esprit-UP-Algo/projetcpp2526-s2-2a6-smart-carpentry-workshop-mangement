// fab_controller.h
#ifndef FAB_CONTROLLER_H
#define FAB_CONTROLLER_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

class fab_controller
{
private:
    int IDFABRICATION;
    int IDMODELE;
    QDate DATEDEBUT;
    QDate DATEFINPREVU;
    QDate DATEFINREEL;
    QString ETATFABRICATION;
    QString TYPE_BOIS;

public:
    fab_controller();  // constructeur par défaut
    fab_controller(int IDFABRICATION, int IDMODELE, QDate DATEDEBUT,
                   QString TYPE_BOIS, QDate DATEFINPREVU, QDate DATEFINREEL,
                   QString ETATFABRICATION);

    int getIDFABRICATION();
    int getIDMODELE();
    QDate getDATEDEBUT();
    QString getTYPE_BOIS();
    QDate getDATEFINPREVU();
    QDate getDATEFINREEL();
    QString getETATFABRICATION();


    void setIDFABRICATION(int);
    void setIDMODELE(int);
    void setDATEDEBUT(QDate);
    void setTYPE_BOIS(QString);
    void setDATEFINPREVU(QDate);
    void setDATEFINREEL(QDate);
    void setETATFABRICATION(QString);


    bool add();
    bool modify(int id);
    bool delete_2(int id);
};

#endif
