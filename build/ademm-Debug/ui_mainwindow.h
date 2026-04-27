/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label_2;
    QWidget *menu_amis;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *btn_personnel;
    QPushButton *btn_modele;
    QPushButton *btn_bois;
    QPushButton *btn_etape;
    QPushButton *btn_fabrication;
    QLabel *label_17;
    QStackedWidget *stackedWidget;
    QWidget *sqx;
    QWidget *page_4;
    QTabWidget *tabWidget;
    QWidget *tab_gestion_2;
    QGroupBox *groupBox_2;
    QLabel *lbl_id_fabrication;
    QLineEdit *aff_id_fab;
    QLabel *lbl_modele;
    QLineEdit *aff_id_modele;
    QLabel *lbl_date_deb;
    QDateEdit *aff_date_deb;
    QLabel *lbl_quantite;
    QSpinBox *aff_quantite;
    QLabel *lbl_qualite;
    QComboBox *aff_qualite;
    QLabel *lbl_commentaire;
    QTextEdit *aff_commentaire;
    QPushButton *btn_ajouter_2;
    QPushButton *btn_modifier;
    QPushButton *btn_supprimer;
    QGroupBox *groupBox_tableau_2;
    QVBoxLayout *verticalLayoutRight_2;
    QHBoxLayout *horizontalLayoutSearch_2;
    QLabel *recherche_6;
    QLineEdit *rech_7;
    QPushButton *btn_rechercher_6;
    QTableWidget *table_modeles_2;
    QHBoxLayout *horizontalLayoutBottom_2;
    QLabel *trie_6;
    QComboBox *cb_critere_recherche_modele_2;
    QPushButton *btn_tire_2;
    QPushButton *btn_export_pdf_modele_2;
    QSpacerItem *horizontalSpacer_2;
    QWidget *tab_stats_2;
    QVBoxLayout *vl_suivi_global;
    QHBoxLayout *hl_top_zone;
    QGroupBox *gb_catalogue_etapes;
    QVBoxLayout *vl_catalogue;
    QTableWidget *table_etapes_catalogue;
    QGroupBox *gb_fabrications_suivi;
    QVBoxLayout *vl_fab_suivi;
    QTreeWidget *tree_fabrications_etapes;
    QFrame *separator_suivi;
    QGroupBox *gb_timeline_visuelle;
    QVBoxLayout *vl_timeline_box;
    QHBoxLayout *hl_legende_bas;
    QLabel *lbl_leg_title;
    QLabel *lbl_leg_v;
    QLabel *lbl_leg_o;
    QLabel *lbl_leg_r;
    QSpacerItem *hs_leg_mid;
    QPushButton *btn_optimizer_ai;
    QTextEdit *te_ai_result;
    QScrollArea *scroll_timeline;
    QWidget *timeline_contents;
    QHBoxLayout *hl_timeline_steps;
    QLabel *lbl_timeline_vide;
    QSpacerItem *hs_timeline_end;
    QWidget *page;
    QTabWidget *tabBois;
    QWidget *tab_gestion_7;
    QGroupBox *groupBox_13;
    QLabel *label_fourn;
    QLabel *label_empl;
    QLabel *label_date_bois;
    QLabel *label_type_bois2;
    QLabel *label_etat_bois;
    QLabel *label_nom_bois;
    QLabel *label_prix;
    QLabel *label_dimension;
    QLineEdit *la_fournisseur_7;
    QLineEdit *la_emplacement_7;
    QDateEdit *la_date_7;
    QComboBox *la_type_7;
    QComboBox *la_etat_7;
    QComboBox *la_nom_7;
    QDoubleSpinBox *la_prix_7;
    QDoubleSpinBox *la_l_7;
    QDoubleSpinBox *la_w_7;
    QDoubleSpinBox *la_h_7;
    QPushButton *la_ajouter_7;
    QPushButton *la_modifier_7;
    QPushButton *la_supprimer_7;
    QGroupBox *groupBox_10;
    QLabel *date_fin_prev_5;
    QLabel *date_fin_reel_5;
    QLineEdit *rech_4;
    QPushButton *btn_rechercher_4;
    QPushButton *btn_trier_4;
    QLabel *trie_3;
    QTableView *tab_rech_2;
    QLabel *recherche_3;
    QPushButton *la_pdf_9;
    QComboBox *la_tri_7;
    QWidget *tab_stats_7;
    QPushButton *stat_bois;
    QTextEdit *textEdit_bois;
    QWidget *tab_optimisatione_7;
    QGroupBox *groupBox_14;
    QLabel *label_long;
    QLineEdit *la_id_14;
    QLabel *label_formedem;
    QComboBox *la_forme_7;
    QPushButton *calcul_decoupage_7;
    QTextEdit *la_textEdit_21;
    QWidget *tab_7;
    QTextEdit *la_textEdit_22;
    QLineEdit *la_question_7;
    QPushButton *poser_7;
    QWidget *page_7;
    QWidget *page_6;
    QTabWidget *tabWidget_modeles;
    QWidget *tab_gestion_modeles;
    QHBoxLayout *horizontalLayoutMain;
    QGroupBox *groupBox_modeles;
    QVBoxLayout *verticalLayoutForm;
    QFormLayout *formLayout;
    QLabel *label_id_mod;
    QLabel *label_nom_mod;
    QLineEdit *le_nom_modele;
    QLabel *label_Type;
    QComboBox *combo_type;
    QLabel *label_id_bois;
    QLineEdit *le_id_bois;
    QLabel *label_longueur;
    QLineEdit *le_longueur;
    QLabel *label_largeur;
    QLineEdit *le_largeur;
    QLabel *label_hauteur;
    QLineEdit *le_hauteur;
    QLabel *label_créepar;
    QLineEdit *le_créepar;
    QLabel *label_date_mod;
    QDateEdit *de_date_creation;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *vButtonsLayout;
    QPushButton *btn_ajouter_modele;
    QHBoxLayout *hButtonsAction;
    QPushButton *btn_modifier_modele;
    QPushButton *btn_supprimer_modele;
    QGroupBox *groupBox_tableau;
    QVBoxLayout *verticalLayoutRight;
    QHBoxLayout *horizontalLayoutSearch;
    QLabel *recherche_5;
    QLineEdit *rech_6;
    QPushButton *btn_rechercher_5;
    QTableWidget *table_modeles;
    QHBoxLayout *horizontalLayoutBottom;
    QLabel *trie_5;
    QComboBox *cb_critere_recherche_modele;
    QPushButton *btn_tire;
    QPushButton *btn_export_pdf_modele;
    QSpacerItem *horizontalSpacer;
    QWidget *tab_statistiques;
    QVBoxLayout *verticalLayoutStats;
    QWidget *placeholder_stats;
    QWidget *tab_tendances;
    QVBoxLayout *verticalLayoutTendances;
    QWidget *placeholder_tendances;
    QWidget *tab_couts;
    QVBoxLayout *verticalLayoutCouts;
    QWidget *placeholder_couts;
    QWidget *page_5;
    QTabWidget *tabWidget_2;
    QWidget *tab_gestion_3;
    QGroupBox *groupBox_3;
    QLabel *label_3;
    QLabel *label_5;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_15;
    QLineEdit *le_cin;
    QLineEdit *le_nom_2;
    QLineEdit *le_prenom;
    QDateEdit *de_naissance;
    QComboBox *cb_poste;
    QDateEdit *de_embauche;
    QLineEdit *le_salaire;
    QLineEdit *le_tel;
    QLineEdit *le_rfid;
    QPushButton *btn_ajouter_3;
    QPushButton *btn_modifier_2;
    QPushButton *btn_supprimer_2;
    QGroupBox *groupBox_11;
    QLabel *date_fin_prev_6;
    QLabel *date_fin_reel_6;
    QLineEdit *rech_5;
    QPushButton *btn_rechercher_3;
    QPushButton *btn_trier_3;
    QLabel *trie_4;
    QTableView *tab_rech_3;
    QLabel *recherche_4;
    QPushButton *la_pdf_10;
    QComboBox *cb_critere_recherche;
    QComboBox *cb_tri;
    QWidget *tab_stats_3;
    QWidget *tab_avance_3;
    QGroupBox *groupBox_6;
    QLabel *label_16;
    QLabel *lbl_resultat_prime;
    QPushButton *btn_calcul_prime;
    QGroupBox *groupBox_7;
    QLabel *lbl_etat_pointage;
    QPushButton *btn_simuler_rfid;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1403, 749);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"/* ============================================================\n"
"   CSS GLOBAL UNIFIE - WoodPilot\n"
"   ============================================================ */\n"
"\n"
"QMainWindow, QWidget#centralwidget {\n"
"    background-color: #EDE0C8;\n"
"}\n"
"\n"
"QStackedWidget {\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"QTabWidget::pane {\n"
"    border: 1px solid #8D6E63;\n"
"    background-color: #EDE0C8;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background-color: #C8B89A;\n"
"    color: #3E2000;\n"
"    padding: 10px 25px;\n"
"    border-top-left-radius: 6px;\n"
"    border-top-right-radius: 6px;\n"
"    margin-right: 3px;\n"
"    font-weight: bold;\n"
"    font-size: 12px;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    background-color: #7F4129;\n"
"    color: #FFFFFF;\n"
"}\n"
"\n"
"QTabBar::tab:hover:!selected {\n"
"    background-color: #B8A080;\n"
"}\n"
"\n"
"QLabel {\n"
"    color: #2E1A00;\n"
"    font-weight: bold;\n"
"    font-size: 12px;\n"
"    backgr"
                        "ound-color: transparent;\n"
"}\n"
"\n"
"QGroupBox {\n"
"    background-color: #FDFAF4;\n"
"    border: 2px solid #B8956A;\n"
"    border-radius: 10px;\n"
"    margin-top: 18px;\n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"    color: #4A2800;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    padding: 2px 8px;\n"
"    color: #4A2800;\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"QLineEdit, QDateEdit, QSpinBox, QDoubleSpinBox {\n"
"    background-color: #FFFFFF;\n"
"    border: 1px solid #C0A882;\n"
"    border-radius: 4px;\n"
"    padding: 5px 8px;\n"
"    color: #2E1A00;\n"
"    font-size: 12px;\n"
"    selection-background-color: #8D6E63;\n"
"}\n"
"\n"
"QLineEdit:focus, QDateEdit:focus {\n"
"    border: 2px solid #7F4129;\n"
"    background-color: #FFFEF8;\n"
"}\n"
"\n"
"QComboBox {\n"
"    background-color: #FFFFFF;\n"
"    border: 1px solid #C0A882;\n"
"    border-radius: 4px;\n"
"    padding: 5px 8px;\n"
"    color: #2E1A00"
                        ";\n"
"    font-size: 12px;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    background-color: #D4B896;\n"
"    width: 24px;\n"
"    border-top-right-radius: 4px;\n"
"    border-bottom-right-radius: 4px;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FFFFFF;\n"
"    border: 1px solid #C0A882;\n"
"    color: #2E1A00;\n"
"    selection-background-color: #7F4129;\n"
"    selection-color: white;\n"
"}\n"
"\n"
"QPushButton {\n"
"    background-color: #7F4129;\n"
"    color: #FFFFFF;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 9px 18px;\n"
"    font-weight: bold;\n"
"    font-size: 12px;\n"
"    min-height: 30px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #9C5233;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #5C2E1A;\n"
"}\n"
"\n"
"QTableView, QTableWidget {\n"
"    background-color: #FFFFFF;\n"
"    border: 1px solid #B8956A;\n"
"    border-radius: 6px;\n"
"    gridline-color: #E8D5BA;\n"
"    font-size: 12px;\n"
""
                        "    color: #2E1A00;\n"
"    alternate-background-color: #FAF4EA;\n"
"    selection-background-color: transparent;\n"
"    selection-color: #2E1A00;\n"
"}\n"
"\n"
"QTableView::item:selected,\n"
"QTableWidget::item:selected {\n"
"    background-color: #D4851A;\n"
"    color: #FFFFFF;\n"
"    font-weight: bold;\n"
"    border-left: 3px solid #7F4129;\n"
"}\n"
"\n"
"QTableView::item:hover,\n"
"QTableWidget::item:hover {\n"
"    background-color: #F0C080;\n"
"    color: #2E1A00;\n"
"}\n"
"\n"
"QTableView::item:selected:hover,\n"
"QTableWidget::item:selected:hover {\n"
"    background-color: #C07010;\n"
"    color: #FFFFFF;\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background-color: #7F4129;\n"
"    color: #FFFFFF;\n"
"    padding: 7px 5px;\n"
"    font-weight: bold;\n"
"    font-size: 12px;\n"
"    border: 1px solid #5C2E1A;\n"
"}\n"
"\n"
"QTextEdit {\n"
"    background-color: #FDFAF4;\n"
"    border: 2px solid #B8956A;\n"
"    border-radius: 8px;\n"
"    padding: 8px;\n"
"    color: #2E1A00;\n"
"    font-size:"
                        " 11pt;\n"
"}\n"
"\n"
"QListWidget {\n"
"    background-color: #FFFFFF;\n"
"    border: 1px solid #B8956A;\n"
"    border-radius: 6px;\n"
"    color: #2E1A00;\n"
"}\n"
"\n"
"QListWidget::item {\n"
"    padding: 8px;\n"
"    border-bottom: 1px solid #E8D5BA;\n"
"}\n"
"\n"
"QListWidget::item:selected {\n"
"    background-color: #7F4129;\n"
"    color: #FFFFFF;\n"
"}\n"
"\n"
"QProgressBar {\n"
"    border: 2px solid #8D6E63;\n"
"    border-radius: 5px;\n"
"    text-align: center;\n"
"    background-color: #F0EBE0;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"    background-color: #7F4129;\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"QScrollBar:vertical {\n"
"    background: #EDE0C8;\n"
"    width: 10px;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background: #7F4129;\n"
"    border-radius: 5px;\n"
"    min-height: 20px;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }\n"
"\n"
"QScrollBar:horizontal {\n"
"    background: #EDE0C8;\n"
"    hei"
                        "ght: 10px;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"    background: #7F4129;\n"
"    border-radius: 5px;\n"
"    min-width: 20px;\n"
"}\n"
"\n"
"QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }\n"
"\n"
"QMenuBar {\n"
"    background-color: #5C2E1A;\n"
"    color: #FFFFFF;\n"
"}\n"
"\n"
"QMenuBar::item:selected {\n"
"    background-color: #7F4129;\n"
"}\n"
"\n"
"QStatusBar {\n"
"    background-color: #5C2E1A;\n"
"    color: #FFFFFF;\n"
"    font-size: 11px;\n"
"}\n"
"   "));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(520, 10, 291, 51));
        QFont font;
        font.setBold(true);
        font.setItalic(true);
        label_2->setFont(font);
        menu_amis = new QWidget(centralwidget);
        menu_amis->setObjectName("menu_amis");
        menu_amis->setGeometry(QRect(0, 80, 151, 561));
        layoutWidget = new QWidget(menu_amis);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(5, 73, 128, 340));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        btn_personnel = new QPushButton(layoutWidget);
        btn_personnel->setObjectName("btn_personnel");

        verticalLayout->addWidget(btn_personnel);

        btn_modele = new QPushButton(layoutWidget);
        btn_modele->setObjectName("btn_modele");

        verticalLayout->addWidget(btn_modele);

        btn_bois = new QPushButton(layoutWidget);
        btn_bois->setObjectName("btn_bois");

        verticalLayout->addWidget(btn_bois);

        btn_etape = new QPushButton(layoutWidget);
        btn_etape->setObjectName("btn_etape");

        verticalLayout->addWidget(btn_etape);

        btn_fabrication = new QPushButton(layoutWidget);
        btn_fabrication->setObjectName("btn_fabrication");

        verticalLayout->addWidget(btn_fabrication);

        label_17 = new QLabel(menu_amis);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(20, 0, 126, 88));
        label_17->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix2/logo interface.png")));
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(140, 80, 1251, 611));
        stackedWidget->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
        sqx = new QWidget();
        sqx->setObjectName("sqx");
        stackedWidget->addWidget(sqx);
        page_4 = new QWidget();
        page_4->setObjectName("page_4");
        tabWidget = new QTabWidget(page_4);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(0, 0, 1231, 621));
        tab_gestion_2 = new QWidget();
        tab_gestion_2->setObjectName("tab_gestion_2");
        groupBox_2 = new QGroupBox(tab_gestion_2);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(10, 0, 361, 571));
        lbl_id_fabrication = new QLabel(groupBox_2);
        lbl_id_fabrication->setObjectName("lbl_id_fabrication");
        lbl_id_fabrication->setGeometry(QRect(20, 30, 141, 20));
        aff_id_fab = new QLineEdit(groupBox_2);
        aff_id_fab->setObjectName("aff_id_fab");
        aff_id_fab->setGeometry(QRect(170, 28, 151, 28));
        lbl_modele = new QLabel(groupBox_2);
        lbl_modele->setObjectName("lbl_modele");
        lbl_modele->setGeometry(QRect(20, 75, 141, 20));
        aff_id_modele = new QLineEdit(groupBox_2);
        aff_id_modele->setObjectName("aff_id_modele");
        aff_id_modele->setGeometry(QRect(170, 73, 151, 28));
        lbl_date_deb = new QLabel(groupBox_2);
        lbl_date_deb->setObjectName("lbl_date_deb");
        lbl_date_deb->setGeometry(QRect(20, 120, 141, 20));
        aff_date_deb = new QDateEdit(groupBox_2);
        aff_date_deb->setObjectName("aff_date_deb");
        aff_date_deb->setGeometry(QRect(170, 118, 151, 29));
        aff_date_deb->setCalendarPopup(true);
        lbl_quantite = new QLabel(groupBox_2);
        lbl_quantite->setObjectName("lbl_quantite");
        lbl_quantite->setGeometry(QRect(20, 165, 141, 20));
        aff_quantite = new QSpinBox(groupBox_2);
        aff_quantite->setObjectName("aff_quantite");
        aff_quantite->setGeometry(QRect(170, 163, 151, 28));
        aff_quantite->setMinimum(1);
        aff_quantite->setMaximum(99999);
        lbl_qualite = new QLabel(groupBox_2);
        lbl_qualite->setObjectName("lbl_qualite");
        lbl_qualite->setGeometry(QRect(20, 210, 141, 20));
        aff_qualite = new QComboBox(groupBox_2);
        aff_qualite->addItem(QString());
        aff_qualite->addItem(QString());
        aff_qualite->addItem(QString());
        aff_qualite->addItem(QString());
        aff_qualite->setObjectName("aff_qualite");
        aff_qualite->setGeometry(QRect(170, 208, 151, 28));
        lbl_commentaire = new QLabel(groupBox_2);
        lbl_commentaire->setObjectName("lbl_commentaire");
        lbl_commentaire->setGeometry(QRect(20, 255, 141, 20));
        aff_commentaire = new QTextEdit(groupBox_2);
        aff_commentaire->setObjectName("aff_commentaire");
        aff_commentaire->setGeometry(QRect(20, 278, 321, 100));
        btn_ajouter_2 = new QPushButton(groupBox_2);
        btn_ajouter_2->setObjectName("btn_ajouter_2");
        btn_ajouter_2->setGeometry(QRect(10, 400, 341, 51));
        btn_ajouter_2->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        btn_modifier = new QPushButton(groupBox_2);
        btn_modifier->setObjectName("btn_modifier");
        btn_modifier->setGeometry(QRect(10, 462, 161, 51));
        btn_modifier->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        btn_supprimer = new QPushButton(groupBox_2);
        btn_supprimer->setObjectName("btn_supprimer");
        btn_supprimer->setGeometry(QRect(180, 462, 171, 51));
        btn_supprimer->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        groupBox_tableau_2 = new QGroupBox(tab_gestion_2);
        groupBox_tableau_2->setObjectName("groupBox_tableau_2");
        groupBox_tableau_2->setGeometry(QRect(380, 0, 809, 539));
        verticalLayoutRight_2 = new QVBoxLayout(groupBox_tableau_2);
        verticalLayoutRight_2->setObjectName("verticalLayoutRight_2");
        horizontalLayoutSearch_2 = new QHBoxLayout();
        horizontalLayoutSearch_2->setObjectName("horizontalLayoutSearch_2");
        recherche_6 = new QLabel(groupBox_tableau_2);
        recherche_6->setObjectName("recherche_6");

        horizontalLayoutSearch_2->addWidget(recherche_6);

        rech_7 = new QLineEdit(groupBox_tableau_2);
        rech_7->setObjectName("rech_7");

        horizontalLayoutSearch_2->addWidget(rech_7);

        btn_rechercher_6 = new QPushButton(groupBox_tableau_2);
        btn_rechercher_6->setObjectName("btn_rechercher_6");
        btn_rechercher_6->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        horizontalLayoutSearch_2->addWidget(btn_rechercher_6);


        verticalLayoutRight_2->addLayout(horizontalLayoutSearch_2);

        table_modeles_2 = new QTableWidget(groupBox_tableau_2);
        if (table_modeles_2->columnCount() < 6)
            table_modeles_2->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        table_modeles_2->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        table_modeles_2->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        table_modeles_2->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        table_modeles_2->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        table_modeles_2->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        table_modeles_2->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        table_modeles_2->setObjectName("table_modeles_2");
        table_modeles_2->setAlternatingRowColors(true);
        table_modeles_2->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        verticalLayoutRight_2->addWidget(table_modeles_2);

        horizontalLayoutBottom_2 = new QHBoxLayout();
        horizontalLayoutBottom_2->setObjectName("horizontalLayoutBottom_2");
        trie_6 = new QLabel(groupBox_tableau_2);
        trie_6->setObjectName("trie_6");

        horizontalLayoutBottom_2->addWidget(trie_6);

        cb_critere_recherche_modele_2 = new QComboBox(groupBox_tableau_2);
        cb_critere_recherche_modele_2->addItem(QString());
        cb_critere_recherche_modele_2->addItem(QString());
        cb_critere_recherche_modele_2->addItem(QString());
        cb_critere_recherche_modele_2->addItem(QString());
        cb_critere_recherche_modele_2->addItem(QString());
        cb_critere_recherche_modele_2->addItem(QString());
        cb_critere_recherche_modele_2->setObjectName("cb_critere_recherche_modele_2");

        horizontalLayoutBottom_2->addWidget(cb_critere_recherche_modele_2);

        btn_tire_2 = new QPushButton(groupBox_tableau_2);
        btn_tire_2->setObjectName("btn_tire_2");
        btn_tire_2->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        horizontalLayoutBottom_2->addWidget(btn_tire_2);

        btn_export_pdf_modele_2 = new QPushButton(groupBox_tableau_2);
        btn_export_pdf_modele_2->setObjectName("btn_export_pdf_modele_2");
        btn_export_pdf_modele_2->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        horizontalLayoutBottom_2->addWidget(btn_export_pdf_modele_2);

        horizontalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayoutBottom_2->addItem(horizontalSpacer_2);


        verticalLayoutRight_2->addLayout(horizontalLayoutBottom_2);

        tabWidget->addTab(tab_gestion_2, QString());
        tab_stats_2 = new QWidget();
        tab_stats_2->setObjectName("tab_stats_2");
        vl_suivi_global = new QVBoxLayout(tab_stats_2);
        vl_suivi_global->setSpacing(8);
        vl_suivi_global->setObjectName("vl_suivi_global");
        vl_suivi_global->setContentsMargins(10, 8, 10, 8);
        hl_top_zone = new QHBoxLayout();
        hl_top_zone->setSpacing(10);
        hl_top_zone->setObjectName("hl_top_zone");
        gb_catalogue_etapes = new QGroupBox(tab_stats_2);
        gb_catalogue_etapes->setObjectName("gb_catalogue_etapes");
        vl_catalogue = new QVBoxLayout(gb_catalogue_etapes);
        vl_catalogue->setSpacing(6);
        vl_catalogue->setObjectName("vl_catalogue");
        vl_catalogue->setContentsMargins(8, 6, 8, 8);
        table_etapes_catalogue = new QTableWidget(gb_catalogue_etapes);
        if (table_etapes_catalogue->columnCount() < 3)
            table_etapes_catalogue->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        table_etapes_catalogue->setHorizontalHeaderItem(0, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        table_etapes_catalogue->setHorizontalHeaderItem(1, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        table_etapes_catalogue->setHorizontalHeaderItem(2, __qtablewidgetitem8);
        table_etapes_catalogue->setObjectName("table_etapes_catalogue");
        table_etapes_catalogue->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        table_etapes_catalogue->setAlternatingRowColors(true);
        table_etapes_catalogue->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        vl_catalogue->addWidget(table_etapes_catalogue);


        hl_top_zone->addWidget(gb_catalogue_etapes);

        gb_fabrications_suivi = new QGroupBox(tab_stats_2);
        gb_fabrications_suivi->setObjectName("gb_fabrications_suivi");
        vl_fab_suivi = new QVBoxLayout(gb_fabrications_suivi);
        vl_fab_suivi->setSpacing(6);
        vl_fab_suivi->setObjectName("vl_fab_suivi");
        vl_fab_suivi->setContentsMargins(8, 6, 8, 8);
        tree_fabrications_etapes = new QTreeWidget(gb_fabrications_suivi);
        tree_fabrications_etapes->setObjectName("tree_fabrications_etapes");
        tree_fabrications_etapes->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        tree_fabrications_etapes->setAlternatingRowColors(true);
        tree_fabrications_etapes->setAnimated(true);

        vl_fab_suivi->addWidget(tree_fabrications_etapes);


        hl_top_zone->addWidget(gb_fabrications_suivi);


        vl_suivi_global->addLayout(hl_top_zone);

        separator_suivi = new QFrame(tab_stats_2);
        separator_suivi->setObjectName("separator_suivi");
        separator_suivi->setStyleSheet(QString::fromUtf8("color: #B8956A; background-color: #B8956A; max-height: 2px;"));
        separator_suivi->setFrameShape(QFrame::Shape::HLine);
        separator_suivi->setFrameShadow(QFrame::Shadow::Sunken);

        vl_suivi_global->addWidget(separator_suivi);

        gb_timeline_visuelle = new QGroupBox(tab_stats_2);
        gb_timeline_visuelle->setObjectName("gb_timeline_visuelle");
        vl_timeline_box = new QVBoxLayout(gb_timeline_visuelle);
        vl_timeline_box->setSpacing(4);
        vl_timeline_box->setObjectName("vl_timeline_box");
        vl_timeline_box->setContentsMargins(8, 4, 8, 8);
        hl_legende_bas = new QHBoxLayout();
        hl_legende_bas->setObjectName("hl_legende_bas");
        hl_legende_bas->setContentsMargins(0, 0, 0, 0);
        lbl_leg_title = new QLabel(gb_timeline_visuelle);
        lbl_leg_title->setObjectName("lbl_leg_title");
        lbl_leg_title->setStyleSheet(QString::fromUtf8("color:#4A2800; font-size:11px; font-weight:bold; background:transparent;"));

        hl_legende_bas->addWidget(lbl_leg_title);

        lbl_leg_v = new QLabel(gb_timeline_visuelle);
        lbl_leg_v->setObjectName("lbl_leg_v");
        lbl_leg_v->setStyleSheet(QString::fromUtf8("color:#27AE60; font-size:11px; font-weight:bold; background:transparent; padding-left:10px;"));

        hl_legende_bas->addWidget(lbl_leg_v);

        lbl_leg_o = new QLabel(gb_timeline_visuelle);
        lbl_leg_o->setObjectName("lbl_leg_o");
        lbl_leg_o->setStyleSheet(QString::fromUtf8("color:#E67E22; font-size:11px; font-weight:bold; background:transparent; padding-left:10px;"));

        hl_legende_bas->addWidget(lbl_leg_o);

        lbl_leg_r = new QLabel(gb_timeline_visuelle);
        lbl_leg_r->setObjectName("lbl_leg_r");
        lbl_leg_r->setStyleSheet(QString::fromUtf8("color:#C0392B; font-size:11px; font-weight:bold; background:transparent; padding-left:10px;"));

        hl_legende_bas->addWidget(lbl_leg_r);

        hs_leg_mid = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hl_legende_bas->addItem(hs_leg_mid);

        btn_optimizer_ai = new QPushButton(gb_timeline_visuelle);
        btn_optimizer_ai->setObjectName("btn_optimizer_ai");
        btn_optimizer_ai->setMinimumSize(QSize(160, 40));
        btn_optimizer_ai->setMaximumSize(QSize(200, 34));
        btn_optimizer_ai->setStyleSheet(QString::fromUtf8("\n"
"background-color: #2C3E50;\n"
"color: #FFFFFF;\n"
"border: none;\n"
"border-radius: 6px;\n"
"padding: 6px 14px;\n"
"font-weight: bold;\n"
"font-size: 12px;\n"
"min-height: 28px;\n"
"                "));

        hl_legende_bas->addWidget(btn_optimizer_ai);


        vl_timeline_box->addLayout(hl_legende_bas);

        te_ai_result = new QTextEdit(gb_timeline_visuelle);
        te_ai_result->setObjectName("te_ai_result");
        te_ai_result->setMaximumSize(QSize(16777215, 60));
        te_ai_result->setVisible(false);
        te_ai_result->setStyleSheet(QString::fromUtf8("\n"
"background-color: #F0FEF4;\n"
"border: 2px solid #27AE60;\n"
"border-radius: 8px;\n"
"padding: 6px 10px;\n"
"color: #1A3A1A;\n"
"font-size: 10pt;\n"
"              "));
        te_ai_result->setReadOnly(true);

        vl_timeline_box->addWidget(te_ai_result);

        scroll_timeline = new QScrollArea(gb_timeline_visuelle);
        scroll_timeline->setObjectName("scroll_timeline");
        scroll_timeline->setStyleSheet(QString::fromUtf8("\n"
"QScrollArea { background-color: #F5ECD7; border: 1px solid #D4B896; border-radius: 6px; }\n"
"QScrollArea > QWidget > QWidget { background-color: #F5ECD7; }\n"
"              "));
        scroll_timeline->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        scroll_timeline->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        scroll_timeline->setWidgetResizable(true);
        timeline_contents = new QWidget();
        timeline_contents->setObjectName("timeline_contents");
        timeline_contents->setGeometry(QRect(0, 0, 1187, 192));
        hl_timeline_steps = new QHBoxLayout(timeline_contents);
        hl_timeline_steps->setSpacing(7);
        hl_timeline_steps->setObjectName("hl_timeline_steps");
        hl_timeline_steps->setContentsMargins(16, 10, 16, 10);
        lbl_timeline_vide = new QLabel(timeline_contents);
        lbl_timeline_vide->setObjectName("lbl_timeline_vide");
        lbl_timeline_vide->setStyleSheet(QString::fromUtf8("color:#8D6E63; font-size:13px; font-style:italic; background:transparent;"));
        lbl_timeline_vide->setAlignment(Qt::AlignmentFlag::AlignCenter);

        hl_timeline_steps->addWidget(lbl_timeline_vide);

        hs_timeline_end = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hl_timeline_steps->addItem(hs_timeline_end);

        scroll_timeline->setWidget(timeline_contents);

        vl_timeline_box->addWidget(scroll_timeline);


        vl_suivi_global->addWidget(gb_timeline_visuelle);

        tabWidget->addTab(tab_stats_2, QString());
        stackedWidget->addWidget(page_4);
        page = new QWidget();
        page->setObjectName("page");
        tabBois = new QTabWidget(page);
        tabBois->setObjectName("tabBois");
        tabBois->setGeometry(QRect(10, 0, 1201, 611));
        tab_gestion_7 = new QWidget();
        tab_gestion_7->setObjectName("tab_gestion_7");
        groupBox_13 = new QGroupBox(tab_gestion_7);
        groupBox_13->setObjectName("groupBox_13");
        groupBox_13->setGeometry(QRect(20, 0, 381, 551));
        label_fourn = new QLabel(groupBox_13);
        label_fourn->setObjectName("label_fourn");
        label_fourn->setGeometry(QRect(20, 60, 121, 20));
        label_empl = new QLabel(groupBox_13);
        label_empl->setObjectName("label_empl");
        label_empl->setGeometry(QRect(20, 100, 111, 20));
        label_date_bois = new QLabel(groupBox_13);
        label_date_bois->setObjectName("label_date_bois");
        label_date_bois->setGeometry(QRect(20, 140, 131, 20));
        label_type_bois2 = new QLabel(groupBox_13);
        label_type_bois2->setObjectName("label_type_bois2");
        label_type_bois2->setGeometry(QRect(20, 190, 91, 21));
        label_etat_bois = new QLabel(groupBox_13);
        label_etat_bois->setObjectName("label_etat_bois");
        label_etat_bois->setGeometry(QRect(20, 240, 171, 20));
        label_nom_bois = new QLabel(groupBox_13);
        label_nom_bois->setObjectName("label_nom_bois");
        label_nom_bois->setGeometry(QRect(21, 288, 141, 20));
        label_prix = new QLabel(groupBox_13);
        label_prix->setObjectName("label_prix");
        label_prix->setGeometry(QRect(24, 330, 63, 20));
        label_dimension = new QLabel(groupBox_13);
        label_dimension->setObjectName("label_dimension");
        label_dimension->setGeometry(QRect(25, 372, 151, 20));
        la_fournisseur_7 = new QLineEdit(groupBox_13);
        la_fournisseur_7->setObjectName("la_fournisseur_7");
        la_fournisseur_7->setGeometry(QRect(180, 50, 121, 28));
        la_emplacement_7 = new QLineEdit(groupBox_13);
        la_emplacement_7->setObjectName("la_emplacement_7");
        la_emplacement_7->setGeometry(QRect(180, 100, 121, 28));
        la_date_7 = new QDateEdit(groupBox_13);
        la_date_7->setObjectName("la_date_7");
        la_date_7->setGeometry(QRect(180, 140, 121, 29));
        la_date_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        la_type_7 = new QComboBox(groupBox_13);
        la_type_7->addItem(QString());
        la_type_7->addItem(QString());
        la_type_7->addItem(QString());
        la_type_7->addItem(QString());
        la_type_7->setObjectName("la_type_7");
        la_type_7->setGeometry(QRect(180, 190, 121, 28));
        la_type_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        la_etat_7 = new QComboBox(groupBox_13);
        la_etat_7->addItem(QString());
        la_etat_7->addItem(QString());
        la_etat_7->addItem(QString());
        la_etat_7->addItem(QString());
        la_etat_7->addItem(QString());
        la_etat_7->setObjectName("la_etat_7");
        la_etat_7->setGeometry(QRect(180, 230, 121, 28));
        la_etat_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        la_nom_7 = new QComboBox(groupBox_13);
        la_nom_7->addItem(QString());
        la_nom_7->addItem(QString());
        la_nom_7->addItem(QString());
        la_nom_7->addItem(QString());
        la_nom_7->addItem(QString());
        la_nom_7->setObjectName("la_nom_7");
        la_nom_7->setGeometry(QRect(180, 280, 121, 28));
        la_nom_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        la_prix_7 = new QDoubleSpinBox(groupBox_13);
        la_prix_7->setObjectName("la_prix_7");
        la_prix_7->setGeometry(QRect(184, 328, 121, 29));
        la_prix_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}\n"
""));
        la_l_7 = new QDoubleSpinBox(groupBox_13);
        la_l_7->setObjectName("la_l_7");
        la_l_7->setGeometry(QRect(184, 368, 88, 29));
        la_l_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}\n"
""));
        la_w_7 = new QDoubleSpinBox(groupBox_13);
        la_w_7->setObjectName("la_w_7");
        la_w_7->setGeometry(QRect(280, 370, 88, 29));
        la_w_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        la_h_7 = new QDoubleSpinBox(groupBox_13);
        la_h_7->setObjectName("la_h_7");
        la_h_7->setGeometry(QRect(184, 403, 88, 29));
        la_h_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        la_ajouter_7 = new QPushButton(groupBox_13);
        la_ajouter_7->setObjectName("la_ajouter_7");
        la_ajouter_7->setGeometry(QRect(10, 440, 361, 48));
        la_ajouter_7->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        la_modifier_7 = new QPushButton(groupBox_13);
        la_modifier_7->setObjectName("la_modifier_7");
        la_modifier_7->setGeometry(QRect(190, 497, 181, 51));
        la_modifier_7->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        la_supprimer_7 = new QPushButton(groupBox_13);
        la_supprimer_7->setObjectName("la_supprimer_7");
        la_supprimer_7->setGeometry(QRect(10, 497, 171, 51));
        la_supprimer_7->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        groupBox_10 = new QGroupBox(tab_gestion_7);
        groupBox_10->setObjectName("groupBox_10");
        groupBox_10->setGeometry(QRect(420, 0, 771, 561));
        date_fin_prev_5 = new QLabel(groupBox_10);
        date_fin_prev_5->setObjectName("date_fin_prev_5");
        date_fin_prev_5->setGeometry(QRect(20, 250, 121, 20));
        date_fin_reel_5 = new QLabel(groupBox_10);
        date_fin_reel_5->setObjectName("date_fin_reel_5");
        date_fin_reel_5->setGeometry(QRect(20, 300, 121, 20));
        rech_4 = new QLineEdit(groupBox_10);
        rech_4->setObjectName("rech_4");
        rech_4->setGeometry(QRect(130, 50, 211, 28));
        btn_rechercher_4 = new QPushButton(groupBox_10);
        btn_rechercher_4->setObjectName("btn_rechercher_4");
        btn_rechercher_4->setGeometry(QRect(360, 30, 90, 48));
        btn_rechercher_4->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        btn_trier_4 = new QPushButton(groupBox_10);
        btn_trier_4->setObjectName("btn_trier_4");
        btn_trier_4->setGeometry(QRect(230, 440, 111, 48));
        btn_trier_4->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        trie_3 = new QLabel(groupBox_10);
        trie_3->setObjectName("trie_3");
        trie_3->setGeometry(QRect(40, 450, 71, 21));
        tab_rech_2 = new QTableView(groupBox_10);
        tab_rech_2->setObjectName("tab_rech_2");
        tab_rech_2->setGeometry(QRect(30, 90, 721, 341));
        recherche_3 = new QLabel(groupBox_10);
        recherche_3->setObjectName("recherche_3");
        recherche_3->setGeometry(QRect(50, 50, 111, 21));
        la_pdf_9 = new QPushButton(groupBox_10);
        la_pdf_9->setObjectName("la_pdf_9");
        la_pdf_9->setGeometry(QRect(360, 440, 191, 48));
        la_pdf_9->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        la_tri_7 = new QComboBox(groupBox_10);
        la_tri_7->addItem(QString());
        la_tri_7->addItem(QString());
        la_tri_7->addItem(QString());
        la_tri_7->addItem(QString());
        la_tri_7->addItem(QString());
        la_tri_7->setObjectName("la_tri_7");
        la_tri_7->setGeometry(QRect(120, 450, 80, 28));
        la_tri_7->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        tabBois->addTab(tab_gestion_7, QString());
        tab_stats_7 = new QWidget();
        tab_stats_7->setObjectName("tab_stats_7");
        stat_bois = new QPushButton(tab_stats_7);
        stat_bois->setObjectName("stat_bois");
        stat_bois->setGeometry(QRect(820, 480, 151, 48));
        stat_bois->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        textEdit_bois = new QTextEdit(tab_stats_7);
        textEdit_bois->setObjectName("textEdit_bois");
        textEdit_bois->setGeometry(QRect(70, 50, 891, 411));
        tabBois->addTab(tab_stats_7, QString());
        tab_optimisatione_7 = new QWidget();
        tab_optimisatione_7->setObjectName("tab_optimisatione_7");
        groupBox_14 = new QGroupBox(tab_optimisatione_7);
        groupBox_14->setObjectName("groupBox_14");
        groupBox_14->setGeometry(QRect(30, 20, 1141, 561));
        label_long = new QLabel(groupBox_14);
        label_long->setObjectName("label_long");
        label_long->setGeometry(QRect(30, 50, 221, 20));
        la_id_14 = new QLineEdit(groupBox_14);
        la_id_14->setObjectName("la_id_14");
        la_id_14->setGeometry(QRect(260, 50, 121, 28));
        label_formedem = new QLabel(groupBox_14);
        label_formedem->setObjectName("label_formedem");
        label_formedem->setGeometry(QRect(30, 90, 131, 21));
        la_forme_7 = new QComboBox(groupBox_14);
        la_forme_7->addItem(QString());
        la_forme_7->addItem(QString());
        la_forme_7->addItem(QString());
        la_forme_7->addItem(QString());
        la_forme_7->addItem(QString());
        la_forme_7->setObjectName("la_forme_7");
        la_forme_7->setGeometry(QRect(170, 90, 121, 28));
        calcul_decoupage_7 = new QPushButton(groupBox_14);
        calcul_decoupage_7->setObjectName("calcul_decoupage_7");
        calcul_decoupage_7->setGeometry(QRect(310, 80, 210, 48));
        calcul_decoupage_7->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        la_textEdit_21 = new QTextEdit(groupBox_14);
        la_textEdit_21->setObjectName("la_textEdit_21");
        la_textEdit_21->setGeometry(QRect(30, 140, 1081, 391));
        tabBois->addTab(tab_optimisatione_7, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName("tab_7");
        la_textEdit_22 = new QTextEdit(tab_7);
        la_textEdit_22->setObjectName("la_textEdit_22");
        la_textEdit_22->setGeometry(QRect(30, 20, 1141, 381));
        la_question_7 = new QLineEdit(tab_7);
        la_question_7->setObjectName("la_question_7");
        la_question_7->setGeometry(QRect(30, 420, 971, 40));
        poser_7 = new QPushButton(tab_7);
        poser_7->setObjectName("poser_7");
        poser_7->setGeometry(QRect(1010, 420, 161, 48));
        poser_7->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        tabBois->addTab(tab_7, QString());
        stackedWidget->addWidget(page);
        page_7 = new QWidget();
        page_7->setObjectName("page_7");
        stackedWidget->addWidget(page_7);
        page_6 = new QWidget();
        page_6->setObjectName("page_6");
        tabWidget_modeles = new QTabWidget(page_6);
        tabWidget_modeles->setObjectName("tabWidget_modeles");
        tabWidget_modeles->setGeometry(QRect(10, 0, 1211, 631));
        tab_gestion_modeles = new QWidget();
        tab_gestion_modeles->setObjectName("tab_gestion_modeles");
        horizontalLayoutMain = new QHBoxLayout(tab_gestion_modeles);
        horizontalLayoutMain->setSpacing(20);
        horizontalLayoutMain->setObjectName("horizontalLayoutMain");
        horizontalLayoutMain->setContentsMargins(15, 15, 15, 15);
        groupBox_modeles = new QGroupBox(tab_gestion_modeles);
        groupBox_modeles->setObjectName("groupBox_modeles");
        groupBox_modeles->setMaximumSize(QSize(350, 16777215));
        verticalLayoutForm = new QVBoxLayout(groupBox_modeles);
        verticalLayoutForm->setObjectName("verticalLayoutForm");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setVerticalSpacing(15);
        label_id_mod = new QLabel(groupBox_modeles);
        label_id_mod->setObjectName("label_id_mod");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_id_mod);

        label_nom_mod = new QLabel(groupBox_modeles);
        label_nom_mod->setObjectName("label_nom_mod");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_nom_mod);

        le_nom_modele = new QLineEdit(groupBox_modeles);
        le_nom_modele->setObjectName("le_nom_modele");

        formLayout->setWidget(1, QFormLayout::FieldRole, le_nom_modele);

        label_Type = new QLabel(groupBox_modeles);
        label_Type->setObjectName("label_Type");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_Type);

        combo_type = new QComboBox(groupBox_modeles);
        combo_type->addItem(QString());
        combo_type->addItem(QString());
        combo_type->addItem(QString());
        combo_type->addItem(QString());
        combo_type->addItem(QString());
        combo_type->setObjectName("combo_type");
        combo_type->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));

        formLayout->setWidget(2, QFormLayout::FieldRole, combo_type);

        label_id_bois = new QLabel(groupBox_modeles);
        label_id_bois->setObjectName("label_id_bois");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_id_bois);

        le_id_bois = new QLineEdit(groupBox_modeles);
        le_id_bois->setObjectName("le_id_bois");

        formLayout->setWidget(3, QFormLayout::FieldRole, le_id_bois);

        label_longueur = new QLabel(groupBox_modeles);
        label_longueur->setObjectName("label_longueur");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_longueur);

        le_longueur = new QLineEdit(groupBox_modeles);
        le_longueur->setObjectName("le_longueur");

        formLayout->setWidget(4, QFormLayout::FieldRole, le_longueur);

        label_largeur = new QLabel(groupBox_modeles);
        label_largeur->setObjectName("label_largeur");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_largeur);

        le_largeur = new QLineEdit(groupBox_modeles);
        le_largeur->setObjectName("le_largeur");

        formLayout->setWidget(5, QFormLayout::FieldRole, le_largeur);

        label_hauteur = new QLabel(groupBox_modeles);
        label_hauteur->setObjectName("label_hauteur");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_hauteur);

        le_hauteur = new QLineEdit(groupBox_modeles);
        le_hauteur->setObjectName("le_hauteur");

        formLayout->setWidget(6, QFormLayout::FieldRole, le_hauteur);

        label_créepar = new QLabel(groupBox_modeles);
        label_créepar->setObjectName("label_cr\303\251epar");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_créepar);

        le_créepar = new QLineEdit(groupBox_modeles);
        le_créepar->setObjectName("le_cr\303\251epar");

        formLayout->setWidget(7, QFormLayout::FieldRole, le_créepar);

        label_date_mod = new QLabel(groupBox_modeles);
        label_date_mod->setObjectName("label_date_mod");

        formLayout->setWidget(8, QFormLayout::LabelRole, label_date_mod);

        de_date_creation = new QDateEdit(groupBox_modeles);
        de_date_creation->setObjectName("de_date_creation");
        de_date_creation->setStyleSheet(QString::fromUtf8("\n"
"/* LA FEN\303\212TRE DU CALENDRIER */\n"
"QCalendarWidget QWidget {\n"
"    background-color: #FAF3E0;\n"
"    alternate-background-color: #F2E8D5; /* Lignes altern\303\251es */\n"
"}\n"
"\n"
"/* FORCE LA COULEUR DES MOIS ET DES ANN\303\211ES (Ce qui n'\303\251tait pas clair) */\n"
"QCalendarWidget QMenu {\n"
"    background-color: #FAF3E0;\n"
"    color: #5D3A2E; /* Texte du menu des mois en Marron */\n"
"    selection-background-color: #8B4513;\n"
"}\n"
"\n"
"/* La barre de navigation du haut (Mois/Ann\303\251e) */\n"
"QCalendarWidget QNavigationBar {\n"
"    background-color: #5D3A2E;\n"
"}\n"
"\n"
"/* Les chiffres et les jours */\n"
"QCalendarWidget QAbstractItemView {\n"
"    color: #5D3A2E; /* Chiffres en marron */\n"
"    selection-background-color: #8B4513;\n"
"    selection-color: white;\n"
"}\n"
"\n"
"/* Les boutons Suivant/Pr\303\251c\303\251dent */\n"
"QCalendarWidget QToolButton {\n"
"    color: white;\n"
"    background-color: #5D3A2E;\n"
"    border-radius: 3px;\n"
"}"));
        de_date_creation->setCalendarPopup(true);

        formLayout->setWidget(8, QFormLayout::FieldRole, de_date_creation);


        verticalLayoutForm->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayoutForm->addItem(verticalSpacer);

        vButtonsLayout = new QVBoxLayout();
        vButtonsLayout->setObjectName("vButtonsLayout");
        btn_ajouter_modele = new QPushButton(groupBox_modeles);
        btn_ajouter_modele->setObjectName("btn_ajouter_modele");
        btn_ajouter_modele->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        vButtonsLayout->addWidget(btn_ajouter_modele);

        hButtonsAction = new QHBoxLayout();
        hButtonsAction->setObjectName("hButtonsAction");
        btn_modifier_modele = new QPushButton(groupBox_modeles);
        btn_modifier_modele->setObjectName("btn_modifier_modele");
        btn_modifier_modele->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        hButtonsAction->addWidget(btn_modifier_modele);

        btn_supprimer_modele = new QPushButton(groupBox_modeles);
        btn_supprimer_modele->setObjectName("btn_supprimer_modele");
        btn_supprimer_modele->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        hButtonsAction->addWidget(btn_supprimer_modele);


        vButtonsLayout->addLayout(hButtonsAction);


        verticalLayoutForm->addLayout(vButtonsLayout);


        horizontalLayoutMain->addWidget(groupBox_modeles);

        groupBox_tableau = new QGroupBox(tab_gestion_modeles);
        groupBox_tableau->setObjectName("groupBox_tableau");
        verticalLayoutRight = new QVBoxLayout(groupBox_tableau);
        verticalLayoutRight->setObjectName("verticalLayoutRight");
        horizontalLayoutSearch = new QHBoxLayout();
        horizontalLayoutSearch->setObjectName("horizontalLayoutSearch");
        recherche_5 = new QLabel(groupBox_tableau);
        recherche_5->setObjectName("recherche_5");

        horizontalLayoutSearch->addWidget(recherche_5);

        rech_6 = new QLineEdit(groupBox_tableau);
        rech_6->setObjectName("rech_6");

        horizontalLayoutSearch->addWidget(rech_6);

        btn_rechercher_5 = new QPushButton(groupBox_tableau);
        btn_rechercher_5->setObjectName("btn_rechercher_5");
        btn_rechercher_5->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        horizontalLayoutSearch->addWidget(btn_rechercher_5);


        verticalLayoutRight->addLayout(horizontalLayoutSearch);

        table_modeles = new QTableWidget(groupBox_tableau);
        if (table_modeles->columnCount() < 10)
            table_modeles->setColumnCount(10);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(0, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(1, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(2, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(3, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(4, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(5, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(6, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(7, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(8, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        table_modeles->setHorizontalHeaderItem(9, __qtablewidgetitem18);
        table_modeles->setObjectName("table_modeles");
        table_modeles->setAlternatingRowColors(true);
        table_modeles->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        verticalLayoutRight->addWidget(table_modeles);

        horizontalLayoutBottom = new QHBoxLayout();
        horizontalLayoutBottom->setObjectName("horizontalLayoutBottom");
        trie_5 = new QLabel(groupBox_tableau);
        trie_5->setObjectName("trie_5");

        horizontalLayoutBottom->addWidget(trie_5);

        cb_critere_recherche_modele = new QComboBox(groupBox_tableau);
        cb_critere_recherche_modele->addItem(QString());
        cb_critere_recherche_modele->addItem(QString());
        cb_critere_recherche_modele->addItem(QString());
        cb_critere_recherche_modele->setObjectName("cb_critere_recherche_modele");
        cb_critere_recherche_modele->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));

        horizontalLayoutBottom->addWidget(cb_critere_recherche_modele);

        btn_tire = new QPushButton(groupBox_tableau);
        btn_tire->setObjectName("btn_tire");
        btn_tire->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        horizontalLayoutBottom->addWidget(btn_tire);

        btn_export_pdf_modele = new QPushButton(groupBox_tableau);
        btn_export_pdf_modele->setObjectName("btn_export_pdf_modele");
        btn_export_pdf_modele->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));

        horizontalLayoutBottom->addWidget(btn_export_pdf_modele);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayoutBottom->addItem(horizontalSpacer);


        verticalLayoutRight->addLayout(horizontalLayoutBottom);


        horizontalLayoutMain->addWidget(groupBox_tableau);

        tabWidget_modeles->addTab(tab_gestion_modeles, QString());
        tab_statistiques = new QWidget();
        tab_statistiques->setObjectName("tab_statistiques");
        verticalLayoutStats = new QVBoxLayout(tab_statistiques);
        verticalLayoutStats->setObjectName("verticalLayoutStats");
        placeholder_stats = new QWidget(tab_statistiques);
        placeholder_stats->setObjectName("placeholder_stats");
        placeholder_stats->setStyleSheet(QString::fromUtf8("background-color: #FFFFFF; border: 1px solid #B8956A; border-radius: 10px;"));

        verticalLayoutStats->addWidget(placeholder_stats);

        tabWidget_modeles->addTab(tab_statistiques, QString());
        tab_tendances = new QWidget();
        tab_tendances->setObjectName("tab_tendances");
        verticalLayoutTendances = new QVBoxLayout(tab_tendances);
        verticalLayoutTendances->setObjectName("verticalLayoutTendances");
        placeholder_tendances = new QWidget(tab_tendances);
        placeholder_tendances->setObjectName("placeholder_tendances");
        placeholder_tendances->setStyleSheet(QString::fromUtf8("background-color: #FFFFFF; border: 1px solid #B8956A; border-radius: 10px;"));

        verticalLayoutTendances->addWidget(placeholder_tendances);

        tabWidget_modeles->addTab(tab_tendances, QString());
        tab_couts = new QWidget();
        tab_couts->setObjectName("tab_couts");
        verticalLayoutCouts = new QVBoxLayout(tab_couts);
        verticalLayoutCouts->setObjectName("verticalLayoutCouts");
        placeholder_couts = new QWidget(tab_couts);
        placeholder_couts->setObjectName("placeholder_couts");
        placeholder_couts->setStyleSheet(QString::fromUtf8("background-color: #FFFFFF; border: 1px solid #B8956A; border-radius: 10px;"));

        verticalLayoutCouts->addWidget(placeholder_couts);

        tabWidget_modeles->addTab(tab_couts, QString());
        stackedWidget->addWidget(page_6);
        page_5 = new QWidget();
        page_5->setObjectName("page_5");
        tabWidget_2 = new QTabWidget(page_5);
        tabWidget_2->setObjectName("tabWidget_2");
        tabWidget_2->setGeometry(QRect(10, 0, 1211, 621));
        tab_gestion_3 = new QWidget();
        tab_gestion_3->setObjectName("tab_gestion_3");
        groupBox_3 = new QGroupBox(tab_gestion_3);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(10, 10, 351, 561));
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 50, 63, 20));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(20, 90, 63, 20));
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(20, 130, 71, 20));
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(20, 170, 131, 20));
        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(20, 210, 91, 21));
        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(20, 250, 171, 20));
        label_11 = new QLabel(groupBox_3);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(20, 290, 63, 20));
        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(20, 340, 91, 20));
        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(20, 380, 141, 20));
        le_cin = new QLineEdit(groupBox_3);
        le_cin->setObjectName("le_cin");
        le_cin->setGeometry(QRect(180, 50, 121, 28));
        le_nom_2 = new QLineEdit(groupBox_3);
        le_nom_2->setObjectName("le_nom_2");
        le_nom_2->setGeometry(QRect(180, 90, 121, 28));
        le_prenom = new QLineEdit(groupBox_3);
        le_prenom->setObjectName("le_prenom");
        le_prenom->setGeometry(QRect(180, 130, 121, 28));
        de_naissance = new QDateEdit(groupBox_3);
        de_naissance->setObjectName("de_naissance");
        de_naissance->setGeometry(QRect(180, 170, 121, 29));
        de_naissance->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        cb_poste = new QComboBox(groupBox_3);
        cb_poste->addItem(QString());
        cb_poste->addItem(QString());
        cb_poste->addItem(QString());
        cb_poste->addItem(QString());
        cb_poste->addItem(QString());
        cb_poste->addItem(QString());
        cb_poste->setObjectName("cb_poste");
        cb_poste->setGeometry(QRect(180, 210, 121, 28));
        cb_poste->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        de_embauche = new QDateEdit(groupBox_3);
        de_embauche->setObjectName("de_embauche");
        de_embauche->setGeometry(QRect(180, 250, 121, 29));
        de_embauche->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}\n"
""));
        le_salaire = new QLineEdit(groupBox_3);
        le_salaire->setObjectName("le_salaire");
        le_salaire->setGeometry(QRect(180, 290, 121, 28));
        le_tel = new QLineEdit(groupBox_3);
        le_tel->setObjectName("le_tel");
        le_tel->setGeometry(QRect(180, 340, 121, 28));
        le_rfid = new QLineEdit(groupBox_3);
        le_rfid->setObjectName("le_rfid");
        le_rfid->setGeometry(QRect(180, 380, 121, 28));
        btn_ajouter_3 = new QPushButton(groupBox_3);
        btn_ajouter_3->setObjectName("btn_ajouter_3");
        btn_ajouter_3->setGeometry(QRect(20, 430, 311, 48));
        btn_ajouter_3->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        btn_modifier_2 = new QPushButton(groupBox_3);
        btn_modifier_2->setObjectName("btn_modifier_2");
        btn_modifier_2->setGeometry(QRect(19, 487, 151, 51));
        btn_modifier_2->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        btn_supprimer_2 = new QPushButton(groupBox_3);
        btn_supprimer_2->setObjectName("btn_supprimer_2");
        btn_supprimer_2->setGeometry(QRect(180, 487, 151, 51));
        btn_supprimer_2->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        groupBox_11 = new QGroupBox(tab_gestion_3);
        groupBox_11->setObjectName("groupBox_11");
        groupBox_11->setGeometry(QRect(390, 10, 781, 561));
        date_fin_prev_6 = new QLabel(groupBox_11);
        date_fin_prev_6->setObjectName("date_fin_prev_6");
        date_fin_prev_6->setGeometry(QRect(20, 250, 121, 20));
        date_fin_reel_6 = new QLabel(groupBox_11);
        date_fin_reel_6->setObjectName("date_fin_reel_6");
        date_fin_reel_6->setGeometry(QRect(20, 300, 121, 20));
        rech_5 = new QLineEdit(groupBox_11);
        rech_5->setObjectName("rech_5");
        rech_5->setGeometry(QRect(130, 50, 211, 28));
        btn_rechercher_3 = new QPushButton(groupBox_11);
        btn_rechercher_3->setObjectName("btn_rechercher_3");
        btn_rechercher_3->setGeometry(QRect(460, 40, 90, 48));
        btn_rechercher_3->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        btn_trier_3 = new QPushButton(groupBox_11);
        btn_trier_3->setObjectName("btn_trier_3");
        btn_trier_3->setGeometry(QRect(230, 440, 111, 48));
        btn_trier_3->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        trie_4 = new QLabel(groupBox_11);
        trie_4->setObjectName("trie_4");
        trie_4->setGeometry(QRect(40, 450, 71, 21));
        tab_rech_3 = new QTableView(groupBox_11);
        tab_rech_3->setObjectName("tab_rech_3");
        tab_rech_3->setGeometry(QRect(30, 90, 721, 341));
        recherche_4 = new QLabel(groupBox_11);
        recherche_4->setObjectName("recherche_4");
        recherche_4->setGeometry(QRect(50, 50, 111, 21));
        la_pdf_10 = new QPushButton(groupBox_11);
        la_pdf_10->setObjectName("la_pdf_10");
        la_pdf_10->setGeometry(QRect(360, 440, 201, 48));
        la_pdf_10->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        cb_critere_recherche = new QComboBox(groupBox_11);
        cb_critere_recherche->addItem(QString());
        cb_critere_recherche->addItem(QString());
        cb_critere_recherche->addItem(QString());
        cb_critere_recherche->setObjectName("cb_critere_recherche");
        cb_critere_recherche->setGeometry(QRect(350, 50, 82, 28));
        cb_critere_recherche->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        cb_tri = new QComboBox(groupBox_11);
        cb_tri->addItem(QString());
        cb_tri->addItem(QString());
        cb_tri->addItem(QString());
        cb_tri->setObjectName("cb_tri");
        cb_tri->setGeometry(QRect(120, 450, 82, 28));
        cb_tri->setStyleSheet(QString::fromUtf8("/* Le champ principal */\n"
"QComboBox {\n"
"\n"
"\n"
"}\n"
"\n"
"/* La liste qui descend (Opaque) */\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #FAF3E0; /* Fond cr\303\250me opaque */\n"
"    selection-background-color: #8B4513; /* Couleur de s\303\251lection */\n"
"    border: 1px solid #5D3A2E;\n"
"}"));
        tabWidget_2->addTab(tab_gestion_3, QString());
        tab_stats_3 = new QWidget();
        tab_stats_3->setObjectName("tab_stats_3");
        tabWidget_2->addTab(tab_stats_3, QString());
        tab_avance_3 = new QWidget();
        tab_avance_3->setObjectName("tab_avance_3");
        groupBox_6 = new QGroupBox(tab_avance_3);
        groupBox_6->setObjectName("groupBox_6");
        groupBox_6->setGeometry(QRect(30, 20, 1141, 181));
        label_16 = new QLabel(groupBox_6);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(30, 70, 451, 20));
        lbl_resultat_prime = new QLabel(groupBox_6);
        lbl_resultat_prime->setObjectName("lbl_resultat_prime");
        lbl_resultat_prime->setGeometry(QRect(40, 120, 200, 20));
        btn_calcul_prime = new QPushButton(groupBox_6);
        btn_calcul_prime->setObjectName("btn_calcul_prime");
        btn_calcul_prime->setGeometry(QRect(509, 130, 151, 48));
        btn_calcul_prime->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        groupBox_7 = new QGroupBox(tab_avance_3);
        groupBox_7->setObjectName("groupBox_7");
        groupBox_7->setGeometry(QRect(30, 220, 1141, 161));
        lbl_etat_pointage = new QLabel(groupBox_7);
        lbl_etat_pointage->setObjectName("lbl_etat_pointage");
        lbl_etat_pointage->setGeometry(QRect(50, 70, 191, 20));
        btn_simuler_rfid = new QPushButton(groupBox_7);
        btn_simuler_rfid->setObjectName("btn_simuler_rfid");
        btn_simuler_rfid->setGeometry(QRect(450, 65, 221, 48));
        btn_simuler_rfid->setStyleSheet(QString::fromUtf8("background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px;"));
        tabWidget_2->addTab(tab_avance_3, QString());
        stackedWidget->addWidget(page_5);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1403, 33));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(4);
        tabWidget->setCurrentIndex(1);
        tabBois->setCurrentIndex(0);
        tabWidget_modeles->setCurrentIndex(2);
        tabWidget_2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Gestion De Soci\303\251te", nullptr));
        btn_personnel->setText(QCoreApplication::translate("MainWindow", "Personnel", nullptr));
        btn_modele->setText(QCoreApplication::translate("MainWindow", "modele", nullptr));
        btn_bois->setText(QCoreApplication::translate("MainWindow", "Bois", nullptr));
        btn_etape->setText(QCoreApplication::translate("MainWindow", "Etape", nullptr));
        btn_fabrication->setText(QCoreApplication::translate("MainWindow", "fabrication", nullptr));
        label_17->setText(QString());
        groupBox_2->setTitle(QString());
        lbl_id_fabrication->setText(QCoreApplication::translate("MainWindow", "ID Fabrication :", nullptr));
        lbl_modele->setText(QCoreApplication::translate("MainWindow", "ID Mod\303\250le :", nullptr));
        lbl_date_deb->setText(QCoreApplication::translate("MainWindow", "Date De D\303\251but :", nullptr));
        lbl_quantite->setText(QCoreApplication::translate("MainWindow", "Quantit\303\251 \303\240 Produire :", nullptr));
        lbl_qualite->setText(QCoreApplication::translate("MainWindow", "Qualit\303\251 :", nullptr));
        aff_qualite->setItemText(0, QCoreApplication::translate("MainWindow", "Standard", nullptr));
        aff_qualite->setItemText(1, QCoreApplication::translate("MainWindow", "Premium", nullptr));
        aff_qualite->setItemText(2, QCoreApplication::translate("MainWindow", "Luxe", nullptr));
        aff_qualite->setItemText(3, QCoreApplication::translate("MainWindow", "Export", nullptr));

        lbl_commentaire->setText(QCoreApplication::translate("MainWindow", "Commentaire :", nullptr));
        btn_ajouter_2->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        btn_modifier->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        btn_supprimer->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        groupBox_tableau_2->setTitle(QString());
        recherche_6->setText(QCoreApplication::translate("MainWindow", "Recherche :", nullptr));
        btn_rechercher_6->setText(QCoreApplication::translate("MainWindow", "Chercher", nullptr));
        QTableWidgetItem *___qtablewidgetitem = table_modeles_2->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID Fabrication", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = table_modeles_2->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Modele", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = table_modeles_2->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Date De Debut", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = table_modeles_2->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Quantite a Produire", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = table_modeles_2->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "Qualite", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = table_modeles_2->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "  Commentaire", nullptr));
        trie_6->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        cb_critere_recherche_modele_2->setItemText(0, QCoreApplication::translate("MainWindow", "Modele", nullptr));
        cb_critere_recherche_modele_2->setItemText(1, QCoreApplication::translate("MainWindow", "ID Fabrication", nullptr));
        cb_critere_recherche_modele_2->setItemText(2, QCoreApplication::translate("MainWindow", "Date Debut", nullptr));
        cb_critere_recherche_modele_2->setItemText(3, QCoreApplication::translate("MainWindow", "Qualite", nullptr));
        cb_critere_recherche_modele_2->setItemText(4, QCoreApplication::translate("MainWindow", "Quantite a Produire", nullptr));
        cb_critere_recherche_modele_2->setItemText(5, QCoreApplication::translate("MainWindow", "Commentaire", nullptr));

        btn_tire_2->setText(QCoreApplication::translate("MainWindow", "Valider Tri", nullptr));
        btn_export_pdf_modele_2->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 Exporter Rapport PDF", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_gestion_2), QCoreApplication::translate("MainWindow", "Gestion De La Fabrication", nullptr));
        gb_catalogue_etapes->setTitle(QCoreApplication::translate("MainWindow", "Catalogue des Etapes", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = table_etapes_catalogue->horizontalHeaderItem(0);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = table_etapes_catalogue->horizontalHeaderItem(1);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "Nom de l'Etape", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = table_etapes_catalogue->horizontalHeaderItem(2);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "Description", nullptr));
        gb_fabrications_suivi->setTitle(QCoreApplication::translate("MainWindow", "Fabrications \342\200\224 cliquer pour voir les etapes", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = tree_fabrications_etapes->headerItem();
        ___qtreewidgetitem->setText(4, QCoreApplication::translate("MainWindow", "Duree (j)", nullptr));
        ___qtreewidgetitem->setText(3, QCoreApplication::translate("MainWindow", "Date", nullptr));
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "Statut", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "Modele / Nom", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "ID / Etape", nullptr));
        gb_timeline_visuelle->setTitle(QCoreApplication::translate("MainWindow", "Timeline de la fabrication selectionnee", nullptr));
        lbl_leg_title->setText(QCoreApplication::translate("MainWindow", "Legende :", nullptr));
        lbl_leg_v->setText(QCoreApplication::translate("MainWindow", "   Planifie", nullptr));
        lbl_leg_o->setText(QCoreApplication::translate("MainWindow", "   En cours", nullptr));
        lbl_leg_r->setText(QCoreApplication::translate("MainWindow", "   Non planifie", nullptr));
#if QT_CONFIG(tooltip)
        btn_optimizer_ai->setToolTip(QCoreApplication::translate("MainWindow", "Optimiser automatiquement le planning de fabrication avec l'IA", nullptr));
#endif // QT_CONFIG(tooltip)
        btn_optimizer_ai->setText(QCoreApplication::translate("MainWindow", "\360\237\244\226 Optimiser AI", nullptr));
        te_ai_result->setPlaceholderText(QCoreApplication::translate("MainWindow", "Le r\303\251sultat de l'optimisation AI appara\303\256tra ici...", nullptr));
        lbl_timeline_vide->setText(QCoreApplication::translate("MainWindow", "Selectionnez une fabrication dans le tableau ci-dessus pour afficher sa timeline", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_stats_2), QCoreApplication::translate("MainWindow", "Suivi", nullptr));
        groupBox_13->setTitle(QString());
        label_fourn->setText(QCoreApplication::translate("MainWindow", "FOURNISSEUR", nullptr));
        label_empl->setText(QCoreApplication::translate("MainWindow", "EMPLACEMENT", nullptr));
        label_date_bois->setText(QCoreApplication::translate("MainWindow", "DATE :", nullptr));
        label_type_bois2->setText(QCoreApplication::translate("MainWindow", "TYPE :", nullptr));
        label_etat_bois->setText(QCoreApplication::translate("MainWindow", "ETAT :", nullptr));
        label_nom_bois->setText(QCoreApplication::translate("MainWindow", "NOM :", nullptr));
        label_prix->setText(QCoreApplication::translate("MainWindow", "PRIX :", nullptr));
        label_dimension->setText(QCoreApplication::translate("MainWindow", "DIMENSIONS (l/w/h) :", nullptr));
        la_type_7->setItemText(0, QCoreApplication::translate("MainWindow", "R\303\251sineux", nullptr));
        la_type_7->setItemText(1, QCoreApplication::translate("MainWindow", "Feuillu", nullptr));
        la_type_7->setItemText(2, QCoreApplication::translate("MainWindow", "Exotique", nullptr));
        la_type_7->setItemText(3, QCoreApplication::translate("MainWindow", "Transform\303\251", nullptr));

        la_etat_7->setItemText(0, QCoreApplication::translate("MainWindow", "Brut", nullptr));
        la_etat_7->setItemText(1, QCoreApplication::translate("MainWindow", "KD", nullptr));
        la_etat_7->setItemText(2, QCoreApplication::translate("MainWindow", "Rabot\303\251", nullptr));
        la_etat_7->setItemText(3, QCoreApplication::translate("MainWindow", "Trait\303\251", nullptr));
        la_etat_7->setItemText(4, QCoreApplication::translate("MainWindow", "Chute", nullptr));

        la_nom_7->setItemText(0, QCoreApplication::translate("MainWindow", "Sapin", nullptr));
        la_nom_7->setItemText(1, QCoreApplication::translate("MainWindow", "Pin", nullptr));
        la_nom_7->setItemText(2, QCoreApplication::translate("MainWindow", "H\303\252tre", nullptr));
        la_nom_7->setItemText(3, QCoreApplication::translate("MainWindow", "Ch\303\252ne", nullptr));
        la_nom_7->setItemText(4, QCoreApplication::translate("MainWindow", "Noyer", nullptr));

        la_ajouter_7->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        la_modifier_7->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        la_supprimer_7->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        groupBox_10->setTitle(QString());
        date_fin_prev_5->setText(QString());
        date_fin_reel_5->setText(QString());
        btn_rechercher_4->setText(QCoreApplication::translate("MainWindow", "Chercher", nullptr));
        btn_trier_4->setText(QCoreApplication::translate("MainWindow", "Valider Tri", nullptr));
        trie_3->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        recherche_3->setText(QCoreApplication::translate("MainWindow", "Recherche :", nullptr));
        la_pdf_9->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 Exporter Rapport PDF", nullptr));
        la_tri_7->setItemText(0, QCoreApplication::translate("MainWindow", "Nom", nullptr));
        la_tri_7->setItemText(1, QCoreApplication::translate("MainWindow", "Date", nullptr));
        la_tri_7->setItemText(2, QCoreApplication::translate("MainWindow", "Type", nullptr));
        la_tri_7->setItemText(3, QCoreApplication::translate("MainWindow", "Etat", nullptr));
        la_tri_7->setItemText(4, QCoreApplication::translate("MainWindow", "Prix", nullptr));

        tabBois->setTabText(tabBois->indexOf(tab_gestion_7), QCoreApplication::translate("MainWindow", "Gestion Bois", nullptr));
        stat_bois->setText(QCoreApplication::translate("MainWindow", "Exporter Stats", nullptr));
        tabBois->setTabText(tabBois->indexOf(tab_stats_7), QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        groupBox_14->setTitle(QCoreApplication::translate("MainWindow", "Optimisation de d\303\251coupage", nullptr));
        label_long->setText(QCoreApplication::translate("MainWindow", "ID de la planche \303\240 d\303\251couper :", nullptr));
        label_formedem->setText(QCoreApplication::translate("MainWindow", "Forme demand\303\251e :", nullptr));
        la_forme_7->setItemText(0, QCoreApplication::translate("MainWindow", "Pieds", nullptr));
        la_forme_7->setItemText(1, QCoreApplication::translate("MainWindow", "Traverses", nullptr));
        la_forme_7->setItemText(2, QCoreApplication::translate("MainWindow", "Socle", nullptr));
        la_forme_7->setItemText(3, QCoreApplication::translate("MainWindow", "Plateau", nullptr));
        la_forme_7->setItemText(4, QCoreApplication::translate("MainWindow", "Fa\303\247ade", nullptr));

        calcul_decoupage_7->setText(QCoreApplication::translate("MainWindow", "Calculer le d\303\251coupage", nullptr));
        tabBois->setTabText(tabBois->indexOf(tab_optimisatione_7), QCoreApplication::translate("MainWindow", "Optimisation", nullptr));
        poser_7->setText(QCoreApplication::translate("MainWindow", "Poser une question", nullptr));
        tabBois->setTabText(tabBois->indexOf(tab_7), QCoreApplication::translate("MainWindow", "Assistant", nullptr));
        groupBox_modeles->setTitle(QCoreApplication::translate("MainWindow", "D\303\251tails du Mod\303\250le", nullptr));
        label_id_mod->setText(QString());
        label_nom_mod->setText(QCoreApplication::translate("MainWindow", "Nom :", nullptr));
        label_Type->setText(QCoreApplication::translate("MainWindow", "Type :", nullptr));
        combo_type->setItemText(0, QCoreApplication::translate("MainWindow", "Table", nullptr));
        combo_type->setItemText(1, QCoreApplication::translate("MainWindow", "Chaise", nullptr));
        combo_type->setItemText(2, QCoreApplication::translate("MainWindow", "Bureau", nullptr));
        combo_type->setItemText(3, QCoreApplication::translate("MainWindow", "Armoire", nullptr));
        combo_type->setItemText(4, QCoreApplication::translate("MainWindow", "Biblioth\303\250que", nullptr));

        label_id_bois->setText(QCoreApplication::translate("MainWindow", "ID Bois :", nullptr));
        label_longueur->setText(QCoreApplication::translate("MainWindow", "Longueur (cm) :", nullptr));
        label_largeur->setText(QCoreApplication::translate("MainWindow", "Largeur (cm) :", nullptr));
        label_hauteur->setText(QCoreApplication::translate("MainWindow", "Hauteur (cm) :", nullptr));
        label_créepar->setText(QCoreApplication::translate("MainWindow", "Cr\303\251e par :", nullptr));
        label_date_mod->setText(QCoreApplication::translate("MainWindow", "Date Cr\303\251ation :", nullptr));
        btn_ajouter_modele->setText(QCoreApplication::translate("MainWindow", "Ajouter au Stock", nullptr));
        btn_modifier_modele->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        btn_supprimer_modele->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        groupBox_tableau->setTitle(QString());
        recherche_5->setText(QCoreApplication::translate("MainWindow", "Recherche :", nullptr));
        btn_rechercher_5->setText(QCoreApplication::translate("MainWindow", "Chercher", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = table_modeles->horizontalHeaderItem(0);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = table_modeles->horizontalHeaderItem(1);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = table_modeles->horizontalHeaderItem(2);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = table_modeles->horizontalHeaderItem(3);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "Type de Bois", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = table_modeles->horizontalHeaderItem(4);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "ID Bois", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = table_modeles->horizontalHeaderItem(5);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "Longueur (cm)", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = table_modeles->horizontalHeaderItem(6);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "Largeur (cm)", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = table_modeles->horizontalHeaderItem(7);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "Hauteur (cm)", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = table_modeles->horizontalHeaderItem(8);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "Cr\303\251e Par", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = table_modeles->horizontalHeaderItem(9);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        trie_5->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        cb_critere_recherche_modele->setItemText(0, QCoreApplication::translate("MainWindow", "Nom", nullptr));
        cb_critere_recherche_modele->setItemText(1, QCoreApplication::translate("MainWindow", "type", nullptr));
        cb_critere_recherche_modele->setItemText(2, QCoreApplication::translate("MainWindow", "ID Bois", nullptr));

        btn_tire->setText(QCoreApplication::translate("MainWindow", "Valider Tri", nullptr));
        btn_export_pdf_modele->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 Exporter Rapport PDF", nullptr));
        tabWidget_modeles->setTabText(tabWidget_modeles->indexOf(tab_gestion_modeles), QCoreApplication::translate("MainWindow", "\360\237\223\246 Gestion Mod\303\250les", nullptr));
        tabWidget_modeles->setTabText(tabWidget_modeles->indexOf(tab_statistiques), QCoreApplication::translate("MainWindow", "\360\237\223\212 Statistiques", nullptr));
        tabWidget_modeles->setTabText(tabWidget_modeles->indexOf(tab_tendances), QCoreApplication::translate("MainWindow", "\360\237\223\210 Tendances", nullptr));
        tabWidget_modeles->setTabText(tabWidget_modeles->indexOf(tab_couts), QCoreApplication::translate("MainWindow", "\360\237\222\260 Analyse Co\303\273ts", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Informations Employ\303\251", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "CIN :", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "NOM :", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "PRENOM :", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "DATE NAISSANCE : :", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "POSTE :", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "DATE D'EMBAUCHE :", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "SALAIRE :", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "TEL :", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "UID Badge / RFID :", nullptr));
        cb_poste->setItemText(0, QCoreApplication::translate("MainWindow", "Poste...", nullptr));
        cb_poste->setItemText(1, QCoreApplication::translate("MainWindow", "Personnel", nullptr));
        cb_poste->setItemText(2, QCoreApplication::translate("MainWindow", "Designer", nullptr));
        cb_poste->setItemText(3, QCoreApplication::translate("MainWindow", "Menuisier", nullptr));
        cb_poste->setItemText(4, QCoreApplication::translate("MainWindow", "Vernisseur", nullptr));
        cb_poste->setItemText(5, QCoreApplication::translate("MainWindow", "Livreur", nullptr));

        btn_ajouter_3->setText(QCoreApplication::translate("MainWindow", "Ajouter ", nullptr));
        btn_modifier_2->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        btn_supprimer_2->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        groupBox_11->setTitle(QString());
        date_fin_prev_6->setText(QString());
        date_fin_reel_6->setText(QString());
        btn_rechercher_3->setText(QCoreApplication::translate("MainWindow", "Chercher", nullptr));
        btn_trier_3->setText(QCoreApplication::translate("MainWindow", "Valider Tri", nullptr));
        trie_4->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        recherche_4->setText(QCoreApplication::translate("MainWindow", "Recherche :", nullptr));
        la_pdf_10->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 Exporter Rapport PDF", nullptr));
        cb_critere_recherche->setItemText(0, QCoreApplication::translate("MainWindow", "CIN", nullptr));
        cb_critere_recherche->setItemText(1, QCoreApplication::translate("MainWindow", "Nom", nullptr));
        cb_critere_recherche->setItemText(2, QCoreApplication::translate("MainWindow", "Poste", nullptr));

        cb_tri->setItemText(0, QCoreApplication::translate("MainWindow", "Nom", nullptr));
        cb_tri->setItemText(1, QCoreApplication::translate("MainWindow", "Salaire", nullptr));
        cb_tri->setItemText(2, QCoreApplication::translate("MainWindow", "Date Embauche", nullptr));

        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_gestion_3), QCoreApplication::translate("MainWindow", "Gestion Personnel", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_stats_3), QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "Calculateur de Primes", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "S\303\251lectionnez un employ\303\251 pour calculer sa prime selon l'anciennet\303\251", nullptr));
        lbl_resultat_prime->setText(QCoreApplication::translate("MainWindow", "0.00 DT", nullptr));
        btn_calcul_prime->setText(QCoreApplication::translate("MainWindow", "Calculer la Prime", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "Pointage Intelligent", nullptr));
        lbl_etat_pointage->setText(QCoreApplication::translate("MainWindow", "En attente de pointage...", nullptr));
        btn_simuler_rfid->setText(QCoreApplication::translate("MainWindow", "Simuler Entr\303\251e Badge RFID", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_avance_3), QCoreApplication::translate("MainWindow", "Avanc\303\251 / IoT", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
