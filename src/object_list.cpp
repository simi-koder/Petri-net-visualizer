/**
 * @file object_list.cpp
 * @author Šimon Čorej (xcorejs00)
 * @brief Implementation of show_list() function
 */
#include "object_list.h"
#include <QDebug>
#include "globals.h"
#include <QUdpSocket>
extern QUdpSocket * gui_socket;


void show_list(QWidget * dock_left, QVBoxLayout * layout_left, drop_widget * canvas, bool visible){
    if (!visible){
        cout << "View action triggered..." << endl;
        // ------------- Cleanup -------------
        QLayoutItem * item;
        while ((item = layout_left->takeAt(0)) != nullptr) {
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

        layout_left->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

        // ------------- HEADER ---------------
        QHBoxLayout * row = new QHBoxLayout();

        QLabel * header = new QLabel();
        header->setText("<h2>OBJECT LIST</h2>");
        header->setStyleSheet(
            "min-width: 130px;"
            "max-width: 160px;"
            "max-height: 40px;"
            "border: 2px solid black;"
        );
        header->setAlignment(Qt::AlignCenter);
        row->addWidget(header);
        // -----------------------------------

        // --------- REFRESH BUTTON ---------- 
        QPushButton * refresh_button = new QPushButton("↻");
        refresh_button->setStyleSheet(
            "background-color: gray;"
            "min-width: 40px;"
            "max-width: 40px;"
            "font-size: 20px;"
        );
        row->addWidget(refresh_button, Qt::AlignRight);
        layout_left->addLayout(row);
        layout_left->addSpacing(10);

        QObject::connect(refresh_button, &QPushButton::clicked, [dock_left, layout_left, canvas, visible](bool checked){
            show_list(dock_left, layout_left, canvas, visible);
            return;
        });
        // -----------------------------------

        // -------------- CELLS --------------
        QLabel * cell_header = new QLabel();
        cell_header->setText("<b>CELLS:</b>");
        cell_header->setStyleSheet(
            "background-color: gray;"
            "min-width: 200px;"
            "font-size: 20px;"
            "max-height: 30px;"

        );
        cell_header->setAlignment(Qt::AlignCenter);
        layout_left->addWidget(cell_header);
        // -----------------------------------

        // ------------ CELL LIST ------------
        QComboBox * combo_cell = new QComboBox();
        for (size_t i = 0; i < cell_list.size(); i++){
            QString new_text = cell_list[i].label_ptr->text();
            new_text.remove(QRegularExpression("<[^>]*>"));
            QStringList new_text_split = new_text.split(":");
            combo_cell->addItem(new_text_split[0] 
                                + " : " 
                                + (new_text_split.size() > 1 ? new_text_split[1] : "")
                                , QVariant::fromValue(&cell_list[i]));
        }
        layout_left->addWidget(combo_cell);
        layout_left->addSpacing(10);
        // -----------------------------------

        // --------- ENTRY AND ADD -----------
        QHBoxLayout * entry_token_add = new QHBoxLayout();
        QLineEdit * entry_cell = new QLineEdit();
        entry_cell->setPlaceholderText("Number of tokens");
        entry_cell->setStyleSheet(
            "min-width: 140px;"
            "max-width: 160px;"
            "min-height: 40px;"
            "font-size: 14px;"
            "border: 2px solid black;"
        );

        QPushButton * add_token = new QPushButton("Add");
        add_token->setStyleSheet(
            "background-color: gray;"
            "min-width: 40px;"
            "max-width: 40px;"
            "min-height: 40px;"
            "font-size: 15px;"
        );
        entry_token_add->addWidget(entry_cell);
        entry_token_add->addWidget(add_token);
        layout_left->addLayout(entry_token_add);
        layout_left->addSpacing(20);
        // -----------------------------------

        // ----------- TRANSITIONS -----------
        QLabel * transition_header = new QLabel();
        transition_header->setText("<b>TRANSITIONS:</b>");
        transition_header->setStyleSheet(
            "background-color: gray;"
            "min-width: 200px;"
            "max-height: 30px;"
            "font-size: 20px;"

        );
        transition_header->setAlignment(Qt::AlignCenter);
        layout_left->addWidget(transition_header);
        // -----------------------------------

        // --------- TRANS LABEL LIST --------
        QComboBox * combo_trans = new QComboBox();
        for (size_t i = 0; i < trans_list.size(); i++){
            QString new_text = trans_list[i].label_ptr->text();
            new_text.remove(QRegularExpression("<[^>]*>"));
            combo_trans->addItem(new_text + ", delay: " + QString::number(trans_list[i].properties->delay_ms), QVariant::fromValue(&trans_list[i]));
        }
        layout_left->addWidget(combo_trans);
        layout_left->addSpacing(10);
        // -----------------------------------

        // ----- ENTRY TRANSITION LABEL ------
        QHBoxLayout * entry_trans_layout = new QHBoxLayout();
        QLineEdit * entry_trans = new QLineEdit();
        entry_trans->setPlaceholderText("Delay");
        entry_trans->setStyleSheet(
            "min-width: 140px;"
            "max-width: 160px;"
            "min-height: 40px;"
            "font-size: 14px;"
            "border: 2px solid black;"
        );

        QPushButton * add_property_trans = new QPushButton("Add");
        add_property_trans->setStyleSheet(
            "background-color: gray;"
            "min-width: 40px;"
            "max-width: 40px;"
            "min-height: 40px;"
            "font-size: 15px;"
        );
        entry_trans_layout->addWidget(entry_trans);
        entry_trans_layout->addWidget(add_property_trans);
        layout_left->addLayout(entry_trans_layout);
        // -----------------------------------

        // -------- TRANS EVENT NAME ---------
        QHBoxLayout * entry_trans_event_layout = new QHBoxLayout();
        QLineEdit * entry_trans_event = new QLineEdit();
        entry_trans_event->setPlaceholderText("Event name");
        entry_trans_event->setStyleSheet(
            "min-width: 140px;"
            "max-width: 160px;"
            "min-height: 40px;"
            "font-size: 14px;"
            "border: 2px solid black;"
        );

        QPushButton * add_event_trans = new QPushButton("Add");
        add_event_trans->setStyleSheet(
            "background-color: gray;"
            "min-width: 40px;"
            "max-width: 40px;"
            "min-height: 40px;"
            "font-size: 15px;"
        );
        entry_trans_event_layout->addWidget(entry_trans_event);
        entry_trans_event_layout->addWidget(add_event_trans);
        layout_left->addLayout(entry_trans_event_layout);
        // -----------------------------------

        // -------- TRANSITION GUARD ---------
        QTextEdit * entry_trans_guard = new QTextEdit();
        entry_trans_guard->setPlaceholderText("Guard code");
        entry_trans_guard->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "min-height: 50px;"
            "max-height: 100px;"
            "border: 2px solid black;"
        );
        layout_left->addWidget(entry_trans_guard);

        QPushButton * add_guard_trans = new QPushButton("Add guard code");
        add_guard_trans->setStyleSheet(
            "background-color: gray;"
            "min-width: 200px;"
            "max-width: 200px;"
            "max-height: 40px;"
            "font-size: 15px;"
        );
        layout_left->addWidget(add_guard_trans);
        // -----------------------------------

        // -------- TRANS ACTION CODE ---------
        QTextEdit * entry_action_trans = new QTextEdit();
        entry_action_trans->setPlaceholderText("Action code");
        entry_action_trans->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "min-height: 50px;"
            "max-height: 100px;"
            "border: 2px solid black;"
        );
        layout_left->addWidget(entry_action_trans);

        QPushButton * add_action_trans = new QPushButton("Add action");
        add_action_trans->setStyleSheet(
            "background-color: gray;"
            "min-width: 200px;"
            "max-width: 200px;"
            "min-height: 30px;"
            "font-size: 15px;"
        );
        layout_left->addWidget(add_action_trans);
        layout_left->addSpacing(10);
        // -----------------------------------

        // --------- ARC LIST HEADER ---------
        QLabel * arc_header = new QLabel();
        arc_header->setText("<b>ARCS:</b>");
        arc_header->setStyleSheet(
            "background-color: gray;"
            "min-width: 200px;"
            "font-size: 20px;"
            "max-height: 30px;"

        );
        arc_header->setAlignment(Qt::AlignCenter);
        layout_left->addWidget(arc_header);
        // -----------------------------------

        // ------------- ARC LIST ------------
        QComboBox * combo_arcs = new QComboBox();

        for (size_t i = 0; i < trans_cell_list.size(); i++){
            QString cell_text = trans_cell_list[i].cell_label->text().remove(QRegularExpression("<[^>]*>"));
            QString trans_text = "T - " + trans_cell_list[i].trans_label->text().remove(QRegularExpression("<[^>]*>"));

            if (trans_cell_list[i].to_from){
                combo_arcs->addItem(cell_text + " --> " 
                                    + trans_text 
                                    + ", weight: " 
                                    + QString::number(trans_cell_list[i].weight)
                                    , QVariant::fromValue(&(trans_cell_list[i])));
            }
        }
        for (size_t i = 0; i < trans_cell_list.size(); i++){
            QString cell_text = trans_cell_list[i].cell_label->text().remove(QRegularExpression("<[^>]*>"));
            QString trans_text = "T - " + trans_cell_list[i].trans_label->text().remove(QRegularExpression("<[^>]*>"));

            if (!trans_cell_list[i].to_from){
                combo_arcs->addItem(trans_text + " <-- " 
                                    + cell_text 
                                    + ", weight: " 
                                    + QString::number(trans_cell_list[i].weight)
                                    , QVariant::fromValue(&(trans_cell_list[i])));
            }
        }
        layout_left->addWidget(combo_arcs);
        layout_left->addSpacing(10);
        // -----------------------------------

        // -------- ADD ARC PROPERTY ---------
        QHBoxLayout * entry_arc_layout = new QHBoxLayout();
        QLineEdit * entry_arc = new QLineEdit();
        entry_arc->setPlaceholderText("Add weight");
        entry_arc->setStyleSheet(
            "min-width: 140px;"
            "max-width: 160px;"
            "min-height: 40px;"
            "font-size: 14px;"
            "border: 2px solid black;"
        );

        QPushButton * add_property_arc = new QPushButton("Add");
        add_property_arc->setStyleSheet(
            "background-color: gray;"
            "min-width: 40px;"
            "max-width: 40px;"
            "min-height: 40px;"
            "font-size: 15px;"
        );
        entry_arc_layout->addWidget(entry_arc);
        entry_arc_layout->addWidget(add_property_arc);
        layout_left->addLayout(entry_arc_layout);
        layout_left->addSpacing(10);
        // -----------------------------------

        // --------- ADD TOKEN ACTION --------
        QObject::connect(add_token, &QPushButton::clicked, [entry_cell, combo_cell](bool checked){
            if (combo_cell->currentIndex() != -1){
                CellStruct * curr_cell = combo_cell->itemData(combo_cell->currentIndex()).value<CellStruct *>();
                
                int new_tokens = entry_cell->text().toInt();
                curr_cell->tokens = new_tokens;

                curr_cell->label_ptr->setText(
                    curr_cell->label_text + "<b>:<br>" + QString::number(new_tokens) + "</b>"
                );

                QStringList new_text = combo_cell->currentText().split(":");
                combo_cell->setItemText(combo_cell->currentIndex(),
                                        new_text[0] + ": "
                                        + QString::number(new_tokens)
                                    );

                // Inject token update
                if (gui_socket != nullptr) {
                    QString gui_name = curr_cell->label_text; 
                    gui_name.remove(QRegularExpression("<[^>]*>"));
                    int colon_index = gui_name.indexOf(':');
                    if (colon_index != -1) gui_name = gui_name.left(colon_index);
                    gui_name = gui_name.replace(" - ", "_").replace("-", "_").replace(" ", "_").trimmed();

                    QString payload = "INJECT:" + gui_name + ":" + QString::number(new_tokens);
                    gui_socket->writeDatagram(payload.toUtf8(), QHostAddress::LocalHost, 8080);
                    
                    qDebug() << "Injected into engine:" << payload;
                }
                entry_cell->clear();
            }
            entry_cell->clear();
        });
        // -----------------------------------

        // --------- ADD TRANS ACTION --------
        QObject::connect(add_property_trans, &QPushButton::clicked, [entry_trans, combo_trans](){
            if (combo_trans->currentIndex() != -1){
                combo_trans->itemData(combo_trans->currentIndex()).value<TransLabelStruct *>()->properties->delay_ms = entry_trans->text().toInt();
                QStringList new_text = combo_trans->currentText().split(":");
                combo_trans->setItemText(combo_trans->currentIndex(),
                                        new_text[0] + ": "
                                        + QString::number(combo_trans->itemData(combo_trans->currentIndex()).value<TransLabelStruct *>()->properties->delay_ms)
                                    );
                entry_trans->clear();
            }
            entry_trans->clear();
        });

        QObject::connect(add_event_trans, &QPushButton::clicked, [entry_trans_event, combo_trans](){
            if (combo_trans->currentIndex() != -1){
                combo_trans->itemData(combo_trans->currentIndex()).value<TransLabelStruct *>()->properties->event_name = entry_trans_event->text();
                entry_trans_event->clear();
            }
            entry_trans_event->clear();
        });

        QObject::connect(add_guard_trans, &QPushButton::clicked, [entry_trans_guard, combo_trans](){
            if (combo_trans->currentIndex() != -1){
                combo_trans->itemData(combo_trans->currentIndex()).value<TransLabelStruct *>()->properties->guard_code = entry_trans_guard->toPlainText();
                entry_trans_guard->clear();
            }
            entry_trans_guard->clear();
        });

        QObject::connect(add_action_trans, &QPushButton::clicked, [entry_action_trans, combo_trans](){
            if (combo_trans->currentIndex() != -1){
                combo_trans->itemData(combo_trans->currentIndex()).value<TransLabelStruct *>()->properties->action_code = entry_action_trans->toPlainText();
                entry_action_trans->clear();
            }
            entry_action_trans->clear();
        });
        // -----------------------------------

        // -------- ADD WEIGHT ACTION --------
        QObject::connect(add_property_arc, &QPushButton::clicked, [entry_arc, combo_arcs, dock_left, layout_left, canvas, visible](bool checked){
            if (combo_arcs->currentIndex() != -1){
                TransitionStruct * curr_arc = combo_arcs->itemData(combo_arcs->currentIndex()).value<TransitionStruct *>();
                curr_arc->weight = entry_arc->text().toInt();
                curr_arc->line->set_weight(curr_arc->weight);
                QString new_text = combo_arcs->currentText().left(combo_arcs->currentText().lastIndexOf(':') + 1);
                combo_arcs->setItemText(combo_arcs->currentIndex(),
                                        new_text + " "
                                        + QString::number(combo_arcs->itemData(combo_arcs->currentIndex()).value<TransitionStruct *>()->weight)
                                    );
            }
            entry_arc->clear();
        });
        // -----------------------------------

        dock_left->show();
        dock_left->raise();
    }
    else {
        dock_left->hide();
    }
};