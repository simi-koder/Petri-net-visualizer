/** 
 * @file serializer.cpp
 * @author Patrik Lošťák (xlostap00)
 * @brief Implementation of the Serializer, which handles saving and loading.
*/

#include "serializer.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QRegularExpression>
#include "globals.h"

// "<b>CELL - 1</b>" --> "CELL - 1"
QString clean_txt(const QString& html_txt) {
    QString output = html_txt;
    // remove all html tags
    output.remove(QRegularExpression("<[^>]*>"));

    // remove all from : to end 
    int colon_index = output.indexOf(':');
    if (colon_index != -1) {
        output = output.left(colon_index);
    }

    // replace bad chars
    output = output.replace(" - ", "_"); // "CELL - 1" -> "CELL_1"
    output = output.replace("-", "_");
    output = output.replace(" ", "_");

    return output.trimmed();
}

bool save_net_to_file(const QString& file_path) {
    QFile file(file_path);
    // try to open the file for writing
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    // Header for net
    QString netName = config_data->net_name.isEmpty() ? "Vychozi_Sit" : config_data->net_name;
    QString netComment = config_data->comment.isEmpty() ? "Bez komentare" : config_data->comment;
    out << "Jmeno site:\n    " << netName << "\n";
    out << "Komentar:\n    " << netComment << "\n";

    if (!config_data->start_places.empty()) {
        out << "Vstupy:\n    ";
        for (const auto& start_place : config_data->start_places) {
            if (start_place != nullptr) {
                out << "    " << clean_txt(start_place->label_text) << "\n";
            }
        }
    } else {
        out << "Vstupy:\n    None\n";
    }

    if (!config_data->end_places.empty()) {
        out << "Vystupy:\n    ";
        for (const auto& end_place : config_data->end_places) {
            if (end_place != nullptr) {
                out << "    " << clean_txt(end_place->label_text) << "\n";
            }
        }
    } else {
        out << "Vystupy:\n    None\n";
    }

    out << "Promenne:\n";
if (config_data->variables.isEmpty()) {
    out << "    None\n";
} else {
    for (const QString& var : config_data->variables) {
        out << "    " << var << "\n";
    }
}

    // Save cells
    out << "Mista (pocatecni tokeny, volitelne akce):\n";
    for (const auto& cell : cell_list) {
        QString name = clean_txt(cell.label_ptr->text());
        // Writes P1 (1) : { }
        out << "    " << name << " (" << cell.tokens <<") : { }\n";
    }

    // Save transitions
    out << "Prechody (prvni blok = arcs, druhy blok = podminky a akce):\n";
    for (const auto& trans : trans_list) {
        QString trans_name = clean_txt(trans.label_ptr->text());
        // Writes T1 :
        out << "T_" << trans_name << " :\n";

        QStringList in_arcs;
        QStringList out_arcs;
        for (const auto& arc :  trans_cell_list) {
            if (arc.trans_label == trans.label_ptr) {
                QString cell_name = clean_txt(arc.cell_label->text());
                QString weight_str = QString::number(arc.weight);
                if (arc.to_from) {
                    // to_from == 1 -> to transition from cell
                    in_arcs << (cell_name + "*" + weight_str); // default 1
                } else {
                    // to_from == 0 -> from transition to cell
                    out_arcs << (cell_name + "*" + weight_str); // default 1
                }
            }
        }

        // Write list of in and out arcs
        out << "    in:  " << in_arcs.join(", ") << "\n";
        out << "    out: " << out_arcs.join(", ") << "\n";


        out << "    when:  " << trans.properties->event_name 
            << " [ " << trans.properties->guard_code << " ] " 
            << "@ " << trans.properties->delay_ms << "\n";
            
        out << "    do: { " << trans.properties->action_code << " }\n";
    }

    file.close();
    return true;
}

