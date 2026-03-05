/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Login
{
public:
    QLabel *label;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QLabel *label_3;
    QLineEdit *lineEdit_2;
    QPushButton *btn_signin;

    void setupUi(QDialog *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName("Login");
        Login->resize(920, 608);
        Login->setStyleSheet(QString::fromUtf8("/* --- Fond de la fen\303\252tre de Login --- */\n"
"#LoginWindow, QWidget {\n"
"    background-color: #F5EBD7; /* M\303\252me beige que la MainWindow */\n"
"}\n"
"\n"
"/* --- Style des Textes (EMAIL, PASSWORD, etc.) --- */\n"
"QLabel {\n"
"    color: #3E2723; /* Marron tr\303\250s fonc\303\251 */\n"
"    font-weight: bold;\n"
"    font-size: 12px;\n"
"    background: transparent;\n"
"}\n"
"\n"
"/* --- Style des champs de saisie (Email et Mot de passe) --- */\n"
"QLineEdit {\n"
"    background-color: #FFFFFF; /* Blanc pur */\n"
"    border: 2px solid #A1887F; /* Bordure marron clair */\n"
"    border-radius: 8px;\n"
"    padding: 8px;\n"
"    color: #3E2723;\n"
"    selection-background-color: #D7CCC8;\n"
"}\n"
"\n"
"/* Effet quand on clique dans le champ */\n"
"QLineEdit:focus {\n"
"    border: 2px solid #5D4037; /* Bordure plus fonc\303\251e au focus */\n"
"}\n"
"\n"
"/* --- Le bouton SIGN IN --- */\n"
"QPushButton {\n"
"    background-color: #5D4037; /* Marron chocolat */\n"
"    color: white;\n"
"    borde"
                        "r-radius: 10px;\n"
"    padding: 10px 20px;\n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"    min-width: 100px;\n"
"}\n"
"\n"
"/* Effet de survol (hover) */\n"
"QPushButton:hover {\n"
"    background-color: #795548; /* Marron un peu plus clair au survol */\n"
"}\n"
"\n"
"/* Effet quand on clique sur le bouton */\n"
"QPushButton:pressed {\n"
"    background-color: #3E2723;\n"
"}\n"
"\n"
"/* --- Style sp\303\251cifique pour le cadre du Logo (si n\303\251cessaire) --- */\n"
"/* Si ton label de logo a un cadre que tu veux styliser */\n"
"QLabel#label_logo {\n"
"    border: none;\n"
"    background: transparent;\n"
"}"));
        label = new QLabel(Login);
        label->setObjectName("label");
        label->setGeometry(QRect(250, 50, 411, 231));
        label->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/Image2 (2).png")));
        label_2 = new QLabel(Login);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(200, 270, 131, 51));
        lineEdit = new QLineEdit(Login);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(200, 320, 491, 41));
        label_3 = new QLabel(Login);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(200, 380, 171, 31));
        lineEdit_2 = new QLineEdit(Login);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setGeometry(QRect(200, 420, 491, 41));
        btn_signin = new QPushButton(Login);
        btn_signin->setObjectName("btn_signin");
        btn_signin->setGeometry(QRect(370, 480, 140, 51));
        btn_signin->setStyleSheet(QString::fromUtf8("font: 11pt \"Segoe UI\";\n"
"font: 700 11pt \"Segoe UI\";"));

        retranslateUi(Login);

        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QDialog *Login)
    {
        Login->setWindowTitle(QCoreApplication::translate("Login", "Dialog", nullptr));
        label->setText(QString());
        label_2->setText(QCoreApplication::translate("Login", "<html><head/><body><p><span style=\" font-size:14pt;\">EMAIL :</span></p></body></html>", nullptr));
        label_3->setText(QCoreApplication::translate("Login", "<html><head/><body><p><span style=\" font-size:14pt;\">PASSWORD :</span></p></body></html>", nullptr));
        btn_signin->setText(QCoreApplication::translate("Login", "SIGN IN", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
