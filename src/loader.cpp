/** 
 * @file loader.cpp
 * @author Filip Kachyňa (xkachyf00)
 * @brief Implementation of the Loader, which loads parsed config file into GUI.
*/

#include "loader.h"
#include "globals.h"
#include "parser.h"
#include "labels.h"

void load_arc_into_gui (QMainWindow &window, drop_widget* canvas, const TransitionStruct& new_arc);

void load_net_into_gui(QMainWindow &window, drop_widget* canvas, NetParser &parser) {
    // Load new cells into GUI
    int cell_id = 0;
    int cells_per_row = 8;
    for (const auto& cell : parser.parsed_cells) {
        click_label *new_label = new click_label(QString("<b>%1</b>").arg(cell.name), "background-color: darkGray;" + cell_style, canvas);
                
        // Calculate position based on cell_id to arrange in grid
        int row = cell_id / cells_per_row;
        int col = cell_id % cells_per_row;
        QPoint pos(500 + col * 150, 150 + row * 150);

        CellStruct new_cell = {new_label, QString("<b>%1</b>").arg(cell.name), pos, cell.tokens};
        cell_list.push_back(new_cell);
                
        new_label->setText(QString("<b>%1:<br>%2</b>").arg(cell.name).arg(cell.tokens));
        new_label->setStyleSheet("background-color: darkGray;" + cell_style);
        new_label->setAlignment(Qt::AlignCenter);

        new_label->move(pos);
        new_label->show();
        placed_labels.push_back(new_label);

        cell_id++;
    }
    cout << "curr cell list size: " << cell_list.size() << endl;

    // Load new transitions into GUI
    int trans_id = 0;
    int trans_per_row = 8;
    std::vector<std::pair<TransLabelStruct, std::vector<TransitionParsed::Arc>>> trans_input_arcs;
    std::vector<std::pair<TransLabelStruct, std::vector<TransitionParsed::Arc>>> trans_output_arcs;
    for (const auto& trans : parser.parsed_transitions) {
        click_trans_label *new_label = new click_trans_label(QString("<b>%1</b>").arg(trans.name), "background-color: darkGray;" + trans_style, canvas);
        new_label->setToolTip(trans.name);
        // Calculate position based on trans_id and cell count to arrange in grid below cells
        int row = trans_id / trans_per_row;
        int col = trans_id % trans_per_row;
        QPoint pos(540 + col * 150, 150 + ((cell_id / cells_per_row) + 1) * 300 + row * 150);

        // Store transition properties
        TransProperties * new_properties = new TransProperties();
        new_properties->delay_ms = trans.delay;
        new_properties->event_name = trans.event_name;
        new_properties->guard_code = trans.guard;
        new_properties->action_code = trans.action;
                
        TransLabelStruct new_trans = {new_label, new_label->text(), pos, new_properties};
        trans_list.push_back(new_trans);

        new_label->setStyleSheet("QToolTip { min-width: 0px; min-height: 0px; padding: 2px; background-color: #ffffe0; color: black; border: 1px solid black; font-size: 12px; } QLabel { background-color: darkGray;" + trans_style + "}");
        new_label->setAlignment(Qt::AlignCenter);

        new_label->move(pos);
        new_label->show();
        placed_trans_labels.push_back(new_label);

        trans_input_arcs.push_back({new_trans, trans.inputs});
        trans_output_arcs.push_back({new_trans, trans.outputs});

        trans_id++;
    }
    cout << "curr trans list size: " << trans_list.size() << endl;

    // Load new arcs into GUI
    bool curr_direction = true;
    for (const auto& trans : trans_input_arcs) {
        for (const auto& arc : trans.second) {
            // Find cell label
            QLabel* cell_label = nullptr;
            for (const auto& cell : cell_list) {
                if (cell.label_text.contains(arc.place_name)) {
                    cell_label = cell.label_ptr;
                    break;
                }
            }
            if (!cell_label) {
                QMessageBox::critical(&window, "Error", "Failed to find cell for arc.");
                continue;
            }

            TransitionStruct new_arc = {trans.first.label_ptr, cell_label, curr_direction, arc.weight, nullptr};

            load_arc_into_gui(window, canvas, new_arc);
        }
    }
            
    curr_direction = false;
    for (const auto& trans : trans_output_arcs) {
        for (const auto& arc : trans.second) {
            // Find cell label
            QLabel* cell_label = nullptr;
            for (const auto& cell : cell_list) {
                if (cell.label_text.contains(arc.place_name)) {
                    cell_label = cell.label_ptr;
                    break;
                }
            }
            if (!cell_label) {
                QMessageBox::critical(&window, "Error", "Failed to find cell for arc.");
                continue;
            }

            TransitionStruct new_arc = {trans.first.label_ptr, cell_label, curr_direction, arc.weight, nullptr};

            load_arc_into_gui(window, canvas, new_arc);
        }
    }
}

// Function for loading an arc into GUI, almost same code as in tools.cpp
void load_arc_into_gui(QMainWindow &window, drop_widget* canvas, const TransitionStruct& new_arc) {
     // Detect duplicate arcs
    bool already_is = std::find_if(trans_cell_list.begin(), trans_cell_list.end(), [&](const TransitionStruct &t) {
    return  (t.cell_label == new_arc.cell_label) &&
            (t.trans_label == new_arc.trans_label) &&
            (t.to_from == new_arc.to_from);
    }) != trans_cell_list.end();
    if (already_is) {
        QMessageBox::critical(&window, "Error", "Duplicate arc detected in configuration.");
        return;
    }

    // Add arc to list before creating line
    trans_cell_list.push_back(new_arc);

    // Line geometry calculation
    if (new_arc.to_from) {
        QPoint start = new_arc.cell_label->pos() + QPoint(50, 50);
        QPoint end = new_arc.trans_label->pos() + QPoint(10, 50);

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
                    
        line_widget * line = new line_widget(local_start, local_end, new_arc.weight, canvas);

        LineStruct line_struct = {line, trans_cell_list.back().cell_label, trans_cell_list.back().trans_label};

        placed_lines.push_back(line_struct);
                    
        trans_cell_list.back().line = line;

        line->setGeometry(x, y, w, h);
        line->show();
        line->lower();
    } else {
        QPoint start = new_arc.trans_label->pos() + QPoint(10, 50);
        QPoint end = new_arc.cell_label->pos() + QPoint(50, 50);
                    
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

        line_widget * line = new line_widget(local_start, local_end, new_arc.weight, canvas);

        LineStruct line_struct = {line, trans_cell_list.back().cell_label, trans_cell_list.back().trans_label};
                    
        placed_lines.push_back(line_struct);

        trans_cell_list.back().line = line;

        line->setGeometry(x, y, w, h);
        line->show();
        line->lower();
    }
}