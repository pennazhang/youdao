#include "widget.h"
#include <QApplication>
#include "YoudaoSetting.h"
#include <QTranslator>
#include "bookInfo.h"

int main(int argc, char *argv[])
{
	g_pRegSetting = new YoudaoSetting("HKEY_CURRENT_USER\\Software\\YoudaoWord", QSettings::NativeFormat);  
	g_pDeviceBookInfo = new BookInfo();

    QApplication app(argc, argv);
	QFont font("Tahoma", 8);
	app.setFont(font);

#if 1
    static QTranslator *pTranslator = NULL;
    if (pTranslator != NULL)
    {
        qApp->removeTranslator(pTranslator);
        delete pTranslator;
        pTranslator = NULL;
    }
    pTranslator = new QTranslator;
    QString qmFileName = ":youdao_zh.qm";
    if (pTranslator->load(qmFileName))
    {
        qApp->installTranslator(pTranslator);
    }
#endif

    Widget w;
    w.show();

#if 1
	QFile qss(":YoudaoWord.qss");
    qss.open(QFile::ReadOnly);
    app.setStyleSheet(qss.readAll());
    qss.close();
#endif

    return app.exec();
}
