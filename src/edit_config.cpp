/**
 * @file edit_config.cpp
 * @author Šimon Čorej (xcorejs00)
 * @brief Implementation of GUI side config file edit
 */

#include "edit_config.h"
#include "globals.h"

// Prints config data
void print_config_data(){
    cout << "------------------------------------------" << endl;
    cout << "CONFIG DATA: \nnet name: " << config_data->net_name.toStdString() << endl;
    cout << "comment: " << config_data->comment.toStdString() << endl;
    cout << "start_places: " << ((config_data->start_places.empty()) ?  "" : config_data->start_places.at(0)->label_text.toStdString()) << endl;
    cout << "end_places: " << ((config_data->end_places.empty())? "" : config_data->end_places.at(0)->label_text.toStdString()) << endl;
    cout << "variables: \n";
    for (const QString& v : config_data->variables) {
        cout << "    " << v.toStdString() << endl;
    }
    cout << "------------------------------------------" << endl;
}

void edit_config(QWidget * dock_left, QVBoxLayout * layout_left, bool visible){
    if (!visible){
        cout << "Edit action triggered..." << endl;
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
                    if (nested_item->widget()) {
                        nested_item->widget()->hide();
                        delete nested_item->widget();
                    }
                }
                delete nested;
            }
        }
        // -----------------------------------

        layout_left->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

        // ------------- HEADER ---------------
        QHBoxLayout * row = new QHBoxLayout();

        QLabel * header = new QLabel();
        header->setText("<h3>EDIT CONFIG</h3>");
        header->setStyleSheet(
            "min-width: 130px;"
            "max-width: 160px;"
            "max-height: 40px;"
            "border: 2px solid black;"
        );
        header->setAlignment(Qt::AlignCenter);
        row->addWidget(header);

        QPushButton * refresh_button = new QPushButton("↻");
        refresh_button->setStyleSheet(
            "background-color: gray;"
            "min-width: 40px;"
            "max-width: 40px;"
            "font-size: 20px;"
        );
        row->addWidget(refresh_button, Qt::AlignRight);
        layout_left->addLayout(row);
        layout_left->addSpacing(20);
        // -----------------------------------

        // ----------- NET NAME --------------
        QLabel * net_name = new QLabel("<b>Net name: </b>");
        net_name->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "max-height: 20px;"
        );
        net_name->setAlignment(Qt::AlignCenter);
        layout_left->addWidget(net_name);
        
        QHBoxLayout * entry_name_add = new QHBoxLayout();
        QLineEdit * entry_name = new QLineEdit();
        entry_name->setPlaceholderText("Add net name");
        entry_name->setStyleSheet(
            "min-width: 140px;"
            "max-width: 160px;"
            "min-height: 30px;"
            "font-size: 14px;"
            "border: 2px solid black;"
        );
        QPushButton * add_name = new QPushButton("Add");
        add_name->setStyleSheet(
            "background-color: gray;"
            "min-width: 40px;"
            "max-width: 40px;"
            "min-height: 30px;"
            "font-size: 15px;"
        );
        entry_name_add->addWidget(entry_name);
        entry_name_add->addWidget(add_name);
        layout_left->addLayout(entry_name_add);
        layout_left->addSpacing(20);        
        // -----------------------------------

        // ------------ COMMENT --------------
        QLabel * comment = new QLabel("<b>Comment: </b>");
        comment->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "max-height: 40px;"
        );
        comment->setAlignment(Qt::AlignCenter);
        layout_left->addWidget(comment);

        QTextEdit * comment_text = new QTextEdit();
        comment_text->setPlaceholderText("Add comment");
        comment_text->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "max-height: 100px;"
            "border: 2px solid black;"
        );
        layout_left->addWidget(comment_text);

        QPushButton * add_comment = new QPushButton();
        add_comment->setText("Add");
        add_comment->setStyleSheet(
            "background-color: gray;"
            "min-width: 200px;"
            "max-width: 200px;"
            "min-height: 30px;"
            "font-size: 15px;"
        );
        layout_left->addWidget(add_comment);
        layout_left->addSpacing(20);
        // -----------------------------------

        // ---------- START PLACE ------------
        QLabel * start_place = new QLabel("<b>Entry Place:</b>");
        start_place->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "max-height: 20px;"
        );
        start_place->setAlignment(Qt::AlignCenter);
        layout_left->addWidget(start_place);

        QComboBox * combo_cell_start = new QComboBox();
        for (size_t i = 0; i < cell_list.size(); i++){
            QString new_text = cell_list[i].label_ptr->text();
            new_text.remove(QRegularExpression("<[^>]*>"));
            QStringList new_text_split = new_text.split(":");
            combo_cell_start->addItem(new_text_split[0] 
                                + " : " 
                                + (new_text_split.size() > 1 ? new_text_split[1] : "")
                                , QVariant::fromValue(&cell_list[i]));
        }
        layout_left->addWidget(combo_cell_start);
        layout_left->addSpacing(20);
        // -----------------------------------

        // ----------- END PLACE -------------
        QLabel * end_place = new QLabel("<b>Exit Place:</b>");
        end_place->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "max-height: 20px;"
        );
        end_place->setAlignment(Qt::AlignCenter);
        layout_left->addWidget(end_place);

        QComboBox * combo_cell_end = new QComboBox();
        for (size_t i = 0; i < cell_list.size(); i++){
            QString new_text = cell_list[i].label_ptr->text();
            new_text.remove(QRegularExpression("<[^>]*>"));
            QStringList new_text_split = new_text.split(":");
            combo_cell_end->addItem(new_text_split[0] 
                                + " : " 
                                + (new_text_split.size() > 1 ? new_text_split[1] : "")
                                , QVariant::fromValue(&cell_list[i]));
        }
        layout_left->addWidget(combo_cell_end);
        layout_left->addSpacing(20);
        // -----------------------------------

        // ------------ VARIABLE -------------
        QLabel * variable = new QLabel("<b>Variables (C++ code):</b>");
        variable->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "max-height: 20px;"
        );
        variable->setAlignment(Qt::AlignCenter);
        layout_left->addWidget(variable);

        QTextEdit * entry_var = new QTextEdit();
        entry_var->setPlaceholderText("int citac = 0;\ndouble teplota = 36.6;");
        entry_var->setStyleSheet(
            "min-width: 200px;"
            "max-width: 200px;"
            "max-height: 100px;"
            "border: 2px solid black;"
        );
        layout_left->addWidget(entry_var);

        QPushButton * add_var = new QPushButton("Save variables");
        add_var->setStyleSheet(
            "background-color: gray;"
            "min-width: 200px;"
            "max-width: 200px;"
            "min-height: 30px;"
            "font-size: 15px;"
        );
        layout_left->addWidget(add_var);
        layout_left->addSpacing(20);
        // -----------------------------------

        // ---------- ACTION BIND ------------
        QObject::connect(add_name, &QPushButton::clicked, [entry_name, net_name](){
            config_data->net_name = entry_name->text();
            net_name->setText("<b>Net name: " + config_data->net_name + "</b>");
            entry_name->clear();
            // print_config_data();
        });

        QObject::connect(add_comment, &QPushButton::clicked, [comment_text, comment](){
            config_data->comment = comment_text->toPlainText();
            comment->setText("<b>Comment: ✔</b>");
            // print_config_data();
        });

        QObject::connect(comment_text, &QTextEdit::textChanged, [comment](){
            comment->setText("<b>Comment:</b>");
        });
        
        QObject::connect(combo_cell_start, QOverload<int>::of(&QComboBox::activated), [combo_cell_start](int index){
            bool found = false;
            config_data->start_places.erase(
                std::remove_if(
                    config_data->start_places.begin(), 
                    config_data->start_places.end(), 
                    [combo_cell_start, &found](const CellStruct * c) {
                        if (c == combo_cell_start->currentData().value<CellStruct * >()){
                            found = true;
                            cout << "removed ... " << endl;
                            return true;
                        }
                        return false;
                    }
                ), config_data->start_places.end()
            );
            if (!found){
                cout << "running..." << endl;
                config_data->start_places.push_back(combo_cell_start->itemData(index).value<CellStruct *>());
                combo_cell_start->setItemText(combo_cell_start->currentIndex(), combo_cell_start->currentText() + " ✔");
            }
            else {
                int index = combo_cell_start->currentIndex();
                combo_cell_start->setItemText(index, combo_cell_start->currentText().remove(" ✔"));
            }
            // print_config_data();
        });

        QObject::connect(combo_cell_end, QOverload<int>::of(&QComboBox::activated), [combo_cell_end](int index){
            bool found = false;
            config_data->end_places.erase(
                std::remove_if(
                    config_data->end_places.begin(), 
                    config_data->end_places.end(), 
                    [combo_cell_end, &found](const CellStruct * c) {
                        if (c == combo_cell_end->currentData().value<CellStruct * >()){
                            found = true;
                            cout << "removed ... " << endl;
                            return true;
                        }
                        return false;
                    }
                ), config_data->end_places.end()
            );

            if (!found){
                cout << "running..." << endl;
                config_data->end_places.push_back(combo_cell_end->itemData(index).value<CellStruct *>());
                combo_cell_end->setItemText(combo_cell_end->currentIndex(), combo_cell_end->currentText() + " ✔");
            }
            else {
                int index = combo_cell_end->currentIndex();
                combo_cell_end->setItemText(index, combo_cell_end->currentText().remove(" ✔"));
            }
            // print_config_data();
        });

        QObject::connect(add_var, &QPushButton::clicked, [entry_var, variable](){
            QString raw_text = entry_var->toPlainText();
            variable->setText("<b>Variables (C++ code): ✔</b>");

            // Split variables by new line, ignore empty lines
            #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0) // Qt mismatch version check
                config_data->variables = raw_text.split('\n', Qt::SkipEmptyParts);
            #else
                config_data->variables = raw_text.split('\n', QString::SkipEmptyParts);
            #endif
            // print_config_data();
        });

        QObject::connect(entry_var, &QTextEdit::textChanged, [variable](){
            variable->setText("<b>Variables (C++ code):</b>");
        });

        QObject::connect(refresh_button, &QPushButton::clicked, [dock_left, layout_left, visible](){
            edit_config(dock_left, layout_left, visible);
        });
        // -----------------------------------

        dock_left->show();
    }
    else {
        dock_left->hide();
    }
}