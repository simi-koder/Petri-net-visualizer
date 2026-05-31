/**
 * @file help.cpp
 * @author Šimon Čorej (xcorejs00)
 * @brief Implementation of help dock widget
 */
#include "help.h"
#include "globals.h"

void show_help(QDockWidget * dock_right, QWidget * sidebar_widget, QVBoxLayout * layout, bool visible){

    if (!visible){
        cout << "Not visible creating help widget..." << endl;

        // ------------- Cleanup -------------
        QLayoutItem * item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()){
                delete item->widget();
            }
            else if (item->layout()){
                QLayout * nested = item->layout();
                QLayoutItem * nested_item;
                while ((nested_item = nested->takeAt(0)) != nullptr) {
                    delete nested_item->widget();
                    delete nested_item;
                }
                delete nested;
            }
        }
        // -----------------------------------
        QString html =  "<style>"
                        "body { font-size: 12pt; line-height: 1.3; font-family: Arial, sans-serif; }"
                        "h2 { font-size: 20pt; margin-bottom: 10px; }"
                        "li { margin-bottom: 3px; }"
                        "</style>"
                        "<body>"
                        "<div align='center'><h2>Help</h2></div>"
                        "<ul>"
                        "<li>Select objects and drag-n-drop them on the board.</li>"
                        "<li>Add transitions via left-click.</li>"
                        "<li>Delete objects by toggling delete button and right-clicking on objects.</li>"
                        "<li>View object list by clicking view option in tool bar.</li>"
                        "<li>Add tokens and other properties as needed.</li>"
                        "<li>Edit configuration by adding name, comment, etc... in edit option in tool bar.</li>"
                        "<li>To save configuration, choose edit option in tool bar and save configuration.</li>"
                        "<li>To load configuration, choose file option in tool bar and open file.</li>"
                        "<li>To run configuration, choose file option in tool bar and run.</li>"
                        "</ul>"
                        "</body>";
        QTextEdit * text_edit = new QTextEdit();
        text_edit->setReadOnly(true);
        text_edit->setHtml(html);

        layout->addWidget(text_edit);
        dock_right->setWidget(sidebar_widget);
        dock_right->show();
    }
    else {
        cout << "Visible deleting help widget..." << endl;
        dock_right->hide();
    }
}