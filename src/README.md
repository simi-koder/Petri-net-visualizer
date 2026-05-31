<div align="center">

# GUI

## FILES
</div>

 - ### Header files:
    - [globals.h](../GUI/globals.h#L3)
        - stores all libraries, variables used by all functions
    - help.h
        - has declaration of show_help function
    - [labels.h](#Labels)
        - implementation of clickable labels, lines and drag-n-drop widgets
    - object_list.h
        - *unfinished* declaration for showing list of all objects and transitions
    - [tools.h](#Tools)
        - declaration of show_tools() and show_help()

- ### Cpp files:
    - [globals.cpp](../GUI/globals.cpp#L3)
        - initialization of all variables
    - help.cpp
        - implementation of help sidebar *unfinished*
    - [labels.cpp](#Labels)
        - implementation of label deletion
    - object_list.cpp
        - *unfinished* show_list() implementation
    - [tools.cpp](#Tools)
        - implementation of show_tools(), e.g.: buttons, labels, lines, etc...

 <div align="center">

## DETAILS
</div>

- ## Labels

#### Main classes:
 - #### [<b>Clickable label</b>](../GUI/labels.h#L9)
```cpp
    class click_label : public QLabel {
    public:
        click_label(const QString text, QString style, QWidget *parent = nullptr) : QLabel(text, parent){
            // Constructs new label with same cell_style 
        }

        void reset() {
            // Resets current label to its original style
        }
    protected:
        // Allows mouse event
        void mousePressEvent(QMouseEvent * event) override {
            if ((event->button() == Qt::LeftButton) && (label_click_allowed)){
                // If left mouse button is pressed and click is allowed by
                // toggling Add transition button
                if (!label_clicked){
                    // If cell was not yet selected, changes color to green
                    if (curr_selected_cells.size() >= 1){
                        // Only one cell can be selected at a time
                    }
                }
                else{
                    // If cell was already selected, unselect it and 
                    // change back to original style
                }
                
            }
            else if ((event->button() == Qt::RightButton) && (delete_allowed)){
                // Delete this cell if Delete button is toggled and
                // clicked right mouse button
                // Calls delete_cell in [labels.cpp](../GUI/labels.cpp#L6)
            }
        }
    private:
        bool label_clicked = false;
        // Private bool to store state of cell
};
```
 - #### <b>[Clickable transition label](../GUI/labels.h#L76)</b>

Same structure and logic, only lists and styles changed

 - #### <b>[Lines](../GUI/labels.h#L141)</b>
```cpp
class line_widget : public QWidget {
    public:
        line_widget(QPoint start, QPoint end, QWidget * parent = nullptr) : QWidget(parent), point1(start), point2(end) {
        }
        
        void set_points(QPoint start, QPoint end){
            // Initialize point1 and poin2 to start and end
        }
    protected:
        void mousePressEvent(QMouseEvent * event) override {
            if ((event->button() == Qt::RightButton) && (delete_allowed)){
                // If delete, hide widget and delete later
            }
        }

    private:
        QPoint point1, point2;

        void paintEvent(QPaintEvent *) override {
            // Paints straight line from start to end

            // Calculates mid point on that line
            QPoint mid_point;

            // Calculates angle of the line
            double angle;

            // Calculates points for both lines of arrow
            QPoint arrow_p1 = mid_point - QPoint(
                arrow_size * std::cos(angle - /* arrow head angle */ ),
                arrow_size * std::sin(angle - /* arrow head angle */ )
            );
            QPoint arrow_p2 = mid_point - QPoint(
                arrow_size * std::cos(angle + /* arrow head angle */ ),
                arrow_size * std::sin(angle + /* arrow head angle */ )
            );
            
            // Draws arrow (two lines with one end point)
        }
};
```

- #### <b>[Drag-n-Drop](../GUI/labels.h#L194)</b>
```cpp
class drag_widget : public QLabel {
    public:
        drag_widget(const QString text, QString style, QWidget *parent = nullptr) : QLabel(text, parent) {
            // Set style and alignment
        }

    protected:
        void mousePressEvent(QMouseEvent * event) override {
            if ((event->button() == Qt::LeftButton) && (!label_click_allowed)) {

                // Mimes data of label and paints by style

                if (this->text() == "<b></b>"){
                    // Draws rectangle if transition is dropped
                }
                else {
                    // Draws elipse if cell is dropped
                }

                // Paints everything, centered on cursor
            }
        }

};

// -------------------- DROP LABEL --------------------
class drop_widget : public QWidget {
public:
    drop_widget(QWidget * parent = nullptr) : QWidget(parent) {
        setAcceptDrops(true);
    }

protected:
    void dragEnterEvent(QDragEnterEvent * event) override {
        if (event->mimeData()->hasText())
            event->acceptProposedAction();
    }

    void dropEvent(QDropEvent * event) override {
        // Rcovers text and style of label
        
        if (text == "<b>CELL</b>"){
            // Creates new click_label, adds id to text
            // id is the current size of cell_list (number of cells existing)
            click_label *label = new click_label(QString("<b>%1 - %2</b>").arg(text).arg(cell_list.size() + 1), style, this);

            // Recalculate position to get rid of shift
            // (Magic numbers to achieve the right alignment)
            QPoint pos;

            // Adds cell to list of existing cells
            CellStruct curr_label = {label, pos};
            cell_list.push_back(curr_label);

            // Places the label at given position

            // Adds to list of placed labels (for deleting if necessary) 
            placed_labels.push_back(label);
        }
        else if (text == "<b></b>"){
            // Creates new click_trans_label
            // id is the current size of trans_list (number of transition labels existing)
            click_trans_label * label = new click_trans_label(QString("<b>%1</b>").arg(trans_list.size() + 1), style, this);

            // Recalculate position to get rid of shift
            // (Magic numbers to achieve the right alignment)
            QPoint pos;

            // Adds transition labels to list of existing transition labels
            TransLabelStruct curr_label = {label, pos};
            trans_list.push_back(curr_label);

            // Places the label at given position

            // Adds to list of existing labels (for deleting if necessary)
            placed_trans_labels.push_back(label);
        }
    }
};
```

- ## TOOLS
![Default state](https://github.com/user-attachments/assets/7e560827-b435-463e-bdd2-a578387f7f57.png)

 - Add transition button:
     - Allows clicking on labels, clicked labels change color to green, checks if only 1 transition and 1 cell were selected
 - Delete button:
     - Allows deletion of labels and widget by rightclick
 - Add token:
     - *unfinished*

![Add Transition Clicked](https://github.com/user-attachments/assets/c2ffd848-5a5e-4841-ae02-05d5a10a4a01.png)
 - After clicking Add transition button:
     - Delete button is disabled and hidden
      - Save transition button is showed, when clicked saves transition if posible

<div align="center">

## COMMANDS
</div>

```bash
cmake --build . --target todos 
# shows all todos and fixmes in project
```