/********************************************************************************
** Form generated from reading UI file 'modifier_fab.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MODIFIER_FAB_H
#define UI_MODIFIER_FAB_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog_Modifier_Fabrication
{
public:
    QVBoxLayout *layout_principal;
    QFrame *frame_titre;
    QHBoxLayout *layout_titre;
    QLabel *lbl_titre;
    QSpacerItem *spacer_top;
    QFrame *frame_contenu;
    QVBoxLayout *layout_contenu;
    QHBoxLayout *layout_id;
    QLabel *lbl_id;
    QLineEdit *input_id_fabrication;
    QHBoxLayout *layout_modele;
    QLabel *lbl_modele;
    QLineEdit *input_modele;
    QHBoxLayout *layout_date;
    QLabel *lbl_date_debut;
    QDateEdit *input_date_debut;
    QSpacerItem *spacer_mid;
    QHBoxLayout *layout_boutons;
    QSpacerItem *sp_l;
    QPushButton *btn_modifier;
    QSpacerItem *sp_m;
    QPushButton *btn_annuler;
    QSpacerItem *sp_r;

    void setupUi(QDialog *Dialog_Modifier_Fabrication)
    {
        if (Dialog_Modifier_Fabrication->objectName().isEmpty())
            Dialog_Modifier_Fabrication->setObjectName("Dialog_Modifier_Fabrication");
        Dialog_Modifier_Fabrication->resize(420, 320);
        Dialog_Modifier_Fabrication->setStyleSheet(QString::fromUtf8("QDialog { background-color: #f0e6d3; }\n"
"QLabel { color: #3b1a08; font-family: \"Segoe UI\"; font-size: 10pt; font-weight: bold; }\n"
"QLineEdit { background-color: #fdf6ec; border: 1px solid #8b5e3c; border-radius: 3px; padding: 3px 6px; color: #3b1a08; font-family: \"Segoe UI\"; font-size: 10pt; }\n"
"QLineEdit:focus { border: 1.5px solid #5a2d0c; }\n"
"QDateEdit { background-color: #fdf6ec; border: 1px solid #8b5e3c; border-radius: 3px; padding: 3px 6px; color: #3b1a08; font-family: \"Segoe UI\"; font-size: 10pt; }\n"
"QDateEdit:focus { border: 1.5px solid #5a2d0c; }\n"
"QDateEdit::drop-down { border: none; background-color: #8b5e3c; width: 20px; }\n"
"QPushButton { background-color: #6b2d0e; color: #f0e6d3; border: none; border-radius: 5px; padding: 8px 20px; font-family: \"Segoe UI\"; font-size: 10pt; font-weight: bold; }\n"
"QPushButton:hover { background-color: #8b3d1a; }\n"
"QPushButton:pressed { background-color: #4a1e08; }\n"
"QPushButton#btn_annuler { background-color: #9e7b5a; }\n"
"QPushButton#b"
                        "tn_annuler:hover { background-color: #b8926e; }\n"
"QPushButton#btn_annuler:pressed { background-color: #7a5a3a; }\n"
"QFrame#frame_titre { background-color: #6b2d0e; border-radius: 5px; }\n"
"QLabel#lbl_titre { color: #f0e6d3; font-size: 12pt; }\n"
"QFrame#frame_contenu { background-color: #f5ede0; border: 2px solid #8b5e3c; border-radius: 5px; }"));
        layout_principal = new QVBoxLayout(Dialog_Modifier_Fabrication);
        layout_principal->setSpacing(0);
        layout_principal->setObjectName("layout_principal");
        layout_principal->setContentsMargins(12, 12, 12, 12);
        frame_titre = new QFrame(Dialog_Modifier_Fabrication);
        frame_titre->setObjectName("frame_titre");
        frame_titre->setFrameShape(QFrame::StyledPanel);
        layout_titre = new QHBoxLayout(frame_titre);
        layout_titre->setObjectName("layout_titre");
        layout_titre->setContentsMargins(10, 8, 10, 8);
        lbl_titre = new QLabel(frame_titre);
        lbl_titre->setObjectName("lbl_titre");
        lbl_titre->setAlignment(Qt::AlignCenter);

        layout_titre->addWidget(lbl_titre);


        layout_principal->addWidget(frame_titre);

        spacer_top = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layout_principal->addItem(spacer_top);

        frame_contenu = new QFrame(Dialog_Modifier_Fabrication);
        frame_contenu->setObjectName("frame_contenu");
        frame_contenu->setFrameShape(QFrame::StyledPanel);
        layout_contenu = new QVBoxLayout(frame_contenu);
        layout_contenu->setSpacing(12);
        layout_contenu->setObjectName("layout_contenu");
        layout_contenu->setContentsMargins(20, 18, 20, 18);
        layout_id = new QHBoxLayout();
        layout_id->setObjectName("layout_id");
        lbl_id = new QLabel(frame_contenu);
        lbl_id->setObjectName("lbl_id");
        lbl_id->setMinimumSize(QSize(130, 0));

        layout_id->addWidget(lbl_id);

        input_id_fabrication = new QLineEdit(frame_contenu);
        input_id_fabrication->setObjectName("input_id_fabrication");

        layout_id->addWidget(input_id_fabrication);


        layout_contenu->addLayout(layout_id);

        layout_modele = new QHBoxLayout();
        layout_modele->setObjectName("layout_modele");
        lbl_modele = new QLabel(frame_contenu);
        lbl_modele->setObjectName("lbl_modele");
        lbl_modele->setMinimumSize(QSize(130, 0));

        layout_modele->addWidget(lbl_modele);

        input_modele = new QLineEdit(frame_contenu);
        input_modele->setObjectName("input_modele");

        layout_modele->addWidget(input_modele);


        layout_contenu->addLayout(layout_modele);

        layout_date = new QHBoxLayout();
        layout_date->setObjectName("layout_date");
        lbl_date_debut = new QLabel(frame_contenu);
        lbl_date_debut->setObjectName("lbl_date_debut");
        lbl_date_debut->setMinimumSize(QSize(130, 0));

        layout_date->addWidget(lbl_date_debut);

        input_date_debut = new QDateEdit(frame_contenu);
        input_date_debut->setObjectName("input_date_debut");
        input_date_debut->setCalendarPopup(true);
        input_date_debut->setDate(QDate(2000, 1, 1));

        layout_date->addWidget(input_date_debut);


        layout_contenu->addLayout(layout_date);

        spacer_mid = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layout_contenu->addItem(spacer_mid);

        layout_boutons = new QHBoxLayout();
        layout_boutons->setObjectName("layout_boutons");
        sp_l = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layout_boutons->addItem(sp_l);

        btn_modifier = new QPushButton(frame_contenu);
        btn_modifier->setObjectName("btn_modifier");
        btn_modifier->setMinimumSize(QSize(110, 35));

        layout_boutons->addWidget(btn_modifier);

        sp_m = new QSpacerItem(15, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layout_boutons->addItem(sp_m);

        btn_annuler = new QPushButton(frame_contenu);
        btn_annuler->setObjectName("btn_annuler");
        btn_annuler->setMinimumSize(QSize(110, 35));

        layout_boutons->addWidget(btn_annuler);

        sp_r = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layout_boutons->addItem(sp_r);


        layout_contenu->addLayout(layout_boutons);


        layout_principal->addWidget(frame_contenu);


        retranslateUi(Dialog_Modifier_Fabrication);
        QObject::connect(btn_annuler, &QPushButton::clicked, Dialog_Modifier_Fabrication, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(Dialog_Modifier_Fabrication);
    } // setupUi

    void retranslateUi(QDialog *Dialog_Modifier_Fabrication)
    {
        Dialog_Modifier_Fabrication->setWindowTitle(QCoreApplication::translate("Dialog_Modifier_Fabrication", "Modifier Fabrication", nullptr));
        lbl_titre->setText(QCoreApplication::translate("Dialog_Modifier_Fabrication", "Modifier une Fabrication", nullptr));
        lbl_id->setText(QCoreApplication::translate("Dialog_Modifier_Fabrication", "ID Fabrication :", nullptr));
        input_id_fabrication->setPlaceholderText(QCoreApplication::translate("Dialog_Modifier_Fabrication", "Ex : FAB-001", nullptr));
        lbl_modele->setText(QCoreApplication::translate("Dialog_Modifier_Fabrication", "Mod\303\250le :", nullptr));
        input_modele->setPlaceholderText(QCoreApplication::translate("Dialog_Modifier_Fabrication", "Nom du mod\303\250le", nullptr));
        lbl_date_debut->setText(QCoreApplication::translate("Dialog_Modifier_Fabrication", "Date De D\303\251but :", nullptr));
        input_date_debut->setDisplayFormat(QCoreApplication::translate("Dialog_Modifier_Fabrication", "dd/MM/yyyy", nullptr));
        btn_modifier->setText(QCoreApplication::translate("Dialog_Modifier_Fabrication", "Modifier", nullptr));
        btn_annuler->setText(QCoreApplication::translate("Dialog_Modifier_Fabrication", "Annuler", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_Modifier_Fabrication: public Ui_Dialog_Modifier_Fabrication {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MODIFIER_FAB_H
