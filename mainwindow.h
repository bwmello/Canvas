#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "canvas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_action_Save_As_triggered();
    void on_action_Print_triggered();

    void on_action_Pen_Tool_triggered();
    void on_action_Line_Tool_triggered();
    void on_action_Crop_Tool_triggered();
    void on_action_Text_Tool_triggered();
    void on_action_Fill_Tool_triggered();
    void on_actionRectangle_Tool_triggered();

    void on_action_Pen_Color_triggered();
    void on_action_Pen_Width_triggered();
    void on_action_Resize_triggered();

    void on_action_About_triggered();
    void on_action_About_Qt_triggered();



    void on_actionPolygon_Tool_triggered();

    void on_actionCircle_Tool_triggered();

    void on_actionEraser_triggered();

private:
    Ui::MainWindow *ui;
    Canvas *canvas;

    void newCanvas();
    bool save();
    bool saveAs();
    bool maybeSave();
};

#endif // MAINWINDOW_H
