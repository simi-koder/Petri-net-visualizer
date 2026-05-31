/**
 * @file engine.cpp
 * @brief Implementation of the Petri net engine classes.
 * @author Patrik Lošťák (xlostap00)
 */
#include "engine.h"
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <ctime>
#include <sstream>

#define MAX_BUFFER_SIZE 1024
#define SEC_IN_MS 1000

PetriNetEngine* g_engine = nullptr;
void (*dump_vars_callback)() = nullptr;
auto global_start_time = std::chrono::steady_clock::now(); // For elapsed time 

long long now() {
    auto current_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(current_time - global_start_time).count();
}

int tokens(const std::string& place_id) {
    if (g_engine) {
        NetPlace* place = g_engine->getPlace(place_id);
        if (place) {
            return place->tokens;
        }
    }
    return 0; // Default to 0 if place not found
}

const char* valueof(const std::string& input_name) {
    if (!g_engine) {
        return "";
    }
    return g_engine->net_inputs[input_name].c_str();
}

bool defined(const std::string& input_name) {
    if (!g_engine) {
        return false;
    }
    // Check if the input name exists in the net_inputs map
    return g_engine->net_inputs.count(input_name) > 0;
}

void output(const std::string& out_name, const std::string& x) {
    if (g_engine) {
        g_engine->set_output(out_name, x);
        g_engine->send_log("Output set: " + out_name + " = " + x);
    }
}

void output(const std::string& out_name, int x) {
    output(out_name, std::to_string(x)); // Convert int to string and call the other overload
}

int elapsed(const std::string& name) {
    if (!g_engine) {
        return 0;
    }
   // try if it is place
   NetPlace* place = g_engine->getPlace(name);
   if (place) {
       return (int)now() - place->last_change_time;
    }
    // check if it is transition
    for (NetTransition* transition : g_engine->transitions) {
        if (transition->id == name && transition->enabled_time != -1) {
            return (int)now() - transition->enabled_time;
        }
    }
    return 0; // Default to 0 if not found
}


NetPlace::NetPlace(std::string name, int init_tokens) : id(name), tokens(init_tokens) {}

void NetPlace::addTokens(int count) {
    tokens += count;
    last_change_time = now(); // Update last change time 
}

void NetPlace::removeTokens(int count) {
    if (tokens >= count) {
        tokens -= count;
        last_change_time = now(); // Update last change time 
    } 
}

NetTransition::NetTransition(std::string name) : id(name), delay_ms(0), check_guard(nullptr), execute_action(nullptr) {}

void NetTransition::addInputArc(NetPlace* place, int weight) {
    input_arcs.push_back({place, weight});
}

void NetTransition::addOutputArc(NetPlace* place, int weight) {
    output_arcs.push_back({place, weight});
}

bool NetTransition::hasEnoughTokens() const {
    for (const auto& arc : input_arcs) {
        if (arc.place->tokens < arc.weight) {
            return false;
        }
    }
    return true;
}

// Fire transition
void NetTransition::fire() {
    enabled_time = -1; // Reset enabled time when firing
    if (!hasEnoughTokens()) {
        std::cerr << "Not enough tokens to fire transition " << id << std::endl;
        return;
    }

    // Remove tokens from input places
    for (const auto& arc : input_arcs) {
        arc.place->removeTokens(arc.weight);
    }
    // Execute action code if defined
    if (execute_action) {
        execute_action();
    }

    // Add tokens to output places
    for (const auto& arc : output_arcs) {
        arc.place->addTokens(arc.weight);
    }
}

// Destructor for PetriNetEngine to clean up
PetriNetEngine::~PetriNetEngine() {
    if (udp_socket != -1) {
        close(udp_socket);
    }

    for (auto place : places) {
        delete place;
    }
    for (auto transition : transitions) {
        delete transition;
    }
}

NetPlace* PetriNetEngine::getPlace(const std::string& id) {
    for (auto place : places) {
        if (place->id == id) {
            return place;
        }
    }
    return nullptr; // Not found
}

bool PetriNetEngine::processStep() {
    std::vector<NetTransition*> to_fire;

    // map for how many tokens are reserved other transitions have reserved
    // avoid conflicts
    std::map<NetPlace*, int> reserved_tokens;

    // Check which transitions can fire
    for (NetTransition* transition : transitions) {
        bool can_fire = true;
        bool has_tokens = true;

        for (const auto& arc : transition->input_arcs) {
            int available_tokens = arc.place->tokens - reserved_tokens[arc.place];

            if (available_tokens < arc.weight) {
                can_fire = false; // Not enough tokens available for this trans
                has_tokens = false;
                break;
            }
        }
            
        if (can_fire && !transition->event_name.empty() && transition->event_name != curr_event) {
            can_fire = false; // Event name doesn't match current event
        }

        // check guard condition
        if (can_fire && transition->check_guard) {
            if (!transition->check_guard()) {
                can_fire = false;
            }
        }

        if (can_fire) {
            if (transition->enabled_time == -1) {
                transition->enabled_time = now(); // Mark when it became enabled
            }

            if (transition->delay_ms == 0) {
                // fire immediately
                send_log("Enabled transition (ready to fire): " + transition->id);
                to_fire.push_back(transition);
                for (const auto& arc : transition->input_arcs) {
                    reserved_tokens[arc.place] += arc.weight;
                }
            } else if (!transition->is_scheduled) {
                // schedule for future firing
                send_log("Pending timeout for transition: " + transition->id + " (delay " + std::to_string(transition->delay_ms) + " ms)");
                transition->is_scheduled = true;
                transition->fire_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(transition->delay_ms);
                for (const auto& arc : transition->input_arcs) {
                    reserved_tokens[arc.place] += arc.weight;
                }
            }
        } else {
            // If transition is not enabled, reset its enabled time
            transition->enabled_time = -1;
        }
        // Update transition color based on state
        std::string new_color = "darkGray";
        if (transition->is_scheduled) {
            new_color = "orange"; // pending
        } else if (has_tokens) {
            new_color = "green"; // ready
        }
        // Only send color update if it changed
        if (transition->curr_color != new_color) {
            transition->curr_color = new_color;
            send_trans_color(transition->id, new_color);
        } 
    }

    // not found, net is waiting
    if (to_fire.empty()) {
        return false;
    }

    // Rdy to fire
    for (NetTransition* transition : to_fire) {
        transition->fire();
        send_log("Fired transition: " + transition->id);
        if (!transition->event_name.empty() && transition->event_name == curr_event) {
            curr_event = ""; // Clear event after firing a transition that was waiting for it
        }
        // update GUI about state changes
        for (const auto& arc : transition->input_arcs) {
            send_state_update(arc.place->id, arc.place->tokens);
        }

        for (const auto& arc : transition->output_arcs) {
            send_state_update(arc.place->id, arc.place->tokens);
        }
        // Dump variable values for debugging after each step
        if (dump_vars_callback) {
            dump_vars_callback(); 
        }
    }

    // Net changed, return true
    return true;
}

void PetriNetEngine::run_engine() {
    bool stop = false;
    while (!stop) {
        // Process a step of the net
        while (processStep()) {
            // Keep firing xd
        }
        curr_event = ""; // Clear after processing
        usleep(10000);
        for (NetPlace* place : places) {
            send_state_update(place->id, place->tokens);
        }

        // Passively wait
        WakeupReason reason = wait_passively();

        // Handle wakeup reason
        if (reason == TIMEOUT_EXPIRED) {
            // find out whose delay expired and fire it
            auto now = std::chrono::steady_clock::now();
            for (NetTransition* transition : transitions) {
                if (transition->is_scheduled && transition->fire_time <= now) {
                    transition->is_scheduled = false; // unschedule
                    transition->fire(); // fire the transition
                    transition->curr_color = "darkGray"; // reset color
                    send_trans_color(transition->id, "darkGray");
                    send_log("Fired delayed transition: " + transition->id);
                    // update GUI about state changes
                    for (const auto& arc : transition->input_arcs) {
                        send_state_update(arc.place->id, arc.place->tokens);
                    }

                    for (const auto& arc : transition->output_arcs) {
                        send_state_update(arc.place->id, arc.place->tokens);
                    }

                    if (dump_vars_callback) {
                    dump_vars_callback(); 
                    }
                }
            }
        } else if (reason == EXTERNAL_EVENT) {
            // GUI sent some event, process it and update state
            process_ext_event();
        } else if (reason == TERMINATE) {
            stop = true;
        }
    }
}

WakeupReason PetriNetEngine::wait_passively() {
    auto now = std::chrono::steady_clock::now();
    long long time_to_next_timeout = -1; // -1 --> no timeouts scheduled

    // Find the soonest transition that is scheduled to fire
    for (const auto& transition : transitions) {
        if (transition->is_scheduled) {
            auto time_left = std::chrono::duration_cast<std::chrono::milliseconds>(transition->fire_time - now).count();
            if (time_left < 0) {
                time_left = 0; // Already expired
            }
            if (time_to_next_timeout == -1 || time_left < time_to_next_timeout) {
                time_to_next_timeout = time_left;
            }
        }
    }

    // Set up select for passive waiting
    struct timeval tv;
    struct timeval* tv_ptr = nullptr;
    if (time_to_next_timeout != -1) {
        tv.tv_sec = time_to_next_timeout / SEC_IN_MS;
        tv.tv_usec = (time_to_next_timeout % SEC_IN_MS) * SEC_IN_MS;
        tv_ptr = &tv;
    }

    // prepare sockets
    fd_set read_fds;
    FD_ZERO(&read_fds);

    int max_fd = 0;
    if (udp_socket != -1) {
        FD_SET(udp_socket, &read_fds);
        max_fd = udp_socket;
    }

    std::cout << "[LOG] Waiting passively for events or timeout..." << std::endl;

    // Passively wait
    int retval = select(max_fd + 1, &read_fds, NULL, NULL, tv_ptr);

    if (retval == -1) {
        return TERMINATE; // On error, terminate the engine
    } else if (retval == 0) {
        std::cout << "[LOG] Timeout expired." << std::endl;
        return TIMEOUT_EXPIRED; // Timeout occurred
    } else {
        // select returned > 0, data on UDP socket
        if (udp_socket != -1 && FD_ISSET(udp_socket, &read_fds)) {
            std::cout << "[LOG] External event received." << std::endl;
            return EXTERNAL_EVENT; // External event received
        }
    }
    return TERMINATE; // Fallback
}

void PetriNetEngine::process_ext_event() {
    if (udp_socket == -1) {
        return;
    }

    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);

    // read data from UDP socket
    ssize_t bytes_recved = recvfrom(udp_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&sender_addr, &sender_len);
    if (bytes_recved > 0) {
        buffer[bytes_recved] = '\0'; // terminate string
        std::string event_data(buffer);
        send_log("Received external event: " + event_data);
        // parse event_data and update state
        if (event_data.find("EVENT:") == 0) {
            curr_event = event_data.substr(6); 
            send_log("Current event set to: " + curr_event);
        } else if (event_data.find("INJECT:") == 0) {
            // format: "INJECT:CELL_1:5"
            std::string payload = event_data.substr(7);
            size_t first_colon = payload.find(":");
            if (first_colon != std::string::npos) {
                std::string place_id = payload.substr(0, first_colon);
                int new_tokens = std::stoi(payload.substr(first_colon + 1));

                NetPlace* place = getPlace(place_id);
                if (place) {
                    place->tokens = new_tokens; // directly set tokens to new value
                    send_log("Injected " + std::to_string(new_tokens) + " tokens into " + place_id);
                    send_state_update(place_id, new_tokens); // update GUI
                }
            }
        } else if (event_data == "STOP") {
            exit(0); // Terminate
        } else if (event_data == "PING") {
            // send back a PONG response
            std::string reply = "PONG:" + (name.empty() ? "Vychozi sit" : name);
            struct sockaddr_in gui_addr;
            std::memset(&gui_addr, 0, sizeof(gui_addr));
            gui_addr.sin_family = AF_INET;
            gui_addr.sin_port = htons(8081); // GUI listening port
            gui_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Localhost
            sendto(udp_socket, reply.c_str(), reply.size(), 0, (struct sockaddr*)&gui_addr, sizeof(gui_addr));

            // Also send all states of tokens in places for GUI to update
            for (const auto& place : places) {
                send_state_update(place->id, place->tokens);
            }
        } else if (event_data.find("SET_INPUT:") == 0) {
            // format: "SET_INPUT:input_name:value"
            std::string payload = event_data.substr(10);
            size_t first_colon = payload.find(":");
            if (first_colon != std::string::npos) {
                std::string input_name = payload.substr(0, first_colon);
                std::string value = payload.substr(first_colon + 1);
                net_inputs[input_name] = value;
                send_log("Input set: " + input_name + " = " + value);
            }
        }
    }
}

bool PetriNetEngine::init_udp_socket(int port) {
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        send_log("Failed to create UDP socket.");
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(udp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        send_log("Failed to bind UDP socket to port " + std::to_string(port));
        close(udp_socket);
        udp_socket = -1;
        return false;
    }

    send_log("UDP socket initialized on port " + std::to_string(port));
    return true;
}

void PetriNetEngine::send_state_update(const std::string& place_id, int tokens) {
    if (udp_socket == -1) {
        return;
    }
    std::string update_msg = "STATE_UPDATE:" + place_id + ":" + std::to_string(tokens);

    struct sockaddr_in gui_addr;
    std::memset(&gui_addr, 0, sizeof(gui_addr));
    gui_addr.sin_family = AF_INET;
    gui_addr.sin_port = htons(8081); // GUI listening port
    gui_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Localhost

    sendto(udp_socket, update_msg.c_str(), update_msg.size(), 0, (struct sockaddr*)&gui_addr, sizeof(gui_addr));
}

void PetriNetEngine::send_log(const std::string& message) {
    if (udp_socket == -1) {
        return;
    }
    // Timestamp the log message
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");

    std::string log_msg = "LOG:[" + ss.str() + "] " + message;

    struct sockaddr_in gui_addr;
    std::memset(&gui_addr, 0, sizeof(gui_addr));
    gui_addr.sin_family = AF_INET;
    gui_addr.sin_port = htons(8081); // GUI listening port
    gui_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Localhost

    sendto(udp_socket, log_msg.c_str(), log_msg.size(), 0, (struct sockaddr*)&gui_addr, sizeof(gui_addr));
}

void PetriNetEngine::send_trans_color(const std::string& trans_id, const std::string& color) {
    if (udp_socket == -1) {
        return;
    }
    std::string color_msg = "TRANS_COLOR:" + trans_id + ":" + color;

    struct sockaddr_in gui_addr;
    std::memset(&gui_addr, 0, sizeof(gui_addr));
    gui_addr.sin_family = AF_INET;
    gui_addr.sin_port = htons(8081); // GUI listening port
    gui_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Localhost
    sendto(udp_socket, color_msg.c_str(), color_msg.size(), 0, (struct sockaddr*)&gui_addr, sizeof(gui_addr));
}