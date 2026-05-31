/**
 * @file globals.h
 * @author Šimon Čorej (xcorejs00)
 * @brief Globah header file containing all necessary libraries, struct and struct list definitions
 */

#pragma once

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDockWidget>
#include <QDrag>
#include <QDropEvent>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMetaType>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QPushButton>
#include <QRegularExpression>
#include <QString>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QProcess>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <cmath>

using namespace std;

class click_label;
class click_trans_label;
class line_widget;
struct CellStruct;
struct TransLabelStruct;
struct TransitionStruct;
struct LineStruct;

extern QString cell_style;
extern QString trans_style;
extern QUdpSocket * gui_socket;

/**
 * @brief Structure containg info about cell label (place)
 * 
 */
struct CellStruct {
    QLabel * label_ptr; ///< Pointer to clickable cell label
    QString label_text; ///< Default text of label e.g. CELL-1, CELL-2, etc...
    QPointF pos;        ///< Position of label
    int tokens = 0;     ///< Number of tokens, default 0
};
/**
 * @brief List of all existing cells
 * 
 */
extern std::vector<CellStruct> cell_list;

/**
 * @brief Struct for Transitions, holds delay, event, guard, action code
 * 
 */
struct TransProperties {
    int delay_ms = 0;
    QString event_name = "";
    QString guard_code = "";
    QString action_code = "";
};

/**
 * @brief Struct containing info about transition label
 * 
 */
struct TransLabelStruct {
    QLabel * label_ptr; ///< Pointer to clickable transition label
    QString label_text; ///< Default text of label e.g. 1, 2, etc... 
    QPointF pos;        ///< Position of label
    TransProperties * properties;
};

/**
 * @brief List of all existing transition labels
 * 
 */
extern std::vector<TransLabelStruct> trans_list;

/**
 * @brief Struct containing info about drawn lines (graphic arc representation)
 * 
 */
struct LineStruct {
    line_widget * line;
    QLabel * cell_label;
    QLabel * trans_label;
};
/**
 * @brief List containing all placed lines (for deletion)
 * 
 */
extern std::vector<LineStruct> placed_lines;

/**
 * @brief Struct containing arc info
 * 
 */
struct TransitionStruct {
    QLabel * trans_label; ///< Pointer to clickable transition label
    QLabel * cell_label;  ///< Pointer to clickable cell label
    bool to_from;         ///< Direction: to -> 1, from -> 0
    int weight = 1;       ///< Weight of arc
    line_widget * line;
};

/**
 * @brief List containing all arcs
 * 
 */
extern std::vector<TransitionStruct> trans_cell_list;

/**
 * @brief List containing currently selected cell labels
 * 
 */
extern std::vector<QLabel*> curr_selected_trans;

/**
 * @brief List containing currently selected transition labels
 * 
 */
extern std::vector<QLabel*> curr_selected_cells;

/**
 * @brief Bool for current direction of arc
 * 
 */
extern bool curr_direction;

/**
 * @brief Bool for allowing clicking on clickable labels
 * 
 */
extern bool label_click_allowed;

/**
 * @brief Bool for allowing deleting labels
 * 
 */
extern bool delete_allowed;

/**
 * @brief List containing all placed cells (for deletion)
 * 
 */
extern std::vector<click_label * > placed_labels;

/**
 * @brief List containing all placed transitions (for deletion)
 * 
 */
extern std::vector<click_trans_label * > placed_trans_labels;

/**
 * @brief Struct containing header of config file
 * 
 */
struct ConfigData {
    QString net_name = "";                      ///< Net name
    QString comment = "";                       ///< Comment
    std::vector<CellStruct *> start_places = {};///< List of starting cells
    std::vector<CellStruct *> end_places = {};  ///< List of end cells
    QStringList variables;                      ///< List of variables
};

/**
 * @brief Global pointer to configuration data
 * 
 */
extern ConfigData * config_data;

Q_DECLARE_METATYPE(CellStruct * )
Q_DECLARE_METATYPE(TransLabelStruct * )
Q_DECLARE_METATYPE(TransitionStruct * )
