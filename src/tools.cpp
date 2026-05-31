/**
 * @file tools.cpp
 * @author Šimon Čorej (xcorejs00)
 * @brief Implementation of show_tools() function
 */
#include "labels.h"
#include "tools.h"
#include "globals.h"

// --------------------------------- SHOW TOOLS -----------------------------------------
void show_tools(QDockWidget * dock_right, QWidget * sidebar_widget, QVBoxLayout * layout, QWidget * canvas, bool visible){
    // ------------------------- TOOL WIDGET -------------------------------------
    if (!visible){
        cout << "Not visible creating tool widget..." << endl;

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

        // ------------ HEADER ---------------
        layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

        QLabel * header = new QLabel("<h2>TOOLS</h2>");
        header->setStyleSheet(
            "min-width: 200px;"
            "border: 2px solid black;"
        );
        header->setAlignment(Qt::AlignCenter);

        layout->addWidget(header);
        layout->addSpacing(20);
        // -----------------------------------

        // ------------ CELL LABEL -----------
        drag_widget * cell_drg_wdgt = new drag_widget("<b>CELL</b>", "background-color: darkGray;" + cell_style, sidebar_widget);
        cell_drg_wdgt->setFixedSize(100, 100);
        layout->addWidget(cell_drg_wdgt, 0, Qt::AlignHCenter);
        layout->addSpacing(20);
        // -----------------------------------

        // ----------- TRANS LABEL -----------
        QLabel * transition_text = new QLabel("<b>Transition</b>");
        transition_text->setAlignment(Qt::AlignCenter);
        layout->addWidget(transition_text);

        drag_widget * trans_drg_wdgt = new drag_widget("<b></b>", "background-color: darkGray;" + trans_style, sidebar_widget);
        trans_drg_wdgt->setFixedSize(20, 100);
        layout->addWidget(trans_drg_wdgt, 0, Qt::AlignHCenter);
        layout->addSpacing(20);
        // -----------------------------------

        // ------- TRANSITION BUTTON ---------
        QPushButton * trans_button = new QPushButton();
        trans_button->setCheckable(true);
        trans_button->setText("Add transition");
        trans_button->setStyleSheet(
            "background-color: darkGray;"
            "min-width: 25px;"
            "min-height: 50px;"
            "text-align: center;"
            "font-weight: bold;"
        );
        layout->addWidget(trans_button);
        // -----------------------------------

        // ----- SAVE TRANSITION BUTTON ------
        QPushButton * save_trans = new QPushButton();
        save_trans->setText("Save transition");
        save_trans->setStyleSheet(
            "background-color: darkGray;"
            "min-width: 25px;"
            "min-height: 50px;"
            "text-align: center;"
            "font-weight: bold;"
        );
        layout->addWidget(save_trans);
        save_trans->hide();
        // -----------------------------------

        // ------ DELETE BUTTON ACTION -------
        QPushButton * del_button = new QPushButton();
        del_button->setCheckable(true);
        del_button->setText("Delete");
        del_button->setStyleSheet(
            "background-color: darkGray;"
            "min-width: 25px;"
            "min-height: 50px;"
            "text-align: center;"
            "font-weight: bold;"
        );
        layout->addWidget(del_button);


        // -- SAVE TRANSITION BUTTON ACTION --
        QObject::connect(save_trans, &QPushButton::clicked, [save_trans, trans_button, canvas](bool checked_save){
            if (curr_selected_cells.size() < 1 || curr_selected_trans.size() < 1) {
                cout << "Not enough selected! Need 1 cell and 1 transition. Aborting..." << endl;
                curr_selected_cells.clear();
                curr_selected_trans.clear();
                trans_button->setChecked(false);
                save_trans->hide();
                
                for (auto label : placed_labels){
                label->reset();
                }
                for (auto label : placed_trans_labels){
                    label->reset();
                }
                return;
            }

            TransitionStruct curr_trans = {curr_selected_trans.at(0), curr_selected_cells.at(0), curr_direction};
            
            bool already_is = std::find_if(
                trans_cell_list.begin(), trans_cell_list.end(), [&](const TransitionStruct &t){
                    return (t.cell_label == curr_trans.cell_label) &&
                    (t.trans_label == curr_trans.trans_label) &&
                    (t.to_from == curr_trans.to_from);
                }
                )!= trans_cell_list.end();
            
            if (!already_is){
                trans_cell_list.push_back(curr_trans);

                // Drawing a line
                if (curr_trans.to_from){
                    QPoint start = curr_trans.cell_label->pos() + QPoint(50, 50);
                    QPoint end = curr_trans.trans_label->pos() + QPoint(10, 50);

                    double alpha = std::atan2(end.y() - start.y(), end.x() - start.x());
                    QPoint cell_start_point = start + QPoint(50 * std::cos(alpha), 50 * std::sin(alpha));

                    int text_margin = 11;
                    int padding = 20 + text_margin;
                    int x = std::min(cell_start_point.x(), end.x()) - padding;
                    int y = std::min(cell_start_point.y(), end.y()) - padding;
                    int w = std::abs(end.x() - cell_start_point.x()) + padding * 2;
                    int h = std::abs(end.y() - cell_start_point.y()) + padding * 2;

                    QPoint offset(x, y);
                    QPoint local_start = cell_start_point - offset;
                    QPoint local_end = end - offset;
                    
                    line_widget * line = new line_widget(local_start, local_end, curr_trans.weight, canvas);

                    LineStruct line_struct = {line, trans_cell_list.back().cell_label, trans_cell_list.back().trans_label};

                    placed_lines.push_back(line_struct);
                    
                    trans_cell_list.back().line = line;

                    line->setGeometry(x, y, w, h);
                    line->show();
                    line->lower();
                }
                else {
                    QPoint start = curr_trans.trans_label->pos() + QPoint(10, 50);
                    QPoint end = curr_trans.cell_label->pos() + QPoint(50, 50);
                    
                    double alpha = std::atan2(end.y() - start.y(), end.x() - start.x());
                    QPoint cell_end_point = end - QPoint(50 * std::cos(alpha), 50 * std::sin(alpha));

                    int text_margin = 11;
                    int padding = 20 + text_margin;
                    int x = std::min(start.x(), cell_end_point.x()) - padding;
                    int y = std::min(start.y(), cell_end_point.y()) - padding;
                    int w = std::abs(cell_end_point.x() - start.x()) + padding * 2;
                    int h = std::abs(cell_end_point.y() - start.y()) + padding * 2;

                    QPoint offset(x, y);
                    QPoint local_start = start - offset;
                    QPoint local_end = cell_end_point - offset;

                    line_widget * line = new line_widget(local_start, local_end, curr_trans.weight, canvas);

                    LineStruct line_struct = {line, trans_cell_list.back().cell_label, trans_cell_list.back().trans_label};
                    
                    placed_lines.push_back(line_struct);

                    trans_cell_list.back().line = line;

                    line->setGeometry(x, y, w, h);
                    line->show();
                    line->lower();
                }
            }
            else {
                cout << "Transition already exists..." << endl;
            }

            cout << "All saved transitions:" << endl;
            for (auto &row : trans_cell_list) {
                cout << (QString("  %1 -> %2")
                    .arg(row.to_from == 1 ? row.cell_label->text() : row.trans_label->text())
                    .arg(row.to_from == 1 ? row.trans_label->text() : row.cell_label->text()))
                    .toStdString() << endl;
            }

            // Reset all placed cells to their defualt color
            curr_selected_cells.clear();
            curr_selected_trans.clear();
            curr_direction = true;

            for (auto label : placed_labels){
                label->reset();
            }
            for (auto label : placed_trans_labels){
                label->reset();
            }
        });
        // -----------------------------------
        
        // ------- TRANS BUTTON ACTION -------
        QObject::connect(trans_button, &QPushButton::toggled, [layout, save_trans, del_button](bool checked){
            // Only shows save button if pressed
            if (checked){
                label_click_allowed = true;
                save_trans->show();
                del_button->setChecked(false);
                del_button->hide();
                cout << "Button on..." << endl;
            }
            else {
                label_click_allowed = false;
                save_trans->hide();
                del_button->show();

                // Reset all placed cells to their defualt color
                for (auto label : placed_labels){
                    label->reset();
                }
                for (auto label : placed_trans_labels){
                    label->reset();
                }
                // Clear list of current cells selected
                curr_selected_cells.clear();
                curr_selected_trans.clear();
                cout << "Button off..." << endl;
            }
        });
        // -----------------------------------

        // ------ DELETE BUTTON ACTION -------
        QObject::connect(del_button, &QPushButton::toggled, [](bool checked){
            if (!label_click_allowed){
                if (checked){
                    delete_allowed = true;
                }
                else {
                    delete_allowed = false;
                }
            }
            else {
                delete_allowed = false;
            }
        });
        // -----------------------------------

        // ----- SEND EVENTS -----------------
        layout->addSpacing(30);

        QLabel * event_header = new QLabel("<b>SIMULATION EVENTS</b>");
        event_header->setAlignment(Qt::AlignCenter);
        layout->addWidget(event_header);

        QLineEdit * event_input = new QLineEdit();
        event_input->setPlaceholderText("Event name (e.g. test)");
        event_input->setStyleSheet(
            "min-height: 30px;"
            "border: 2px solid black;"
        );
        layout->addWidget(event_input);

        QPushButton * send_event_btn = new QPushButton("Send Event");
        send_event_btn->setStyleSheet(
            "background-color: darkGray;"
            "min-height: 40px;"
            "font-weight: bold;"
        );
        layout->addWidget(send_event_btn);

        QObject::connect(send_event_btn, &QPushButton::clicked, [event_input]() {
            QString event_name = event_input->text().trimmed();
            if (!event_name.isEmpty() && gui_socket != nullptr) {
                
                // shortcut for setting input and firing event in one moment
                //: "input_name=value" -> sends SET_INPUT and then EVENT with the same name
                if (event_name.contains("=")) {
                    QStringList parts = event_name.split("=");
                    QString name = parts[0].trimmed();
                    QString value = parts[1].trimmed();
                    
                    // send SET_INPUT to engine
                    QString set_payload = "SET_INPUT:" + name + ":" + value;
                    gui_socket->writeDatagram(set_payload.toUtf8(), QHostAddress::LocalHost, 8080);
                    
                    // send EVENT to engine
                    QString event_payload = "EVENT:" + name;
                    gui_socket->writeDatagram(event_payload.toUtf8(), QHostAddress::LocalHost, 8080);
                    
                    cout << "Sent SET_INPUT and EVENT for: " << name.toStdString() << " = " << value.toStdString() << endl;
                } 
                // custom command ("INJECT:cell1:5" or "SET_INPUT:var1:10")
                else if (event_name.startsWith("EVENT:") || 
                           event_name.startsWith("INJECT:") || 
                           event_name.startsWith("SET_INPUT:")) {
                    gui_socket->writeDatagram(event_name.toUtf8(), QHostAddress::LocalHost, 8080);
                    cout << "Command sent to engine: " << event_name.toStdString() << endl;
                } 
                // event without value
                else {
                    QString payload = "EVENT:" + event_name;
                    gui_socket->writeDatagram(payload.toUtf8(), QHostAddress::LocalHost, 8080);
                    cout << "Event sent to engine: " << event_name.toStdString() << endl;
                }
                event_input->clear();
            }
        });

        // -----------------------------------
        // --------- STOP SIMULATION -----------
        QPushButton * stop_sim_btn = new QPushButton("Stop Simulation");
        stop_sim_btn->setStyleSheet(
            "background-color: #ff4d4d;"
            "min-height: 40px;"
            "font-weight: bold;"
        );
        layout->addWidget(stop_sim_btn);

        QObject::connect(stop_sim_btn, &QPushButton::clicked, []() {
            if (gui_socket != nullptr) {
                QString payload = "STOP";
                QByteArray data = payload.toUtf8();
                gui_socket->writeDatagram(data, QHostAddress::LocalHost, 8080);
                cout << "Stop simulation signal sent to engine" << endl;
            }
            QMessageBox::information(nullptr, "Simulation Stopped", "The simulation has been stopped.");
        });

        dock_right->setWidget(sidebar_widget);
        dock_right->show();

    }
    else {
        cout << "Visible deleting tool widget..." << endl;
        dock_right->hide();
        }
    // ---------------------------------------------------------------------------
}
// --------------------------------------------------------------------------------------