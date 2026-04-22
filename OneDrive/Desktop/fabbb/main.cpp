#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "connexion.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "=== DEBUT DE L'APPLICATION ===";

    // Afficher les drivers SQL disponibles
    qDebug() << "Drivers SQL disponibles:";
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver, drivers) {
        qDebug() << "  -" << driver;
    }

    // Tester la connexion — utilise createInstance() et createconnect()
    Connexion& conn = Connexion::createInstance();

    qDebug() << "Test de connexion...";

    if (!conn.createconnect()) {
        qDebug() << "Erreur de connexion!";
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("Erreur");
        msgBox.setText("Impossible de se connecter à la base de données Oracle.\n\n"
                       "Vérifiez:\n"
                       "- La source ODBC 'woodpilot' existe\n"
                       "- L'utilisateur 'youssef' est correct\n"
                       "- Le mot de passe '1234' est correct\n"
                       "- La base Oracle est demarree\n\n"
                       "Drivers disponibles: " + drivers.join(", "));
        msgBox.setStyleSheet(
            "QMessageBox { background-color: white; border-radius: 10px; }"
            "QLabel { color: #2E1A00; background-color: white; font-size: 12px; }"
            "QPushButton { background-color: #7F4129; color: white; border: none; border-radius: 6px; padding: 8px 20px; }"
            "QPushButton:hover { background-color: #9C5233; }"
            );
        msgBox.exec();
        return -1;
    }

    qDebug() << "Connexion reussie!";

    MainWindow w;
    w.show();

    qDebug() << "Fenetre affichee";

    return a.exec();
}
