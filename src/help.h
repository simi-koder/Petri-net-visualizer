/**
 * @file help.h
 * @author Šimon Čorej (xcorejs00)
 * @brief Header file for help.cpp
 */
#pragma once
#include "globals.h"

/**
 * @brief Function shows instructions on right dock widget
 * 
 * @param dock_right Right dock widget containing sidebar
 * @param sidebar_widget Sidebar containing layout
 * @param layout Layout containing all widgets
 * @param visible Boolean if dock_right is visible
 */
void show_help(QDockWidget * dock_right, QWidget * sidebar_widget, QVBoxLayout * layout, bool visible);