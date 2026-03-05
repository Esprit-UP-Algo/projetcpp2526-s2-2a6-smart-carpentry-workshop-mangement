/********************************************************************************
** Form generated from reading UI file 'supprimer_fab.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUPPRIMER_FAB_H
#define UI_SUPPRIMER_FAB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog_Supprimer_Fabrication
{
public:
    QVBoxLayout *layout_principal;
    QFrame *frame_titre;
    QHBoxLayout *layout_titre;
    QLabel *lbl_titre;
    QSpacerItem *spacer_top;
    QFrame *frame_contenu;
    QVBoxLayout *layout_contenu;
    QHBoxLayout *layout_msg;
    QLabel *lbl_icone;
    QVBoxLayout *layout_texte;
    QLabel *lbl_message;
    QLabel *lbl_sous_message;
    QSpacerItem *spacer_mid;
    QHBoxLayout *layout_boutons;
    QSpacerItem *sp_l;
    QPushButton *btn_supprimer;
    QSpacerItem *sp_m;
    QPushButton *btn_annuler;
    QSpacerItem *sp_r;

    void setupUi(QDialog *Dialog_Supprimer_Fabrication)
    {
        if (Dialog_Supprimer_Fabrication->objectName().isEmpty())
            Dialog_Supprimer_Fabrication->setObjectName("Dialog_Supprimer_Fabrication");
        Dialog_Supprimer_Fabrication->resize(400, 220);
        Dialog_Supprimer_Fabrication->setStyleSheet(QString::fromUtf8("QDialog { background-color: #f0e6d3; }\n"
"QLabel { color: #3b1a08; font-family: \"Segoe UI\"; font-size: 10pt; font-weight: bold; }\n"
"QLabel#lbl_titre { color: #f0e6d3; font-size: 12pt; }\n"
"QLabel#lbl_icone { color: #c0392b; font-size: 28pt; }\n"
"QLabel#lbl_message { color: #3b1a08; font-size: 11pt; font-weight: bold; }\n"
"QLabel#lbl_sous_message { color: #7a5a3a; font-size: 9pt; font-weight: normal; }\n"
"QPushButton { background-color: #6b2d0e; color: #f0e6d3; border: none; border-radius: 5px; padding: 8px 20px; font-family: \"Segoe UI\"; font-size: 10pt; font-weight: bold; }\n"
"QPushButton:hover { background-color: #8b3d1a; }\n"
"QPushButton:pressed { background-color: #4a1e08; }\n"
"QPushButton#btn_supprimer { background-color: #8b1a1a; }\n"
"QPushButton#btn_supprimer:hover { background-color: #a52020; }\n"
"QPushButton#btn_supprimer:pressed { background-color: #5c1010; }\n"
"QPushButton#btn_annuler { background-color: #9e7b5a; }\n"
"QPushButton#btn_annuler:hover { background-color: #b8926e; }\n"
""
                        "QPushButton#btn_annuler:pressed { background-color: #7a5a3a; }\n"
"QFrame#frame_titre { background-color: #6b2d0e; border-radius: 5px; }\n"
"QFrame#frame_contenu { background-color: #f5ede0; border: 2px solid #8b5e3c; border-radius: 5px; }"));
        layout_principal = new QVBoxLayout(Dialog_Supprimer_Fabrication);
        layout_principal->setSpacing(0);
        layout_principal->setObjectName("layout_principal");
        layout_principal->setContentsMargins(12, 12, 12, 12);
        frame_titre = new QFrame(Dialog_Supprimer_Fabrication);
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

        frame_contenu = new QFrame(Dialog_Supprimer_Fabrication);
        frame_contenu->setObjectName("frame_contenu");
        frame_contenu->setFrameShape(QFrame::StyledPanel);
        layout_contenu = new QVBoxLayout(frame_contenu);
        layout_contenu->setSpacing(10);
        layout_contenu->setObjectName("layout_contenu");
        layout_contenu->setContentsMargins(20, 18, 20, 18);
        layout_msg = new QHBoxLayout();
        layout_msg->setObjectName("layout_msg");
        lbl_icone = new QLabel(frame_contenu);
        lbl_icone->setObjectName("lbl_icone");
        lbl_icone->setAlignment(Qt::AlignCenter);
        lbl_icone->setMaximumSize(QSize(50, 16777215));

        layout_msg->addWidget(lbl_icone);

        layout_texte = new QVBoxLayout();
        layout_texte->setSpacing(4);
        layout_texte->setObjectName("layout_texte");
        lbl_message = new QLabel(frame_contenu);
        lbl_message->setObjectName("lbl_message");
        lbl_message->setWordWrap(true);

        layout_texte->addWidget(lbl_message);

        lbl_sous_message = new QLabel(frame_contenu);
        lbl_sous_message->setObjectName("lbl_sous_message");

        layout_texte->addWidget(lbl_sous_message);


        layout_msg->addLayout(layout_texte);


        layout_contenu->addLayout(layout_msg);

        spacer_mid = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layout_contenu->addItem(spacer_mid);

        layout_boutons = new QHBoxLayout();
        layout_boutons->setObjectName("layout_boutons");
        sp_l = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layout_boutons->addItem(sp_l);

        btn_supprimer = new QPushButton(frame_contenu);
        btn_supprimer->setObjectName("btn_supprimer");
        btn_supprimer->setMinimumSize(QSize(110, 35));

        layout_boutons->addWidget(btn_supprimer);

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


        retranslateUi(Dialog_Supprimer_Fabrication);
        QObject::connect(btn_annuler, &QPushButton::clicked, Dialog_Supprimer_Fabrication, qOverload<>(&QDialog::reject));
        QObject::connect(btn_supprimer, &QPushButton::clicked, Dialog_Supprimer_Fabrication, qOverload<>(&QDialog::accept));

        QMetaObject::connectSlotsByName(Dialog_Supprimer_Fabrication);
    } // setupUi

    void retranslateUi(QDialog *Dialog_Supprimer_Fabrication)
    {
        Dialog_Supprimer_Fabrication->setWindowTitle(QCoreApplication::translate("Dialog_Supprimer_Fabrication", "Confirmation Suppression", nullptr));
        lbl_titre->setText(QCoreApplication::translate("Dialog_Supprimer_Fabrication", "Supprimer une Fabrication", nullptr));
        lbl_icone->setText(QCoreApplication::translate("Dialog_Supprimer_Fabrication", "\342\232\240", nullptr));
        lbl_message->setText(QCoreApplication::translate("Dialog_Supprimer_Fabrication", "\303\212tes-vous s\303\273r de vouloir supprimer ?", nullptr));
        lbl_sous_message->setText(QCoreApplication::translate("Dialog_Supprimer_Fabrication", "Cette action est irr\303\251versible.", nullptr));
        btn_supprimer->setText(QCoreApplication::translate("Dialog_Supprimer_Fabrication", "Supprimer", nullptr));
        btn_annuler->setText(QCoreApplication::translate("Dialog_Supprimer_Fabrication", "Annuler", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_Supprimer_Fabrication: public Ui_Dialog_Supprimer_Fabrication {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUPPRIMER_FAB_H
