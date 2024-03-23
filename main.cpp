#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqmlcontext.h>
#include "DndControler.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    DndControler dndControler;
   
    engine.rootContext()->setContextProperty("DndControler", &dndControler);
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/qtdraganddropkit/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
