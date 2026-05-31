/**
 * @file labels.cpp
 * @author Šimon Čorej (xcorejs00)
 * @brief Contains delete functions of labels passed by argument
 */
#include "labels.h"
#include "globals.h"

void delete_cell(QLabel * item){
    placed_lines.erase(
        std::remove_if(placed_lines.begin(), placed_lines.end(), [item](const LineStruct &l){
            if (l.cell_label == item){
                l.line->hide();
                l.line->deleteLater();
                return 1;
            }
            return 0;
        }),
        placed_lines.end()
    );

    curr_selected_cells.erase(
        std::remove(curr_selected_cells.begin(), curr_selected_cells.end(), item),
        curr_selected_cells.end()
    );
    placed_labels.erase(
        std::remove(placed_labels.begin(), placed_labels.end(), item),
        placed_labels.end()
    );
    cell_list.erase(
        std::remove_if(cell_list.begin(), cell_list.end(), [item](const CellStruct & c){
            return c.label_ptr == item;
        }),
        cell_list.end()
    );
    trans_cell_list.erase(
        std::remove_if(trans_cell_list.begin(), trans_cell_list.end(), [item](TransitionStruct & t){
            return t.cell_label == item;
        }),
        trans_cell_list.end()
    );
    item->hide();
    item->deleteLater();
}

void delete_transition(QLabel * item){
    placed_lines.erase(
        std::remove_if(placed_lines.begin(), placed_lines.end(), [item](const LineStruct & l){
             if (l.trans_label == item){
                l.line->hide();
                l.line->deleteLater();
                return 1;
            }
            return 0;
        }),
        placed_lines.end()
    );
    curr_selected_trans.erase(
        std::remove(curr_selected_trans.begin(), curr_selected_trans.end(), item),
        curr_selected_trans.end()
    );
    placed_trans_labels.erase(
        std::remove(placed_trans_labels.begin(), placed_trans_labels.end(), item),
        placed_trans_labels.end()
    );
    for (auto & tl : trans_list) {
        if (tl.label_ptr == item) {
            delete tl.properties;
        }
    }
    trans_list.erase(
        std::remove_if(trans_list.begin(), trans_list.end(), [item](const TransLabelStruct & tl){
            return tl.label_ptr == item;
        }),
        trans_list.end()
    );
    trans_cell_list.erase(
        std::remove_if(trans_cell_list.begin(), trans_cell_list.end(), [item](const TransitionStruct & t){
            return t.trans_label == item;
        }),
        trans_cell_list.end()
    );
    item->hide();
    item->deleteLater();
}