/** 
 * @file loader.h
 * @author Filip Kachyňa (xkachyf00)
 * @brief Header file for the Loader.
*/

#pragma once
#include "globals.h"
#include "parser.h"
#include "labels.h"

/**
 * @brief Function for loading the net into GUI based on parsed data.
 */
void load_net_into_gui(QMainWindow &window, drop_widget* canvas, NetParser &parser);