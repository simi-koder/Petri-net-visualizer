<div align=center>

# <b>Petri net</b> - visualization

### Table of contents
</div>

#### - [Introduction](#Introduction)
- [Brief explanation](#brief-explanation)
- [UML](#uml)

#### - [Program section](#program-section)
- [GUI](#GUI)
    - [Introduction](#introduction-1)
        - [Structure](#structure)
        - [Examples](#examples)
    - [Monitoring](#Monitoring)

- [Code generator](#Code-generator)
    - [Introduction](#introduction-2)
    - [Structure](#structure-1)
    - [Examples](#examples-)

- [Configuration file](#Configuration-file)

#### - [Miscellaneous](#miscellaneous)
- [Authors](#Authors)
- [Usage](#Usage)
- [Constraints & abilities](#constraints--abilities)
- [Documentation](#documentation)
---



<div align=center>

## Introduction
</div>


> This application is used for visualization and editing Petri net. <br>
> GUI allows creating, loading and saving different configurations, token injection and monitoring.

### Brief explanation
> Petri net visualizes how tokens travel through configuration in real time.

#### Contents:
- Places:
    > Cells with "x" amount of tokens (tokens don't have any real value).
- Transitions:
    > Objects with attributes such as delay (time in ms to "fire"), guard/action code and event name.
    > Transitions "fire" and pull tokens from places and push them to others connected by arcs
- Arcs:
    > Lines connecting places and transitions. <br>
    > They can have weight atribute to specify how many tokens can travel through them.

> After running configuration, transitions fire and transfer tokens to different places. <br>
> Changes in number of tokens can be all seen in real time and monitor shows detailed information about each token transfer.

<div align=center>

### UML

<u>[**--click to view--**](/misc/ICP-diagram.pdf)</u>

</div>

<div align=center>

## Program section
</div>

> This section explains how each part was implemented, what it does and its functionality.

<div align=center>

## GUI

### Introduction
</div>

> This part shows in brief how GUI was created, its structure, functionality and examples.<br>
> GUI is responsible for creating configurations, loading data for engine and finally showing what happens in real time. <br>
>
> Version : Qt5

### Structure

- <b>Files</b>

    - <b>globals.h/cpp</b>
        > Declares all libraries, variables used by all functions, defines structures for places transitions and arcs.

    - <b>labels.h/cpp</b>
        > Implementation of clickable labels, lines and drag-n-drop widgets.

    - <b>tools.h/cpp</b>
        > Shows draggable labels, buttons to add transitions and delete button.

    - <b>object_list.h/cpp</b>
        > Show list of objects (places, transitions, arcs) and allows inserting properties into them.

    - <b>edit_config.h/cpp</b>
        > Creates a dock widget allowing inserting data to <u>[configuration file](#configuration-file)</u> such as net name, coment, variables, ...

    - <b>help.h/cpp</b>
        > Contains show_help function, displays usage on right dock widget.
    
    For detailed explanation of each file and its context <u>[see documentation](#documentation)</u>.

### Examples

![Example 1](/misc/tools_view.png)


![Example 2](/misc/tools_edit_log.png)

<div align=center>

### Monitoring
</div>

> GUI in addition to visual changes in number of tokens in different places, in tool bar is a toggable option to show change_log.
>
> Engine pipes specific information depending on current operation that is running or was completed to this log. 
>
> This allows more detailed oversight over configuration behavior.
>
> The internal Engine sends formatted datagrams (e.g., `STATE_UPDATE:CELL_1:5`, `TRANS_COLOR:T_1:green`, or `LOG:...`) to the GUI. The GUI parses these messages and visually updates token numbers and transition colors (Gray = missing tokens, Green = ready, Orange = computing/delay).

**Example of Log Output:**

```text
[LOG] UDP socket initialized on port 8080
[LOG] Received external event: SET_INPUT:in:1
[LOG] Input set: in = 1
[LOG] Enabled transition (ready to fire): T_on
[LOG] Fired transition: T_on
[LOG] VAR timeout = 5000
```

<div align=center>

## Code generator
### Introduction
</div>

> The Code Generator acts as a bridge between the visual representation in the GUI and the executable C++ backend. Instead of running a generic interpreter, our application generates a standalone, highly optimized C++ source file (`petri_net_run.cpp`) specifically tailored to the created net.

### Structure
> - **`generator.h/cpp`**: Reads parsed data from the `NetParser` (places, transitions, arcs, variables, guards, and actions).
> - Translates user-defined C++ logic directly into executable C++ functions (e.g., `guard_T_on()`, `action_T_on()`).
> - Automatically calls the local `g++` compiler via `QProcess` to compile the generated file with `engine.cpp` into an executable binary (`petri_engine`).

### Examples
> If a transition `T_1` has a guard `[ kredit >= 15 ]` and action `{ kredit -= 15; }`, the generator outputs:

```cpp
bool guard_T_1() {
    return kredit >= 15;
}
void action_T_1() {
    kredit -= 15; g_engine->send_log("Kredit snizen.");
}
```

<div align=center>

## Configuration file

</div>

> Created by creating new configuration in GUI.
> 
> This configuration file can be changed by hand if necessary and saved/loaded.
>
> File is created by serializer.h/cpp when "Save configuration" option in GUI toolbar is triggered.
>
> File is parsed by parser.h/cpp files, which parse the file and loads it to desired structs for further use.
>
> File can be loaded into GUI, achieved by loader.h/cpp. <br>
> Allows saving configuration and opening it in app and eliminates the need to "draw" the same configuration again.

Structure:

```txt
   Jmeno site:
        test_net
    Komentar:
        something ... 
    Vstupy:
        CELL_1
        CELL_2
        CELL_3
    Vystupy:
        CELL_4
    Promenne:
        int citac = 0;
    Mista (pocatecni tokeny, volitelne akce):
        CELL_1 (50) : { }
        CELL_2 (40) : { }
        CELL_3 (5) : { }
        CELL_4 (0) : { }
    Prechody (prvni blok = arcs, druhy blok = podminky a akce):
    T_1 :
        in:  CELL_2*20, CELL_1*10, CELL_3*1
        out: CELL_4*50
        when:   [/*some code*/] @ 1000
        do: {/*some code*/} 
```

---
<div align="center"><b>End of program section</b></div>

---

<div align=center>

## Miscellaneous
</div>

### Authors
- <b>xlostap00</b> (leader) : Engine, Serializer, Generator, Miscellaneous (MakeFile, GitHub, ... )
- <b>xkachyf00</b> : Loader, Parser, Miscellaneous
- <b>xcorejs00</b> : GUI, Miscellaneous

### Usage
**Prerequisites:**
- `make`
- `g++` compiler (required for generating the engine on the fly)
- `Qt5` development libraries

**Build and Run:**
1. Compile the project using Makefile: `make`
2. Run the application: `./icp_project` (or `make run`)
3. Inside the GUI: Draw your net or load an existing `.txt` config via `File -> Open file`.
4. Click `File -> Run` to generate, compile, and start the Engine.
5. Use the right sidebar to send events (e.g., `EVENT:event_name` or `SET_INPUT:var:value`) and watch the simulation.

### Constraints & abilities
**Abilities:**
- Full support for Time Petri Nets (delays via `@ time_ms`).
- Support for internal C++ variables, conditional guards, and execution actions.
- Real-time visual feedback (dynamic color-coding of transitions).
- Client-Server architecture (GUI and Engine run independently and communicate via UDP).
- Robust syntax shortcuts (e.g., sending `in=1` automatically updates the input and triggers the event).

**Constraints:**
- Guard conditions and actions must be written in valid C++ syntax, otherwise the generated engine will fail to compile.
- Requires a local C++ compiler (`g++`) to be installed and accessible in the system path.
- Net structure cannot be modified while the simulation is actively running (requires stopping and rebuilding the engine).

### Documentation
> Created through doxygen. To generate documentation run:
 - ``` make doxygen ```
