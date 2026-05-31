/**
 * @file tools.h
 * @author Šimon Čorej (xcorejs00)
 * @brief Header file for tools.cpp, shows draggable widgets in right dock widget with add transition and delete option
 */
#pragma once

#include "globals.h"
#include "labels.h"

/**
 * @brief Shows draggable widgets in right dock widget with add transition and delete option
 * 
 * @param dock_right Right dock widget contains sidebar_widget
 * @param sidebar_widget Sidebar_widget contains layout
 * @param layout Layout contains all labels nad buttons
 * @param canvas Main drop area
 * @param visible Boolean if dock is visible
 */
void show_tools(QDockWidget * dock_right, QWidget * sidebar_widget, QVBoxLayout * layout, QWidget * canvas, bool visible);