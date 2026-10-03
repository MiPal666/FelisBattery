#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <sailfishapp.h>

#include "batterymonitor.h"

int main(int argc, char *argv[])
{
    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();

    BatteryMonitor monitor;
    view->rootContext()->setContextProperty("batteryMonitor", &monitor);
    view->setSource(SailfishApp::pathTo("qml/harbour-felisbattery.qml"));
    view->show();

    return app->exec();
}
