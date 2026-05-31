/** 
 * @file parser.cpp
 * @author Filip Kachyňa (xkachyf00)
 * @brief Implementation of the Parser, which loads the net from a configuration file.
*/

#include "parser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include "globals.h"
#include <map>

bool NetParser::parse(const QString& file_path) {
    // Open the file for reading
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error_log.append(QString("Error: Unable to open file '%1'").arg(file_path));
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    // Check for required sections
    map<State, bool> checklist = {
        {State::Name, false},
        {State::Comment, false},
        {State::Inputs, false},
        {State::Outputs, false},
        {State::Variables, false},
        {State::Places, false},
        {State::Transitions, false}
    };

    // Line number for error reporting
    int line_number = 0;

    State state = State::None;

    // Read the file line by line
    while(!in.atEnd()) {
        // Read the line and trim whitespace
        QString line = in.readLine().trimmed();
        line_number++;

        if (line.isEmpty()) {
            continue;
        }

        // Check for section headers
        if ((line.startsWith("Jmeno site")|| line.startsWith("Jméno sítě") || line.startsWith("Nazev site") || line.startsWith("Název sítě") || line.startsWith("Name") || line.startsWith("Net name")) && line.endsWith(":")) {
            state = State::Name;
            checklist[State::Name] = true;
            continue;
        } else if ((line.startsWith("Komentar") || line.startsWith("Komentář") || line.startsWith("Comment") || line.startsWith("comment")) && line.endsWith(":")) {
            state = State::Comment;
            checklist[State::Comment] = true;
            continue;
        } else if ((line.startsWith("Vstupy") || line.startsWith("Inputs") || line.startsWith("inputs") || line.startsWith("start_place")) && line.endsWith(":")) {
            state = State::Inputs;
            checklist[State::Inputs] = true;
            continue;
        } else if ((line.startsWith("Vystupy") || line.startsWith("Výstupy") || line.startsWith("Outputs") || line.startsWith("outputs") || line.startsWith("end_place")) && line.endsWith(":")) {
            state = State::Outputs;
            checklist[State::Outputs] = true;
            continue;
        } else if ((line.startsWith("Promenne") || line.startsWith("Proměnné") || line.startsWith("Variables") || line.startsWith("variables")) && line.endsWith(":")) {
            state = State::Variables;
            checklist[State::Variables] = true;
            continue;
        } else if ((line.startsWith("Mista") || line.startsWith("Místa") || line.startsWith("Places") || line.startsWith("places")) && line.endsWith(":")) {
            state = State::Places;
            checklist[State::Places] = true;
            continue;
        } else if ((line.startsWith("Prechody") || line.startsWith("Přechody") || line.startsWith("Transitions") || line.startsWith("transitions")) && line.endsWith(":")) {
            state = State::Transitions;
            checklist[State::Transitions] = true;
            continue;
        } else if (line.startsWith("#")) {
            continue;
        }

        // Process the line based on the current state
        switch(state) {
            // Store net name
            case State::Name: {
                config_data->net_name = line;
                break;
            }

            // Store comment
            case State::Comment: {
                config_data->comment = line;
                break;
            }

            // Store inputs
            case State::Inputs: {
                if (line.toLower() == "none" || line.toLower() == "zadne" || line.toLower() == "žádné") {
                    state = State::None;
                    break;
                }
                CellStruct * start_cell = new CellStruct();
                start_cell->label_text = line;
                config_data->start_places.push_back(start_cell);
                break;
            }

            // Store outputs
            case State::Outputs: {
                if (line.toLower() == "none" || line.toLower() == "zadne" || line.toLower() == "žádné") {
                    state = State::None;
                    break;
                }
                CellStruct * end_cell = new CellStruct();
                end_cell->label_text = line;
                config_data->end_places.push_back(end_cell);
                break;
            }

            // Store variables
            case State::Variables: {
                if (line.toLower() == "none" || line.toLower() == "zadne" || line.toLower() == "žádné") {
                    state = State::None;
                    break;
                }
                config_data->variables.append(line);
                break;
            }

            // Store places (Cells)
            case State::Places: {
                QRegularExpression place_regex(R"(^\s*(\w+)\s*\((\d+)\)\s*:\s*\{\s*(.*?)\s*\}$)");
                QRegularExpressionMatch place_match = place_regex.match(line);
                if (place_match.hasMatch()) {
                    CellParsed cell;
                    cell.name = place_match.captured(1);
                    cell.tokens = place_match.captured(2).toInt();
                    cell.action = place_match.captured(3);
                    parsed_cells.push_back(cell);
                } else {
                    error_log.append(QString("Line %1: Invalid place format").arg(line_number));
                }
                break;
            }

            // Store transitions
            case State::Transitions: {
                TransitionParsed transition;
                transition.delay = 0; // default delay is 0

                // Read transition name
                QRegularExpression trans_name_regex(R"(^\s*(?:T_)?(\w+)\s*:$)");
                QRegularExpressionMatch name_match = trans_name_regex.match(line);
                if (name_match.hasMatch()) {
                    transition.name = name_match.captured(1);
                } else {
                    error_log.append(QString("Line %1: Invalid transition name format").arg(line_number));
                }

                // Read transition inputs
                line = in.readLine().trimmed();
                line_number++;
                while (line.isEmpty() && !in.atEnd()) {
                    line = in.readLine().trimmed();
                    line_number++;
                }
                QRegularExpression trans_in_regex(R"(^\s*in:\s*(.*)$)");
                QRegularExpressionMatch in_match = trans_in_regex.match(line);
                if (in_match.hasMatch()) {
                    QString trans_ins = in_match.captured(1);
                    QStringList inputs = trans_ins.split(",", Qt::SkipEmptyParts);

                    for (const QString& input : inputs) {
                        QRegularExpression input_regex(R"(^\s*(\w+)\s*\*\s*(\d+)$)");
                        QRegularExpressionMatch input_match = input_regex.match(input.trimmed());
                        if (input_match.hasMatch()) {
                            TransitionParsed::Arc parsed_in;
                            parsed_in.place_name = input_match.captured(1).trimmed();
                            parsed_in.weight = input_match.captured(2).toInt();
                            transition.inputs.push_back(parsed_in);
                        } else {
                            error_log.append(QString("Line %1: Invalid transition input format").arg(line_number));
                        }
                    }
                } else {
                    error_log.append(QString("Line %1: Missing or invalid transition inputs").arg(line_number));
                }

                // Read transition outputs
                line = in.readLine().trimmed();
                line_number++;
                while (line.isEmpty() && !in.atEnd()) {
                    line = in.readLine().trimmed();
                    line_number++;
                }
                QRegularExpression trans_out_regex(R"(^\s*out:\s*(.*)$)");
                QRegularExpressionMatch out_match = trans_out_regex.match(line);
                if (out_match.hasMatch()) {
                    QString trans_outs = out_match.captured(1);
                    QStringList outputs = trans_outs.split(",", Qt::SkipEmptyParts);

                    for (const QString& output : outputs) {
                        QRegularExpression output_regex(R"(^\s*(\w+)\s*\*\s*(\d+)$)");
                        QRegularExpressionMatch output_match = output_regex.match(output.trimmed());
                        if (output_match.hasMatch()) {
                            TransitionParsed::Arc parsed_out;
                            parsed_out.place_name = output_match.captured(1).trimmed();
                            parsed_out.weight = output_match.captured(2).toInt();
                            transition.outputs.push_back(parsed_out);
                        } else {
                            error_log.append(QString("Line %1: Invalid transition output format").arg(line_number));
                        }
                    }
                } else {
                    error_log.append(QString("Line %1: Missing or invalid transition outputs").arg(line_number));
                }

                // Read transition condition
                line = in.readLine().trimmed();
                line_number++;
                while (line.isEmpty() && !in.atEnd()) {
                    line = in.readLine().trimmed();
                    line_number++;
                }
                QRegularExpression trans_cond_regex(R"(^\s*when:\s*(.*)\s*$)");
                QRegularExpressionMatch cond_match = trans_cond_regex.match(line);
                if (cond_match.hasMatch()) {
                    QRegularExpression condition_regex(R"(^\s*([^\[@]*?)\s*(?:\[\s*(.*?)\s*\])?\s*(?:@\s*(.*))?$)");
                    QRegularExpressionMatch condition_match = condition_regex.match(cond_match.captured(1));
                    if (condition_match.hasMatch()) {
                        transition.event_name = condition_match.captured(1).trimmed();
                        if (!condition_match.captured(2).isEmpty()) {
                            transition.guard = condition_match.captured(2).trimmed();
                        }
                        if (!condition_match.captured(3).isEmpty()) {
                            bool delay_is_num;
                            int delay_value = condition_match.captured(3).toInt(&delay_is_num);
                            if (!delay_is_num) {
                                transition.delay_var = condition_match.captured(3).trimmed();
                            } else {
                                transition.delay = delay_value;
                            }
                        }
                    } else {
                        error_log.append(QString("Line %1: Invalid transition condition format").arg(line_number));
                    }
                } else {
                    error_log.append(QString("Line %1: Missing or invalid transition condition").arg(line_number));
                }

                // Read transition action
                line = in.readLine().trimmed();
                line_number++;
                while (line.isEmpty() && !in.atEnd()) {
                    line = in.readLine().trimmed();
                    line_number++;
                }
                QRegularExpression trans_action_regex(R"(^\s*do:\s*(.*)\s*$)");
                QRegularExpressionMatch action_match = trans_action_regex.match(line);
                if (action_match.hasMatch()) {
                    QRegularExpression action_regex(R"(^\s*(?:\{\s*(.*?)\s*\})?\s*$)");
                    QRegularExpressionMatch action_match_inner = action_regex.match(action_match.captured(1).trimmed());
                    if (action_match_inner.hasMatch()) {
                        transition.action = action_match_inner.captured(1);
                    } else {
                        error_log.append(QString("Line %1: Invalid transition action format").arg(line_number));
                    }
                } else {
                    error_log.append(QString("Line %1: Missing or invalid transition action").arg(line_number));
                }

                parsed_transitions.push_back(transition);
                break;
            }

            // No state
            case State::None: {
                break;
            }

            // Unknown state
            default: {
                error_log.append(QString("Line %1: Unknown state").arg(line_number));
                break;
            }
        }
    }
    
    // Check if all required sections were found
    for (const auto& [section, found] : checklist) {
        if (!found) {
            error_log.append(QString("Missing required section: %1").arg(
                [section]() {
                    switch(section) {
                        case State::Name: return "Name";
                        case State::Comment: return "Comment";
                        case State::Inputs: return "Inputs";
                        case State::Outputs: return "Outputs";
                        case State::Variables: return "Variables";
                        case State::Places: return "Places";
                        case State::Transitions: return "Transitions";
                        default: return "Unknown";
                    }
                }()
            ));
        }
    }

    // If there were any errors, return false
    if (!error_log.isEmpty()) {
        return false;
    }

    // Close the file and return true
    file.close();
    return true;
}