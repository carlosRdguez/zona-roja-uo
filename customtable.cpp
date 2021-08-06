#include "customtable.h"

CustomTable::CustomTable(QWidget* parent) : QTableWidget(parent) {
    setMouseTracking(true);
    setGridStyle(Qt::NoPen);
    int id = QFontDatabase::addApplicationFont("://fonts/Lato-Heavy.ttf");
    QString lato = QFontDatabase::applicationFontFamilies(id).at(0);
    horizontalHeader()->setFont(QFont(lato, 14));
    verticalHeader()->setFont(QFont(lato, 10));
    setFont(QFont(lato, 12));
    QPixmap cursor_pixmap("://cursors/cursor.png");
    QCursor cursor_default(cursor_pixmap, 7, 8);
    setCursor(cursor_default);
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(5);
    shadowEffect->setColor(QColor(145, 145, 145, 80));
    shadowEffect->setOffset(-3, -3);
    setGraphicsEffect(shadowEffect);
}

CustomTable::~CustomTable() {

}

void CustomTable::mouseMoveEvent(QMouseEvent *event) {
    if(event->pos().y() >= contentsRect().top() &&
       event->pos().y() <= contentsRect().bottom() &&
       event->pos().x() >= contentsRect().left() &&
            event->pos().x() <= contentsRect().right()) {
        // mouse posicionado sobre el contenido de la tabla
        auto row = rowAt(event->pos().y());
        filaResaltada = row; // actualizar fila resaltada
        repaint();
    }
    QTableWidget::mouseMoveEvent(event);
}

void CustomTable::paintEvent(QPaintEvent* event) {
    QTableView::paintEvent(event);
    draw();
}

void CustomTable::draw() {
    QPainter painter(viewport());
    painter.setPen(QColor(Qt::gray));
    int x1 = contentsRect().left();
    int y1 = contentsRect().top();
    int x2 = contentsRect().right();
    int y2 = contentsRect().top();
    for(int i = 0; i < rowCount(); i++) {
        y1 += rowHeight(i);
        y2 += rowHeight(i);
        if(i == 0) {
            y1 -= 2;
            y2 -= 2;
        }
        painter.drawLine(x1, y1, x2, y2);
        // aplicar efecto de sombra sobre la fila
        if(i == filaResaltada) {
            painter.setPen(Qt::NoPen);
            QLinearGradient colorGradient(x1 + (x2-x1)/2, y2, x1 + (x2-x1)/2, y1-8);
            QColor gray(80, 80, 80, 50);
            QColor white(255, 255, 255, 10);
            colorGradient.setColorAt(0, gray);
            colorGradient.setColorAt(1, white);
            painter.setBrush(colorGradient);
            painter.drawRect(x1, y1-8, x2-x1, 8);
            QLinearGradient colorGradient2(x1 + (x2-x1)/2, y2-rowHeight(i)+8, x1 + (x2-x1)/2, y1-rowHeight(i));
            white.setAlpha(10);
            gray.setAlpha(50);
            colorGradient2.setColorAt(0, white);
            colorGradient2.setColorAt(1, gray);
            painter.setBrush(colorGradient2);
            painter.drawRect(x1, y1-rowHeight(i)+1, x2-x1, 8);
            painter.setPen(QColor(Qt::gray));
        }
    }
}

