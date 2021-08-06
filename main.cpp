#include "ventana_principal.h"
#include <QApplication>

#include <QProxyStyle>
#include <QPainter>

#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>

// Disables focus drawing for all widgets
class Style_tweaks : public QProxyStyle
{
public:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
        QPainter *painter, const QWidget *widget) const
    {
        if (element == QStyle::PE_FrameFocusRect) return;

        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new Style_tweaks);
    int id = QFontDatabase::addApplicationFont("://fonts/Lato-Regular.ttf");
    QString lato = QFontDatabase::applicationFontFamilies(id).at(0);
    a.setFont(QFont(lato, 12));
    QPixmap pixmap("://images/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    VentanaPrincipal w;
    w.show();
    splash.finish(&w);
    return a.exec();
}
