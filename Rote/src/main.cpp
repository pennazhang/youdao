#include "mainwindow.h"
#include <QApplication>
#include "youdaosetting.h"
#include "bookInfo.h"
#include "YoudaoDatabase.h"
#include <QStandardPaths>
#include <QDir>
#include <QTranslator>
#include <QRegExp>
#include <QScreen>

#if defined(Q_OS_IOS)
#include "AutoLock.h"
AutoLock g_autoLock;
#endif

QString g_path;
extern void setApplicationStyleSheet(QString fileName, int fontScalePercent);

int main(int argc, char *argv[])
{
#ifdef __APPLE__
	/* For IOS and MAC */
    g_path = QStandardPaths::standardLocations(QStandardPaths::DataLocation).value(0);
    QDir dir(g_path);
    if (!dir.exists())
    {
        dir.mkpath(g_path);
    }
    
    if (!g_path.isEmpty() && !g_path.endsWith("/"))
    {
        g_path += "/";
        qDebug() << "APPLE is defined here..." << g_path;
    }
#endif

#if defined(Q_OS_IOS)
	/* For IOS only */
//	g_autoLock.setAutoLockDisable(); //To disable sleep mode in IOS, but it seems that the consumered power is huge.
#endif

    g_database.init();
//	g_pRegSetting = new YoudaoSetting("Rote.ini", QSettings::NativeFormat);  
	g_pRegSetting = new YoudaoSetting(g_path + "Rote.ini", QSettings::IniFormat, NULL);
//	g_pRegSetting = new YoudaoSetting("HKEY_CURRENT_USER\\Software\\YoudaoWord", QSettings::NativeFormat);  

	g_pBookInfo = new BookInfo();

    QApplication a(argc, argv);

#if 1
    static QTranslator *pTranslator = NULL;
    if (pTranslator != NULL)
    {
        qApp->removeTranslator(pTranslator);
        delete pTranslator;
        pTranslator = NULL;
    }
    pTranslator = new QTranslator;
    QString qmFileName = ":Rote_zh.qm";
    if (pTranslator->load(qmFileName))
    {
        qApp->installTranslator(pTranslator);
    }
#endif

#if 0
	QFile qss(":Rote.qss");
    qss.open(QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());
    qss.close();
#else

    QScreen *pScreen = a.primaryScreen ();
	QFont f = a.font();
	int fontPixel = f.pixelSize();
	if (fontPixel == -1)
	{
		fontPixel = f.pointSize() * pScreen->logicalDotsPerInch() / 72; // Normally we develop our application at default DPI = 72, pixelSize = 16.  
	}
	qDebug() << "Screen DPI = " << pScreen->logicalDotsPerInch() << ", default font point = " << f.pointSize() << ", fontPixel = " << fontPixel;
	setApplicationStyleSheet(":Rote.qss", fontPixel * 100 / 16);
#endif

    MainWindow w;
	g_pMainWindow = &w;
//    w.showMaximized();
	w.show();

    return a.exec();
}

void setApplicationStyleSheet(QString fileName, int fontScalePercent)
{
	QFile qss(":Rote.qss");
    qss.open(QFile::ReadOnly);
	QByteArray lines;
	while (true)
	{
		QByteArray line = qss.readLine();
		if (line.size() == 0)
		{
			break;
		}
		QString lineString = (QString)line;
		QRegExp re("Font: (\\d+)px");
		int pos = re.indexIn(lineString);
		if (pos > -1)
		{
			QString value = re.cap(1);
			int fontSize = value.toInt();
			QString newValue = QString::number((int)(fontSize * fontScalePercent / 100));
			lineString.replace(value, newValue);
//			qDebug() << line << lineString;
			lines += lineString.toUtf8();
		}
		else
		{
			lines += line;
		}
	}
    qss.close();

	qApp->setStyleSheet(lines);
}