/**
 * @file labels.h
 * @author Šimon Čorej (xcorejs00)
 * @brief Header file for labels.cpp, inheritance of QLabel class as clickable labels, drag-n-drop labels and lines
 */
#pragma once

#include "globals.h"

/**
 * @brief Function deletes wanted cell and all lists where this cell is present
 * 
 * @param item Pointer to label of a cell to be deleted 
 */
void delete_cell(QLabel * item);

/**
 * @brief Function deletes wanted transition label and all lists where this label is present
 * 
 * @param item Pointer to label of a transition to be deleted 
 */
void delete_transition(QLabel * item);

/**
 * @brief Inherites all functionallity of QLabel and overrides mousePressEvent giving it a clickable property (changes color to green)
 * 
 */
class click_label : public QLabel {
    public:
        click_label(const QString text, QString style, QWidget *parent = nullptr) : QLabel(text, parent){
            setAlignment(Qt::AlignCenter);
            setStyleSheet(style); 
        }

        void reset() {
            label_clicked = false;
            setStyleSheet("background-color: darkGray;" + cell_style);
        }
    protected:
        void mousePressEvent(QMouseEvent * event) override {
            if ((event->button() == Qt::LeftButton) && (label_click_allowed)){
                QString tmp_text = this->text();
                tmp_text.remove(QRegularExpression("<[^>]*>"));
                QString cell_id;
                if (tmp_text.contains(" - ")) {
                    QStringList tmp_text_list = tmp_text.split("-");
                    cell_id = tmp_text_list.at(1).trimmed();
                } else if (tmp_text.contains(":")) {
                    QStringList tmp_text_list = tmp_text.split(":");
                    cell_id = tmp_text_list.at(0).trimmed();
                } else {
                    cell_id = tmp_text.trimmed();
                }

                if (!label_clicked){
                    if (curr_selected_cells.size() >= 1){
                        cout << "More than one cell selected. Can only select Transition -> Cell and vice versa. Exiting..." << endl;
                        return;
                    }

                    this->setStyleSheet(
                        "background-color: green;"
                        +
                        cell_style
                    );
                    // Check if no transitions are selected if so cell -> transition
                    curr_selected_cells.push_back(this);
                    if (curr_selected_trans.size() == 0){
                        curr_direction = true;
                    }

                    cout << "clicked label id: " << cell_id.toStdString() << endl;
                    
                    label_clicked = true;
                }
                else{
                    this->setStyleSheet(
                        "background-color: darkGray;"
                        +
                        cell_style
                    );
                    curr_selected_cells.erase(
                        std::remove(curr_selected_cells.begin(), curr_selected_cells.end(), this),
                        curr_selected_cells.end()
                    );
                    cout << "unclicked label id: " << cell_id.toStdString() << endl;
                    label_clicked = false;
                }
                cout << "list of cells: " << endl;
                for (auto &item : curr_selected_cells) {
                    cout << item->text().toStdString() << endl;
                }
            }
            else if ((event->button() == Qt::RightButton) && (delete_allowed)){
                delete_cell(this);
            }
        }
    private:
        bool label_clicked = false;
};

/**
 * @brief Inherites all functionallity of QLabel and overrides mousePressEvent giving it a clickable property (changes color to green)
 * 
 */
class click_trans_label : public QLabel {
    public:
        click_trans_label(const QString text, QString style, QWidget * parent) : QLabel(text, parent){
            setAlignment(Qt::AlignCenter);
            setStyleSheet(style);
        }

        void reset() {
            label_clicked = false;
            setStyleSheet("background-color: darkGray;" + trans_style);
        }
    protected:
        void mousePressEvent(QMouseEvent * event) override {
            if ((event->button() == Qt::LeftButton) && (label_click_allowed)){
                QString tmp_text = this->text();
                tmp_text.remove(QRegularExpression("<[^>]*>"));

                if (!label_clicked){
                    if (curr_selected_trans.size() >= 1){
                        cout << "More than one transition selected. Can only select Transition -> Cell and vice versa. Exiting..." << endl;
                        return;
                    }
                    this->setStyleSheet(
                        "background-color: green;"
                        +
                        trans_style
                    );
                    // Chewck if no cells are selected if so transition -> cell
                    curr_selected_trans.push_back(this);
                    if (curr_selected_cells.size() == 0){
                        curr_direction = false;
                    }

                    cout << "clicked trans label id: " << tmp_text.toStdString() << endl;
                    
                    label_clicked = true;
                }
                else{
                    this->setStyleSheet(
                        "background-color: darkGray;"
                        +
                        trans_style
                    );
                    curr_selected_trans.erase(
                        std::remove(curr_selected_trans.begin(), curr_selected_trans.end(), this),
                        curr_selected_trans.end()
                    );
                    cout << "unclicked trans label id: " << tmp_text.toStdString() << endl;
                    label_clicked = false;
                }
                cout << "list of transitions: " << endl;
                for (auto &item : curr_selected_trans) {
                    cout << item->text().toStdString() << endl;
                }
            }
            else if ((event->button() == Qt::RightButton) && (delete_allowed)){
                delete_transition(this);
            }
        }
    private:
        bool label_clicked = false;
};

/**
 * @brief Inherites all functionallity of QWidget and overrides mousePressEvent giving it ability to be deleted by click.
 * Paints a line with arrow head from start point1 to end point2
 * 
 */
class line_widget : public QWidget {
    public:
        line_widget(QPoint start, QPoint end, int weight, QWidget * parent = nullptr) : QWidget(parent), point1(start), point2(end), weight(weight){
            setStyleSheet("background: transparent;");
        }
        
        void set_points(QPoint start, QPoint end){
            point1 = start;
            point2 = end;
            update();
        }
        void set_weight(int w){
            weight = w;
            update();
        }
    protected:
        void mousePressEvent(QMouseEvent * event) override {
            if ((event->button() == Qt::RightButton) && (delete_allowed)){
                this->hide();
                placed_lines.erase(
                    std::remove_if(placed_lines.begin(), placed_lines.end(), [this](const LineStruct & l){
                        if (l.line == this){
                            trans_cell_list.erase(
                                std::remove_if(trans_cell_list.begin(), trans_cell_list.end(), [l](const TransitionStruct &t){
                                    return ((t.cell_label == l.cell_label) && (t.trans_label == l.trans_label));
                                }),
                                trans_cell_list.end()
                            );
                            return 1;
                        }
                        return 0;
                    }),
                    placed_lines.end()
                );
                this->deleteLater();
                cout << "All saved transitions:" << endl;
                for (auto &row : trans_cell_list) {
                    cout << (QString("  %1 -> %2")
                        .arg(row.to_from == 1 ? row.cell_label->text() : row.trans_label->text())
                        .arg(row.to_from == 1 ? row.trans_label->text() : row.cell_label->text()))
                        .toStdString() << endl;
                }
            }
        }

    private:
        QPoint point1, point2;
        int weight;

        void paintEvent(QPaintEvent *) override {
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(QPen(Qt::black, 2));

            painter.drawLine(point1, point2);

            QPoint mid_point = (point1 + point2) / 2;

            double angle = std::atan2(point2.y() - point1.y(), point2.x() - point1.x());

            int arrow_size = 15;

            QPoint arrow_p1 = mid_point - QPoint(
                arrow_size * std::cos(angle - M_PI / 8),
                arrow_size * std::sin(angle - M_PI / 8)
            );
            QPoint arrow_p2 = mid_point - QPoint(
                arrow_size * std::cos(angle + M_PI / 8),
                arrow_size * std::sin(angle + M_PI / 8)
            );

            QPoint mid_point_text = mid_point + QPoint(12, -12);
            
            painter.drawLine(mid_point, arrow_p1);
            painter.drawLine(mid_point, arrow_p2);

            painter.setPen(Qt::red);
            QFont font = painter.font();
            font.setPointSize(11);
            font.setBold(true);
            painter.setFont(font);
            painter.drawText(mid_point_text, QString::number(weight));
        }
};

/**
 * @brief Inherites all functionallity of QLabel and overrides mousePressEvent, miming data of dragged label
 * 
 */
class drag_widget : public QLabel {
    public:
        drag_widget(const QString text, QString style, QWidget *parent = nullptr) : QLabel(text, parent) {
            setAlignment(Qt::AlignCenter);
            setStyleSheet(style);
        }

    protected:
        void mousePressEvent(QMouseEvent * event) override {
            if ((event->button() == Qt::LeftButton) && (!label_click_allowed)) {

                cout << "dragging..." << endl;

                // Copy data from default label
                QDrag * drag = new QDrag(this);
                QMimeData * mime_data = new QMimeData();
                mime_data->setText(this->text());
                mime_data->setData("style", this->styleSheet().toUtf8());
                drag->setMimeData(mime_data);

                // Create pixmap to recreate label perfectly
                QPixmap pixmap(this->size());
                pixmap.fill(Qt::transparent);

                QPainter painter(&pixmap);
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setBrush(QColor("darkGray"));
                painter.setPen(Qt::NoPen);

                if (this->text() == "<b></b>"){
                    painter.drawRect(pixmap.rect());
                }
                else {
                    painter.drawEllipse(pixmap.rect());
                }

                painter.setPen(Qt::black);
                painter.drawText(pixmap.rect(), Qt::AlignCenter, "");
                painter.end();

                // Set recreated label, align cursor in the middle and execute
                drag->setPixmap(pixmap);
                drag->setHotSpot(pixmap.rect().center());
                drag->exec(Qt::CopyAction);
            }
        }

};

/**
 * @brief Inherites all functionallity of QWidget, allows dropEvent copying data from dragged widget and pasting it to a position on canvas
 * 
 */
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
        QString text = event->mimeData()->text();
        QString style = QString::fromUtf8(event->mimeData()->data("style"));
        
        // Add id to cell
        if (text == "<b>CELL</b>"){
            click_label *label = new click_label(QString("<b>%1 - %2</b>").arg(text).arg(cell_list.size() + 1), style, this);

            // Recalculate position to get rid of shift
            // (Magic numbers to achieve the right alignment)
            QPoint pos = event->pos() - QPoint(label->width()/2, label->height()/0.6);
            CellStruct curr_label = {label, label->text(), pos};
            cell_list.push_back(curr_label);

            cout << "curr cell list size: " << cell_list.size() << endl;

            label->setStyleSheet(style);
            label->setAlignment(Qt::AlignCenter);

            label->move(pos);
            label->show();
            placed_labels.push_back(label);
        }
        else if (text == "<b></b>"){
            click_trans_label * label = new click_trans_label(QString("<b>%1</b>").arg(trans_list.size() + 1), style, this);

            QPoint pos = event->pos() - QPoint(10, 50);
            TransProperties * properties = new TransProperties();
            TransLabelStruct curr_label = {label, label->text(), pos, properties};
            trans_list.push_back(curr_label);

            cout << "curr trans list size: " << trans_list.size() << endl;

            label->setStyleSheet(style);
            label->setAlignment(Qt::AlignCenter);

            label->move(pos);
            label->show();
            placed_trans_labels.push_back(label);
        }

        cout << "Dropped: " << text.toStdString() << endl;
    }
};
