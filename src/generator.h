/** 
 * @file generator.h
 * @brief Header file for the code generator
 * @author Patrik Lošťák (xlostap00)
 */

#pragma once
#include "parser.h"
#include <QString>

/**
 * @brief Generate C++ code for the Petri net engine based on the parsed configuration
 * @param parser reference to the parser containing the parsed net configuration
 * @param output_path path to save the generated code file
 * @return true if generation was successful, false otherwise
 */
bool generate_engine_code(const NetParser& parser, const QString& output_path);