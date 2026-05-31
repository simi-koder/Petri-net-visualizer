/**
 * @file generator.cpp
 * @brief Code generator for the Petri net engine based on the parsed configuration.
 * @author Patrik Lošťák (xlostap00)
 */

#include "parser.h"
#include "globals.h"
#include <fstream>
#include <iostream>

bool generate_engine_code(const NetParser& parser, const QString& output_path) {
    std::ofstream out(output_path.toStdString());
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << output_path.toStdString() << std::endl;
        return false;
    }

    // Write includes
    out << "// Generated Petri net engine code\n";
    out << "#include \"engine.h\"\n";
    out << "#include <iostream>\n\n";

    // Insert global vars
    out << "// User variables\n";
    if (config_data) {
        config_data->variables.removeDuplicates();
        for (const QString& var : config_data->variables) {
            out << var.toStdString() << "\n";
        }
    }
    out << "\nvoid dump_vars() {\n";
    if (config_data) {
        for (const QString& var : config_data->variables) {
            QString var_name = var.split("=").first().trimmed().split(" ").last().trimmed();
            if (!var_name.isEmpty()) {
                out << "    if (g_engine) g_engine->send_log(\"VAR " << var_name.toStdString() << " = \" + std::to_string(" << var_name.toStdString() << "));\n";
            }
        }
    }
    out << "}\n\n";



    // Generate guards for transitions
    out << "// Condition guard\n";
    for (const auto& trans : parser.parsed_transitions) {
        out << "bool guard_" << trans.name.toStdString() << "() {\n";
        if (trans.guard.isEmpty()) {
            out << "    return true;\n";
        } else {
            out << "    return " << trans.guard.toStdString() << ";\n";
        }
        out << "}\n\n";
    }

    // Generate actions for transitions
    out << "// Transition actions\n";
    for (const auto& trans : parser.parsed_transitions) {
        out << "void action_" << trans.name.toStdString() << "() {\n";
        if (!trans.action.isEmpty() && trans.action != "{ }") {
            out << "    " << trans.action.toStdString() << "\n";
        }
        out << "}\n\n";
    }

    // Generate engine setup code
    out << "// Engine setup\n";
    out << "int main() {\n";
    out << "    dump_vars_callback = dump_vars;\n";
    out << "    PetriNetEngine engine;\n";
    out << "    g_engine = &engine;\n";
    out << "    engine.name = \"" << (config_data ? config_data->net_name.toStdString() : "Vychozi_Sit") << "\";\n\n";

    // Add cells
    for (const auto& cell : parser.parsed_cells) {
        out << "    engine.places.push_back(new NetPlace(\"" << cell.name.toStdString() << "\", " << cell.tokens << "));\n";
    }
    out << "\n";

    // Add transitions
    for (const auto& trans : parser.parsed_transitions) {
        std::string raw_name = trans.name.toStdString(); // "1"
        std::string t_name = "T_" + raw_name; // "T_1"
        out << "    NetTransition* " << t_name << " = new NetTransition(\"" << t_name << "\");\n";

        // map guard and action functions
        out << "    " << t_name << "->check_guard = guard_" << raw_name << ";\n";
        out << "    " << t_name << "->execute_action = action_" << raw_name << ";\n";
        out << "    " << t_name << "->event_name = \"" << trans.event_name.toStdString() << "\";\n";
        out << "    " << t_name << "->delay_ms = " << trans.delay << ";\n";

        // Add input arcs
        for (const auto& input : trans.inputs) {
            out << "    " << t_name << "->addInputArc(engine.getPlace(\"" << input.place_name.toStdString() << "\"), " << input.weight << ");\n";
        }
        // Add output arcs
        for (const auto& output : trans.outputs) {
            out << "    " << t_name << "->addOutputArc(engine.getPlace(\"" << output.place_name.toStdString() << "\"), " << output.weight << ");\n";
        }
        out << "    engine.transitions.push_back(" << t_name << ");\n\n";
    }

    // Run the engine
    out << "    if (engine.init_udp_socket(8080)) {\n";
    out << "        engine.run_engine();\n";
    out << "    }\n";
    out << "    return 0;\n";
    out << "}\n";

    out.close();
    return true;
}