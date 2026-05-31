/**
 * @file edit_config.h
 * @author Šimon Čorej (xcorejs00)
 * @brief Header file for config file edit
 */
#pragma once

#include "globals.h"

/**
 * @brief Function shows configurable data on left dock-widget
 * @param dock_left Left widget
 * @param layout_left Layout of left widget
 * @param visible Boolean dock_left visible?
 * 
 */
void edit_config(QWidget * dock_left, QVBoxLayout * layout_left, bool visible);