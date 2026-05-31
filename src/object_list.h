/**
 * @file object_list.h
 * @author Šimon Čorej (xcorejs00)
 * @brief Header file for object_list.cpp
 */
#pragma once

#include "globals.h"
#include "labels.h"

/**
 * @brief Shows list of all objects and entry labels for adding different properties
 * 
 * @param dock_left Left dock widget
 * @param layout Layout of dock widget
 * @param visible Boolean if dock widget is visible
 */
void show_list(QWidget * dock_left, QVBoxLayout * layout, drop_widget * canvas, bool visible);