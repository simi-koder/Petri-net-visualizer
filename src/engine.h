/**
 * @file engine.h
 * @brief Header file for the Petri net engine classes.
 * @author Patrik Lošťák (xlostap00)
 */

#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

class NetPlace;

// Forward declar cause of compiler
class PetriNetEngine;
extern PetriNetEngine* g_engine; // Global pointer to the engine for logging from generated code
extern void (*dump_vars_callback)(); 

// Builtin functions for inscribed code to interact with the engine
long long now();
int tokens(const std::string& place_id);
const char* valueof(const std::string& input_name);
bool defined(const std::string& input_name);
void output(const std::string& out_name, const std::string& x);
void output(const std::string& out_name, int x); // Overload for integer output
int elapsed(const std::string& name);

/**
 * @brief Struct representing arc in the Petri net, con place to trans
 */
struct NetArc {
    NetPlace* place;
    int weight;
};

/**
 * @brief Class representing a place in the Petri net
 */
class NetPlace {
public:
    std::string id;
    int tokens;
    long long last_change_time = 0;
    std::string action_code;

    /**
     * @brief Construct a new Net Place object  
     * @param name identifier of the place
     * @param init_tokens starting number of tokens in the place
     */
    NetPlace(std::string name, int init_tokens);

    /**
     * @brief Add tokens to the place
     * @param count how many tokens to add
     */
    void addTokens(int count);
    /**
     * @brief Remove tokens from the place
     * @param count how many tokens to remove
     */
    void removeTokens(int count);
};

/**
 * @brief Class representing a transition in the Petri net
 */
class NetTransition {
public:
    std::string id;
    long long enabled_time = -1;
    std::vector<NetArc> input_arcs;
    std::vector<NetArc> output_arcs;

    std::string event_name;
    std::string guard_code;
    int delay_ms;
    bool is_scheduled = false; // For handling delayed transitions
    std::chrono::time_point<std::chrono::steady_clock> fire_time; // When the transition should fire after delay
    std::string action_code;
    std::string curr_color;

    std::function<bool()> check_guard = []() { return true; }; // Default returns true
    std::function<void()> execute_action = []() {}; // Default do nothing 

    /**
     * @brief Construct a new Net Transition object
     * @param name identifier of the transition
     */
    NetTransition(std::string name);
    /**
     * @brief Add an input arc from a place to this transition
     * @param place pointer to the input place
     * @param weight weight of the arc (number of tokens required)
     */
    void addInputArc(NetPlace* place, int weight);
    /**
     * @brief Add an output arc from this transition to a place
     * @param place pointer to the output place
     * @param weight weight of the arc (number of tokens produced)
     */
    void addOutputArc(NetPlace* place, int weight);
    /**
     * @brief Check if the transition has enough tokens in its input places to fire
     * @return true if it can fire, false otherwise
     */
    bool hasEnoughTokens() const;
    /**
     * @brief Fire the transition: remove tokens from input places and add tokens to output places
     */
    void fire();
};

enum WakeupReason {
    TIMEOUT_EXPIRED,
    EXTERNAL_EVENT,
    TERMINATE
};

/**
 * @brief Class representing the Petri net engine, which manages places and transitions
 */
class PetriNetEngine {
public:
    std::string name;
    std::vector<NetPlace*> places;
    std::vector<NetTransition*> transitions;
    std::map<std::string, std::string> net_inputs;
    std::map<std::string, std::string> net_outputs;

    std::string get_input(const std::string& name) {
        return net_inputs[name];
    }
    /**
     * @brief Destroy the Petri Net Engine object and clean up
     */
    ~PetriNetEngine();
    /** 
     * @brief Get a place by its identifier
     * @param id identifier of the place to retrieve
     * @return pointer to the NetPlace with the given id, or nullptr if not found
    */
    NetPlace* getPlace(const std::string& id);

        /**
        * @brief Run the Petri net engine
        */
    void run_engine();
    /**
     * @brief Initialize the UDP socket for receiving external events
     * @param port the port number to bind the socket to
     * @return true if the socket was successfully initialized, false otherwise
     */
    bool init_udp_socket(int port);
    /**
     * @brief Set an output variable in the net
     * @param name the name of the output variable
     * @param value the value to set for the output variable
     */
    void set_output(const std::string& name, const std::string& value) {
        net_outputs[name] = value;
    }
    /**
     * @brief Send a state update to the GUI
     * @param place_id the identifier of the place whose state is being updated
     * @param tokens the current number of tokens in that place
     */
    void send_state_update(const std::string& place_id, int tokens);
    /**
     * @brief Send a log message to the GUI
     * @param message the log message to send
     */
    void send_log(const std::string& message);
    /**
     * @brief Send a transition color update to the GUI
     * @param trans_id the identifier of the transition whose color is being updated
     * @param color the new color to set for the transition ("orange", "green", "darkGray")
     */
    void send_trans_color(const std::string& trans_id, const std::string& color);

private:
    int udp_socket = -1; // Socket for external events
    std::string curr_event = ""; // Current event being processed
    /**
     * @brief Process a single step of the Petri net by firing all enabled transitions
     * @return true if at least one transition was fired, 
     * @return false if no transitions could be fired
     */
    bool processStep();
    /**
     * @brief Wait passively for the next event or timeout
     * @return WakeupReason indicating why the engine woke up
     */
    WakeupReason wait_passively();
    /**
     * @brief Process an external event, updating the state
     */
    void process_ext_event();
};

