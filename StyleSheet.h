#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>

class StyleSheet {
public:
    static QString getStyleSheet() {
        return R"(
            /* Main Window Background - Beige clair bois */
            QMainWindow {
                background-color: #F5EFE7;
            }
            
            /* Tab Widget Styling - Tons de bois */
            QTabWidget::pane {
                border: 2px solid #8B6F47;
                background-color: #FAF6F0;
                border-radius: 8px;
                top: -2px;
            }
            
            QTabBar::tab {
                background-color: #6B5644;
                color: white;
                padding: 10px 20px;
                margin-right: 2px;
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                font-weight: bold;
                min-width: 150px;
            }
            
            QTabBar::tab:selected {
                background-color: #A0826D;
            }
            
            QTabBar::tab:hover:!selected {
                background-color: #8B6F47;
            }
            
            /* Group Box Styling - Panneau bois */
            QGroupBox {
                background-color: rgba(210, 180, 140, 0.2);
                border: 2px solid #A0826D;
                border-radius: 10px;
                margin-top: 10px;
                padding-top: 15px;
                font-weight: bold;
                color: #4A3C2A;
            }
            
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 5px 10px;
                color: #4A3C2A;
            }
            
            /* Table Widget Styling */
            QTableWidget {
                background-color: rgba(255, 255, 255, 0.9);
                alternate-background-color: rgba(222, 184, 135, 0.15);
                border: 2px solid #A0826D;
                border-radius: 8px;
                gridline-color: #D2B48C;
                selection-background-color: #A0826D;
                selection-color: white;
            }
            
            QTableWidget::item {
                padding: 5px;
                color: #3D2E1F;
            }
            
            QHeaderView::section {
                background-color: #8B6F47;
                color: white;
                padding: 8px;
                border: 1px solid #6B5644;
                font-weight: bold;
            }
            
            /* Line Edit Styling */
            QLineEdit, QTextEdit, QDateEdit {
                background-color: #FFFEF9;
                border: 2px solid #C4A57B;
                border-radius: 6px;
                padding: 8px;
                font-size: 11pt;
                color: #3D2E1F;
            }
            
            QLineEdit:focus, QTextEdit:focus, QDateEdit:focus {
                border: 2px solid #8B6F47;
                background-color: #FFFFFF;
            }
            
            /* Combo Box Styling */
            QComboBox {
                background-color: #FFFEF9;
                border: 2px solid #C4A57B;
                border-radius: 6px;
                padding: 8px;
                font-size: 11pt;
                color: #3D2E1F;
            }
            
            QComboBox:focus {
                border: 2px solid #8B6F47;
            }
            
            QComboBox::drop-down {
                border: none;
                width: 30px;
            }
            
            QComboBox::down-arrow {
                image: none;
                border-left: 5px solid transparent;
                border-right: 5px solid transparent;
                border-top: 5px solid #4A3C2A;
                margin-right: 10px;
            }
            
            QComboBox QAbstractItemView {
                background-color: #FFFEF9;
                selection-background-color: #D2B48C;
                selection-color: #3D2E1F;
                border: 2px solid #C4A57B;
            }
            
            /* Button Styling - Bois foncé */
            QPushButton {
                background-color: #6B5644;
                color: white;
                border: none;
                border-radius: 8px;
                padding: 10px 20px;
                font-weight: bold;
                font-size: 10pt;
                min-width: 100px;
            }
            
            QPushButton:hover {
                background-color: #8B6F47;
            }
            
            QPushButton:pressed {
                background-color: #5A4634;
            }
            
            QPushButton:disabled {
                background-color: #D2B48C;
                color: #8B7355;
            }
            
            /* Special Buttons - Différents tons de bois */
            QPushButton#addButton {
                background-color: #8B7355;
            }
            
            QPushButton#addButton:hover {
                background-color: #A0826D;
            }
            
            QPushButton#updateButton {
                background-color: #A0826D;
            }
            
            QPushButton#updateButton:hover {
                background-color: #B8956A;
            }
            
            QPushButton#deleteButton {
                background-color: #8B4513;
            }
            
            QPushButton#deleteButton:hover {
                background-color: #A0522D;
            }
            
            QPushButton#searchButton {
                background-color: #6B5644;
            }
            
            QPushButton#searchButton:hover {
                background-color: #8B6F47;
            }
            
            /* Labels */
            QLabel {
                color: #4A3C2A;
                font-size: 10pt;
            }
            
            /* Status Bar */
            QStatusBar {
                background-color: #E8DCC8;
                color: #4A3C2A;
                border-top: 2px solid #C4A57B;
            }
            
            /* Scroll Bars */
            QScrollBar:vertical {
                background-color: #F5EFE7;
                width: 12px;
                border-radius: 6px;
            }
            
            QScrollBar::handle:vertical {
                background-color: #A0826D;
                border-radius: 6px;
                min-height: 20px;
            }
            
            QScrollBar::handle:vertical:hover {
                background-color: #8B6F47;
            }
            
            QScrollBar:horizontal {
                background-color: #F5EFE7;
                height: 12px;
                border-radius: 6px;
            }
            
            QScrollBar::handle:horizontal {
                background-color: #A0826D;
                border-radius: 6px;
                min-width: 20px;
            }
            
            QScrollBar::handle:horizontal:hover {
                background-color: #8B6F47;
            }
        )";
    }
};

#endif // STYLESHEET_H
