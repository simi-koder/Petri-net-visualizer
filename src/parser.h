/** 
 * @file parser.h
 * @author Filip Kachyňa (xkachyf00)
 * @brief Header file for the Parser.
*/

#pragma once
#include <string>
#include <vector>
#include <QString>
#include <QStringList>

/** 
 * @brief Class for storing a place.
 * 
*/
class CellParsed {
    public:
        QString name;
        int tokens;
        QString action;
};

/** 
 * @brief Class for storing a transition.
 * 
*/
class TransitionParsed {
    public:
        struct Arc {
            QString place_name;
            int weight;
        };

        QString name;
        std::vector<Arc> inputs;
        std::vector<Arc> outputs;
        QString event_name;
        QString guard;
        int delay;
        QString delay_var;
        QString action;
};

/**
 * @brief Parser FSM states.
 */
enum class State {
    None,
    Name,
    Comment,
    Inputs,
    Outputs,
    Variables,
    Places,
    Transitions
};

/**
 * @brief Class responsible for parsing the configuration file and loading the net.
 * 
 */
class NetParser {
    public:
        /**
         * @brief Parsed cells.
         */
        std::vector<CellParsed> parsed_cells;

        /**
         * @brief Parsed transitions.
         */
        std::vector<TransitionParsed> parsed_transitions;

        /**
         * @brief Error log for storing parsing errors.
         */
        QStringList error_log;

        /**
         * @brief Parses the configuration file and loads the net.
         * 
         * @param file_path The path to the configuration file.
         * @return True if the parsing was successful, false otherwise.
         */
        bool parse(const QString& file_path);
};
