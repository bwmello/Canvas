#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <string>
#include <vector>
using std::string;
using std::vector;

class Canvas : public QWidget
{
    Q_OBJECT

public:
    Canvas(QWidget *parent = 0);

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void setTool(int tool);
    void setPolyTool();
    void setEraser();
    void resetColor();
    bool isModified() const;
    QColor penColor() const;
    int penWidth() const;
    QSize getSize();
    void setSize(const QSize &newSize);

public slots:
    void print();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void drawLineTo(const QPoint &endPoint);
    void drawRect(const int &xEndPosition, const int &yEndPosition);
    void drawPoly(const QPoint &endPoint);
    void drawTextAt(const QPoint &Point);
    void drawCircle(const QPoint &endPoint);
    void cropRect(const int &xEndPosition, const int &yEndPosition);
    void drawFill(const QPoint &Point);

    bool _isModified;
    bool _isDrawing;
    bool _isText;
    int _penWidth;
    int _numSides;
    double _angle;
    int _tool;

    QColor _penColor;
    QColor _lastColor;
    QImage _image;
    QPoint _lastPoint;
};

#endif // CANVAS_H
