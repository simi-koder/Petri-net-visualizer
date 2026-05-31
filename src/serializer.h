/**
 * @file serializer.h
 * @author Patrik Lošťák (xlostap00)
 * @brief Header file for the Serializer, which handles saving and loading of the application's state.
*/
#pragma once
#include <QString>

/** 
* @brief Saves the current state of the net to a file specified by file_path.
* @param file_path The path to the file where the net state should be saved.
* @return True if the net was successfully saved, false otherwise.
*/
bool save_net_to_file(const QString& file_path);