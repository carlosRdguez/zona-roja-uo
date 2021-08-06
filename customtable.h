#ifndef CUSTOMTABLE_H
#define CUSTOMTABLE_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QSize>
#include <QFontDatabase>
#include <QHeaderView>
#include <QLinearGradient>
#include <QGraphicsDropShadowEffect>


class CustomTable : public QTableWidget
{    
public:
    explicit CustomTable(QWidget *parent = nullptr);
    virtual ~CustomTable();
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    // dibuja una linea bajo cada fila
    void draw();
private:
    int filaResaltada = -1;
};

#endif // CUSTOMTABLE_H
