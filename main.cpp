#include <QCoreApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QWindow>
#include "tuduino.h"

#include <QQmlContext>
#include <QDebug>
#include <QtQuick>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QGuiApplication app(argc, argv);
    QApplication app(argc, argv);
    QScopedPointer<tuduino> TuDuino(new tuduino);
    QQuickView view;
    view.rootContext()->setContextProperty("TuDuino", TuDuino.data());
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();
    return app.exec();
}
