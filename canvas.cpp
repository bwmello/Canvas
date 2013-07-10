#include "canvas.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QDialog>
#include <QInputDialog>
#include <QFontDialog>
#include <QFont>
#include <QDir>
#include <cmath>
#include <iostream>

const int PEN = 1;
const int RECT = 2;
const int POLY = 3;
const int FILL = 4;
const int LINE = 5;
const int CROP = 6;
const int TEXT = 7;
const int CIRCLE = 8;

Canvas::Canvas(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    _isModified = false;
    _isDrawing = false;
    _isText = false;
    _penWidth = 1;
    _angle = 0;
    _penColor = Qt::black;
    _lastColor = Qt::black;
    _tool = PEN;

    setSize(QSize(400, 400));
}

bool Canvas::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    _image = loadedImage;
    _isModified = false;
    update();
    return true;
}

bool Canvas::saveImage(const QString &fileName)
{
    if (_image.save(fileName)) {
        _isModified = false;
        return true;
    } else {
        return false;
    }
}

void Canvas::setPenColor(const QColor &newColor)
{
    _penColor = newColor;
    _lastColor = _penColor;
}

void Canvas::setPenWidth(int newWidth)
{
    _penWidth = newWidth;
}

void Canvas::setTool(int tool)
{
    _tool=tool;
    resetColor();
}

void Canvas::setEraser()
{
    _penColor = Qt::white;
    _tool = PEN;
}

void Canvas::setPolyTool()
{
    bool ok;

    int sides = QInputDialog::getInt(this, tr("Canvas"),
                                        tr("Select number of sides:"),
                                        _numSides,
                                        3, 10, 1, &ok);

    int angle = QInputDialog::getInt(this, tr("Canvas"),
                                        tr("Select the angle (Degrees):"),
                                        _angle,
                                        0, 360, 1, &ok);

    if (ok)
    {
        _numSides = sides;
        _tool = POLY;
        _angle = angle * 0.0174532925; //Convert to radians
        resetColor();
    }
}

void Canvas::resetColor()
{
    _penColor = _lastColor;
}

bool Canvas::isModified() const
{
    return _isModified;
}

QColor Canvas::penColor() const
{
    return _penColor;
}

int Canvas::penWidth() const
{
    return _penWidth;
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
            _lastPoint = event->pos();
            _isDrawing = true;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && _isDrawing && (_tool == PEN))
        drawLineTo(event->pos());
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _isDrawing)
    {
        switch(_tool)
        {
            case PEN:drawLineTo(event->pos());
                break;
            case LINE:drawLineTo(event->pos());
                break;
            case RECT:drawRect(event->pos().x(), event->pos().y());
                break;
            case POLY:drawPoly(event->pos());
                break;
            case CIRCLE:drawCircle(event->pos());
                break;
            case CROP:cropRect(event->pos().x(), event->pos().y());
                break;
            case TEXT:drawTextAt(event->pos());
                break;
            case FILL:
            {
                _lastColor = QColor(_image.pixel(event->pos()));
                drawFill(event->pos());
                update();
            } break;
        }
    }
    _isDrawing = false;
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, _image, dirtyRect); //TODO understand
}

void Canvas::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&_image);
    painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(_lastPoint, endPoint);  //TODO not drawing when lastpoint == endpoint
    _isModified = true;

    int rad = (_penWidth / 2) + 2;
    update(QRect(_lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));

    _lastPoint = endPoint;
}

void Canvas::drawTextAt(const QPoint &point)
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter Text"), tr("Text:"), QLineEdit::Normal, "Canvas", &ok);

    if(ok)
    {
        QFont font = QFontDialog::getFont( &ok, QFont("Helvetica [Cronyx]", 10), this);
        if(ok)
        {
            QPainter painter(&_image);
            painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.setFont(font);
            painter.drawText(point, text);
            _isModified = true;
        }
    }

    update();
}

void Canvas::drawCircle(const QPoint &endPoint)
{
    long double radius = sqrt(pow(abs(_lastPoint.x()-endPoint.x()), 2) + pow(abs(_lastPoint.y()-endPoint.y()), 2));
    int width = radius*2;
    int height = radius*2;

    QPainter painter(&_image);
    painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawArc(_lastPoint.x()-width*0.5, _lastPoint.y()-height*0.5, width, height, 0, 5760); //5760 = 16*360  qt uses 1/16 degrees
    _isModified = true;

    update();
}

void Canvas::drawFill(const QPoint &point)
{
    //Algorithm based on fill algorithm from easypaint

    int left_x = 0;
    QPoint temp_point = point;
    while(true)
    {
        if(_image.pixel(temp_point) != _lastColor.rgb())
            break;
        _image.setPixel(temp_point, _penColor.rgb());
        if(temp_point.x() > 0)
        {
            temp_point -= QPoint(1,0);
            left_x = temp_point.x();
        }
        else
            break;
    }

    int right_x= 0;
    temp_point = point + QPoint(1,0);
    while(true)
    {
        if(_image.pixel(temp_point) != _lastColor.rgb())
            break;
        _image.setPixel(temp_point, _penColor.rgb());
        if(temp_point.x() < _image.width() - 1)
        {
            temp_point += QPoint(1,0);
            right_x = temp_point.x();
        }
        else
            break;
    }

    for(int x_spot = left_x+1; x_spot < right_x; ++x_spot)
    {
        if(point.y() < 1 || point.y() >= _image.height() - 1)
            break;
        if(right_x > _image.width())
            break;
        QRgb currentColor = _image.pixel(x_spot, point.y() - 1 );
        if(currentColor == _lastColor.rgb() && currentColor != _penColor.rgb())
            drawFill(QPoint(x_spot, point.y() - 1));
        currentColor = _image.pixel(x_spot, point.y() + 1);
        if(currentColor == _lastColor.rgb() && currentColor != _penColor.rgb())
            drawFill(QPoint(x_spot, point.y() + 1));
    }

}

void Canvas::drawRect(const int &xEndPosition, const int &yEndPosition)
{
    int width = abs(_lastPoint.x() - xEndPosition);
    int height = abs(_lastPoint.y() - yEndPosition);
    int drawPointX = xEndPosition;
    int drawPointY = yEndPosition;
    if(_lastPoint.x() < xEndPosition)
        drawPointX = _lastPoint.x();
    if(_lastPoint.y() < yEndPosition)
        drawPointY = _lastPoint.y();

    QPainter painter(&_image);
    painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
    painter.drawRect(drawPointX, drawPointY, width, height);
    _isModified = true;

    int rad = (_penWidth / 2) + 2;
    update(QRect(drawPointX, drawPointY, width, height).normalized().adjusted(-rad, -rad, +rad, +rad));
}

void Canvas::drawPoly(const QPoint &endPoint)
{
    long double radius = sqrt(pow(abs(_lastPoint.x()-endPoint.x()), 2) + pow(abs(_lastPoint.y()-endPoint.y()), 2));

    QPoint* points = new QPoint[_numSides];
    for(int i = 0; i < _numSides; i++)
    {
        points[i].setX(0);
        points[i].setY(0);
    }

    double pi = 3.141592;

    for(int i=0; i<_numSides; i++)
    {
        points[i].setX(_lastPoint.x() + radius*cos((i*2*pi/_numSides - _angle)));
        points[i].setY(_lastPoint.y() + radius*sin((i*2*pi/_numSides - _angle)));
    }

    QPainter painter(&_image);
    painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
    painter.drawPolygon(points, _numSides);
    _isModified = true;
    update();

    delete [] points;
}

void Canvas::cropRect(const int &xEndPosition, const int &yEndPosition)
{
    if((abs(_lastPoint.x()-xEndPosition)*abs(_lastPoint.y()-yEndPosition)) < 25)
        return; //Don't crop selections w/ an area of less than 25 pixels

    int topSide=yEndPosition; int bottomSide=_lastPoint.y();
    int leftSide=xEndPosition; int rightSide=_lastPoint.x();
    if(_lastPoint.y() < yEndPosition)
        topSide = _lastPoint.y(); bottomSide = yEndPosition;
    if(_lastPoint.x() < xEndPosition)
        leftSide = _lastPoint.x(); rightSide = xEndPosition;

    QSize winSize = getSize();
    int winHeight = winSize.height();
    int winWidth = winSize.width();

    QPainter painter(&_image);
    painter.eraseRect(0,0,winWidth,topSide);
    painter.eraseRect(0,0,leftSide,winHeight);
    painter.eraseRect(0,bottomSide,winWidth,winHeight);
    painter.eraseRect(rightSide,0,winWidth,winHeight);
    _isModified = true;

    update();
}

void Canvas::setSize(const QSize &newSize)
{
    if (_image.size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(Qt::white);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), _image);
    _image = newImage;
    update();
}

void Canvas::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *printDialog = new QPrintDialog(&printer, this);

    if (printDialog->exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = _image.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(_image.rect());
        painter.drawImage(0, 0, _image);
    }
#endif // QT_NO_PRINTER
}

QSize Canvas::getSize() {
    return _image.size();
}
