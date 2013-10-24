#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

#include <QQmlContext>

#include "../../plistloader.h"
#include "../../plistquickimageprovider.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;

    PListLoader listLoader;

    if (listLoader.loadFile("examples_qml.plist"))
    {
        viewer.rootContext()->setContextProperty("PListDefault", &listLoader);
        viewer.engine()->addImageProvider(QLatin1String("plist"), new PListQuickImageProvider(&listLoader));

        viewer.setMainQmlFile(QStringLiteral("qml/quick/main.qml"));
        viewer.showExpanded();
    }
    else
    {
        qDebug() << QString("%1 parse error line %2 column %3 :\n%4")
                      .arg("examples_qml.plist")
                      .arg(listLoader.getErrorLine())
                      .arg(listLoader.getErrorColumn())
                      .arg(listLoader.getErrorMessage());
        return false;
    }

    return app.exec();
}
