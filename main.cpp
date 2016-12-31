#include <QtGui/QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QFileInfo>

#include "EEGHeadMeasure.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EEGHeadMeasure w;
    QString locale = QLocale::system().name();
    QFileInfo styleFileInfo(":/EEGHeadMeasure/DefaultStyle.qss");
    QFile styleFile(":/EEGHeadMeasure/DefaultStyle.qss");
    if (true == styleFileInfo.exists())
      {
      if (styleFile.open(QIODevice::ReadOnly))
        {
        //qApp->setStyleSheet(styleFile.readAll());
        styleFile.close();
        }
      }


    w.show();
    return a.exec();
}
