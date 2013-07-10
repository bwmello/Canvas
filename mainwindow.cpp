#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImageWriter>

const int PEN = 1;
const int RECT = 2;
const int POLY = 3;
const int FILL = 4;
const int LINE = 5;
const int CROP = 6;
const int TEXT = 7;
const int CIRCLE = 8;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    newCanvas();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();  //TODO bug to fix?  mac related?
    }
}

void MainWindow::on_action_New_triggered()
{
    if (maybeSave()) {
        delete canvas;
        newCanvas();
    }
}

void MainWindow::on_action_Open_triggered()
{
    if (maybeSave()) {
        setWindowFilePath(QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath()));
        if (!windowFilePath().isEmpty())
            canvas->openImage(windowFilePath());
    }
}

void MainWindow::on_action_Save_triggered()
{
    save();
}

void MainWindow::on_action_Save_As_triggered()
{
    saveAs();
}

void MainWindow::on_action_Print_triggered()
{
    canvas->print();
}

void MainWindow::on_action_Pen_Tool_triggered()
{
    canvas->setTool(PEN);
}

void MainWindow::on_actionEraser_triggered()
{
    canvas->setEraser();
}

void MainWindow::on_action_Line_Tool_triggered()
{
    canvas->setTool(LINE);
}

void MainWindow::on_actionCircle_Tool_triggered()
{
    canvas->setTool(CIRCLE);
}

void MainWindow::on_actionRectangle_Tool_triggered()
{
    canvas->setTool(RECT);
}

void MainWindow::on_actionPolygon_Tool_triggered()
{
    canvas->setPolyTool();
}

void MainWindow::on_action_Text_Tool_triggered()
{
    canvas->setTool(TEXT);
}

void MainWindow::on_action_Fill_Tool_triggered()
{
    canvas->setTool(FILL);
}

void MainWindow::on_action_Crop_Tool_triggered()
{
    canvas->setTool(CROP);
}

void MainWindow::on_action_Pen_Color_triggered()
{
    QColor newColor = QColorDialog::getColor(canvas->penColor());
    if (newColor.isValid())
        canvas->setPenColor(newColor);
}

void MainWindow::on_action_Pen_Width_triggered()
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Canvas"),  //TODO refactor magic string
                                        tr("Select pen width:"),
                                        canvas->penWidth(),
                                        1, 50, 1, &ok);
    if (ok)
        canvas->setPenWidth(newWidth);
}

void MainWindow::on_action_Resize_triggered()
{
    QSize oldSize = canvas->getSize();
    int newWidth = QInputDialog::getInt(this, tr("Canvas"),  //TODO refactor magic string
                                        tr("New Width:"),
                                        oldSize.width());  //TODO handle user clicking cancel
    int newHeight = QInputDialog::getInt(this, tr("Canvas"),  //TODO refactor magic string
                                         tr("New Height:"),
                                         oldSize.height());  //TODO handle user clicking cancel
    canvas->setSize(QSize(newWidth, newHeight));
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("About Canvas"), //TODO refacter magic string
                       tr("Canvas is a GUI paint application developed by:"
                          "<ul><li>Charlie Carlson</li>"
                          "<li>Andrew Johnston</li>"
                          "<li>Brent Mello</li>"
                          "<li>Mark Robinson</li></ul>"
                          "Canvas was built as a project for the University of Alaska Fairbanks' "
                          "spring 2013 Software Construction course.  It is based on the Qt "
                          "<a href=\"http://qt-project.org/doc/qt-4.8/widgets-scribble.html\">Scribble</a> "
                          "example and was developed using Qt 5.0.2."));
}

void MainWindow::on_action_About_Qt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::newCanvas() {
    canvas = new Canvas;
    setCentralWidget(canvas);
    setWindowFilePath("");
}

bool MainWindow::save()
{
    if (windowFilePath().isEmpty())
        return saveAs();
    else
        return canvas->saveImage(windowFilePath());
}

bool MainWindow::saveAs()
{
    QString filter = "All Files (*)";
    for (auto format : QImageWriter::supportedImageFormats()) {
        filter += ";;" + QString(format).toUpper() + " Files (*." + QString(format) + ")";
    }

    setWindowFilePath(QFileDialog::getSaveFileName(this, tr("Save As"), windowFilePath(), filter));
    if (windowFilePath().isEmpty()) {
        return false;
    }
    else {
        return canvas->saveImage(windowFilePath());
    }
}

bool MainWindow::maybeSave()
{
    if (canvas->isModified()) {
        QMessageBox::StandardButton selectedButton;
        selectedButton = QMessageBox::warning(this, tr("Canvas"),  //TODO refactor magic string
                                              tr("The image has been modified.\n"
                                                 "Do you want to save your changes?"),
                                              QMessageBox::Save | QMessageBox::Discard
                                              | QMessageBox::Cancel);
        if (selectedButton == QMessageBox::Save) {
            return save();
        } else if (selectedButton == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}
