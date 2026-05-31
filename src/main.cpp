/**
 * @file main.cpp
 * @author Šimon Čorej (xcorejs00)
 * @author Patrik Lošťák (xlostap00)
 * @brief Main function
 */
#include "tools.h"
#include "help.h"
#include "object_list.h"
#include "edit_config.h"
#include "globals.h"
#include <QMetaType>
#include "serializer.h"
#include "generator.h"
#include "parser.h"
#include "loader.h"

QUdpSocket *gui_socket = nullptr;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Petri net - visualization");
    window.resize(1000, 900);

    drop_widget * canvas = new drop_widget();
    window.setCentralWidget(canvas);

    // bind udp socket for receiving updates from engine
    gui_socket = new QUdpSocket(&window);
    gui_socket->bind(QHostAddress::LocalHost, 8081);

    // ------------------------ Right dock --------------------------------
    QDockWidget * dock_right = new QDockWidget(&window);
    dock_right->setAllowedAreas(Qt::RightDockWidgetArea);
    dock_right->setStyleSheet(
        "background-color: lightGray;"
    );
    dock_right->setMinimumWidth(200);

    QWidget * sidebar_widget = new QWidget();
    QVBoxLayout * layout_right = new QVBoxLayout(sidebar_widget);
    
    window.addDockWidget(Qt::RightDockWidgetArea, dock_right);
    dock_right->hide();

    show_tools(dock_right, sidebar_widget, layout_right, canvas, dock_right->isVisible());
    // --------------------------------------------------------------------

    // ------------------------ Left dock ---------------------------------
    QWidget * dock_left = new QWidget(canvas);
    dock_left->setFixedWidth(220);
    dock_left->setFixedHeight(900);
    dock_left->setStyleSheet(
        "background-color: lightGray;"
    );
    dock_left->move(0, 0);

    QVBoxLayout * layout_left = new QVBoxLayout(dock_left);
    dock_left->hide();
    // --------------------------------------------------------------------

    // ----------------------- Bottom dock --------------------------------
    QDockWidget * dock_bottom = new QDockWidget("Engine Log", &window);
    dock_bottom->setAllowedAreas(Qt::BottomDockWidgetArea);
    QTextEdit * log_console = new QTextEdit();
    log_console->setReadOnly(true);
    log_console->setStyleSheet(
        "background-color: black; color: #00FF00; font-family: monospace; font-size: 12px;"
    );
    dock_bottom->setWidget(log_console);
    dock_bottom->hide();
    window.addDockWidget(Qt::BottomDockWidgetArea, dock_bottom);

    // ----------------------- Menu bar -----------------------------------
    QMenuBar * menuBar = window.menuBar();
    
    QMenu * file = menuBar->addMenu("File");
    QMenu * edit = menuBar->addMenu("Edit");

    QAction * tools_action = menuBar->addAction("Tools");
    QAction * help_action = menuBar->addAction("Help");
    QAction * view_action = menuBar->addAction("View");
    QAction * log_action = menuBar->addAction("Show log");

    QAction * new_action = file->addAction("New file");
    QAction * open_action = file->addAction("Open file");
    QAction * run_action = file->addAction("Run");
    QAction * edit_action = edit->addAction("Edit configuration");
    QAction * save_action = edit->addAction("Save configuration");

    // --------------------------------------------------------------------

    // ----------------------- Action bind --------------------------------
    QObject::connect(new_action, &QAction::triggered, [&window, save_action](){
        QMessageBox msg;
        msg.setWindowTitle("Warning");
        msg.setText("Do you want to save configuration, or discard it?");
        msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msg.setDefaultButton(QMessageBox::Save);

        int result = msg.exec();

        if (result == QMessageBox::Save){
            save_action->trigger();
        }
        else if (result == QMessageBox::Discard){
            auto labels_to_delete = placed_labels;
            for (auto &item : labels_to_delete){
                delete_cell(item);
            }
            auto trans_labels_to_delete = placed_trans_labels;
            for (auto &item : trans_labels_to_delete){
                delete_transition(item);
            }

            placed_labels.clear();
            placed_trans_labels.clear();
            cell_list.clear();
            trans_list.clear();
            trans_cell_list.clear();
            placed_lines.clear();
            curr_selected_cells.clear();
            curr_selected_trans.clear();

            config_data->net_name = "";
            config_data->comment = "";
            config_data->start_places.clear();
            config_data->end_places.clear();
            config_data->variables.clear();
        }
    });

    QObject::connect(help_action, &QAction::triggered, [dock_right, sidebar_widget, layout_right]() {
        bool visible = dock_right->isVisible();
        show_help(dock_right, sidebar_widget, layout_right, visible);
    });

    QObject::connect(tools_action, &QAction::triggered, [dock_right, sidebar_widget, layout_right, canvas]() {
        bool visible = dock_right->isVisible();
        show_tools(dock_right, sidebar_widget, layout_right, canvas, visible);
    });

    QObject::connect(view_action, &QAction::triggered, [dock_left, layout_left, canvas](){
        bool visible = dock_left->isVisible();
        show_list(dock_left, layout_left, canvas, visible);
    });

    QObject::connect(edit_action, &QAction::triggered, [dock_left, layout_left](){
        bool visible = dock_left->isVisible();
        edit_config(dock_left, layout_left, visible);
    });

    QObject::connect(save_action, &QAction::triggered, [&window](){
        QString file_name = QFileDialog::getSaveFileName(&window, "Save net", "", "Text files (*.txt);;All files (*)");

        if (!file_name.isEmpty()) {
            if (!file_name.endsWith(".txt")) {
                file_name += ".txt";
            }
            
            if (save_net_to_file(file_name)) {
                QMessageBox::information(&window, "Success", "Net saved successfully.");
            } else {
                QMessageBox::critical(&window, "Error", "Failed to save the net.");
            }
        }
    });

    QObject::connect(open_action, &QAction::triggered, [&window, canvas, new_action](){
        // Open file dialog
        QString file_name = QFileDialog::getOpenFileName(&window, "Open net", "", "Text files (*.txt);;All files (*)");

        // Check if a file was selected
        if (!file_name.isEmpty()) {
            // Validate file extension
            if (!file_name.endsWith(".txt")) {
                QMessageBox::critical(&window, "Error", "Please select a valid .txt file.");
                return;
            }

            // Trigger new action to clear current net
            new_action->trigger();

            // Load conf file via parser
            NetParser parser;
            if (!parser.parse(file_name)) {
                QMessageBox::critical(&window, "Error", "Failed to parse the net configuration.", parser.error_log.join("\n"));
                return;
            }

            // Load new net into GUI
            load_net_into_gui(window, canvas, parser);
        }
    });

    QObject::connect(run_action, &QAction::triggered, [&window](){
        // Save current config to temp file
        QString temp_file = "tmp_run_conf.txt";
        if (!save_net_to_file(temp_file)) {
            QMessageBox::critical(&window, "Error", "Failed to save the net configuration.");
            return;
        }

        // Load conf file via parser
        NetParser parser;
        config_data->start_places.clear();
        config_data->end_places.clear();
        if (!parser.parse(temp_file)) {
            QMessageBox::critical(&window, "Error", "Failed to parse the net configuration.", parser.error_log.join("\n"));
            return;
        }

        // Generate engine code
        QString output_path = "petri_net_run.cpp";
        if (!generate_engine_code(parser, output_path)) {
            QMessageBox::critical(&window, "Error", "Failed to generate engine code.");
            return;
        }

        // Compile the generated code via g++
        QProcess compiler;
        QStringList args;
        args << "-I" << "src" << output_path << "src/engine.cpp" << "-o" << "petri_engine";
        compiler.start("g++", args);
        compiler.waitForFinished();

        if (compiler.exitCode() != 0) {
            QString error_output = compiler.readAllStandardError();
            QMessageBox::critical(&window, "Compilation Error", "Failed to compile the engine code:\n" + error_output);
            return;
        }

        // Run the compiled engine
        QProcess::startDetached("./petri_engine", QStringList(), QString(), nullptr);
        QMessageBox::information(&window, "Engine Started", "The Petri net engine has been started.");
        
    });

    QObject::connect(log_action, &QAction::triggered, [dock_bottom](){
        bool visible = dock_bottom->isVisible();
        if (!visible){
            dock_bottom->show();
        }
        else{
            dock_bottom->hide();
        }
    });

    // GUI socket for receiving state updates from the engine
    QObject::connect(gui_socket, &QUdpSocket::readyRead, [&]() {
        while (gui_socket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = gui_socket->receiveDatagram();
            QString event_data = QString::fromUtf8(datagram.data());
            std::cout << "[LOG] Received event from engine: " << event_data.toStdString() << std::endl;

            // Process the received event like "STATE_UPDATE:CELL_1:5"
            if (event_data.startsWith("STATE_UPDATE:")) {
                QStringList parts = event_data.split(":");
                if (parts.size() == 3) {
                    QString place_name = parts[1];
                    int new_tokens = parts[2].toInt();

                    // Find cell and update
                    for (auto& cell : cell_list) {
                       QString gui_name = cell.label_ptr->text();
                       gui_name.remove(QRegularExpression("<[^>]*>")); // no html tags
                       int idx = gui_name.indexOf(":");
                       if (idx != -1) {
                            gui_name = gui_name.left(idx);
                       }
                       gui_name = gui_name.replace(" - ", "_").replace("-", "_").replace(" ", "_").trimmed();

                        if (gui_name == place_name) {
                            cell.tokens = new_tokens;
                            cell.label_ptr->setText(cell.label_text + "<b>:<br>" + QString::number(new_tokens) + "</b>");
                            break;
                        }
                    }
                }
            } else if (event_data.startsWith("LOG:")) {
                QString log_message = event_data.mid(4);
                log_console->append(log_message);
            } else if (event_data.startsWith("PONG:")) {
                QString pong_message = event_data.mid(5);
                log_console->append("Successfully connected to network " + pong_message);

                NetParser parser;
                if (!parser.parse("tmp_run_conf.txt")) {
                    log_console->append("Failed to parse the net configuration for serialization.");
                } else {
                    load_net_into_gui(window, canvas, parser);
                    log_console->append("Successfully loaded the net configuration from file.");
                }
            } else if (event_data.startsWith("TRANS_COLOR:")) {
                QStringList parts = event_data.split(":");
                if (parts.size() == 3) {
                    QString trans_id = parts[1];
                    QString color = parts[2];

                    for (auto& trans : trans_list) {
                        QString gui_name = trans.label_ptr->text();
                        gui_name.remove(QRegularExpression("<[^>]*>")); // no html tags
                        gui_name = gui_name.replace(" - ", "_").replace("-", "_").replace(" ", "_").trimmed();

                        if ("T_" + gui_name == trans_id || gui_name == trans_id) {
                            trans.label_ptr->setStyleSheet("QToolTip { min-width: 0px; min-height: 0px; padding: 2px; background-color: #ffffe0; color: black; border: 1px solid black; font-size: 12px; } QLabel { background-color: " + color + ";" + trans_style + "}");
                        }
                    }
                }
            }
        }
    });

    // --------------------------------------------------------------------
    // Test ping
    if (gui_socket != nullptr) {
        gui_socket->writeDatagram("PING", QHostAddress::LocalHost, 8080);
    }

    // --------------------------------------------------------------------
    window.show();
    return app.exec();
}
