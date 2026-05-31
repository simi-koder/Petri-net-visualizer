/**
 * @file globals.cpp
 * @author Šimon Čorej (xcorejs00)
 * @brief Initialization of lists and booleans defined in globals.h and default cell and transition style implementation
 */
#include "globals.h"

/**
 * @brief Default cell label style
 * 
 */
QString cell_style =    "border-radius: 50px;"
                        "border: 2px solid black;"
                        "min-width: 100px;"
                        "min-height: 100px;"
                        "max-width: 100px;"
                        "max-height: 100px;"
                        "text-align: center;" ;

/**
 * @brief Default transition label style
 * 
 */
QString trans_style =   "border: 2px solid black;"
                        "min-width: 40px;"
                        "min-height: 100px;"
                        "max-width: 20px;"
                        "max-height: 100px;"
                        "text-align: center;" ;

bool label_click_allowed = false;
bool delete_allowed = false;
bool curr_direction = true;

std::vector<TransLabelStruct> trans_list = {};
std::vector<TransitionStruct> trans_cell_list = {};
std::vector<QLabel*> curr_selected_trans = {};

std::vector<QLabel*> curr_selected_cells = {};
std::vector<CellStruct> cell_list = {};

std::vector<click_label * > placed_labels;
std::vector<click_trans_label * > placed_trans_labels;
std::vector<LineStruct> placed_lines = {};

ConfigData * config_data = new ConfigData();