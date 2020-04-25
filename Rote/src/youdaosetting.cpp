#include "YoudaoSetting.h"
#include <QDir>
#include <QBuffer>
#include <QDebug>

YoudaoSetting * g_pRegSetting;

YoudaoSetting::YoudaoSetting(const QString &fileName, Format format, QObject *parent ) :
	QSettings(fileName, format, parent)
{

}



YoudaoSetting::~YoudaoSetting(void)
{
}


QString YoudaoSetting::getCommentFromReg(int index)
{
	QString strKey;
	strKey.sprintf("Comment %d", index);
	
	QString strValue = value(strKey, QString("")).toString(); 
	return (strValue);
}

void YoudaoSetting::setCommentFromReg(int index, QString strValue)
{
	QString strKey;
	strKey.sprintf("Comment %d", index);

	setValue(strKey, strValue);
}


QString YoudaoSetting::getDBNameFromReg(int index)
{
	QString strKey;
	strKey.sprintf("DBName %d", index);
	
	QString strValue = value(strKey, QString("")).toString(); 
	return (strValue);
}

void YoudaoSetting::setDBNameFromReg(int index, QString strDBName)
{
	QString strKey;
	strKey.sprintf("DBName %d", index);
	
	setValue(strKey, strDBName);
}

int YoudaoSetting::getSelectionFromReg()
{
	QString strKey("Selection");
	
	QString strValue = value(strKey, QString("0")).toString();
	int ret = strValue.toInt();
	return (ret);
}

void YoudaoSetting::setSelectionFromReg(int nSelect)
{
	QString strKey("Selection");
	QString	strValue;
	strValue.sprintf("%d", nSelect);
	
	setValue(strKey, strValue);
}

QString YoudaoSetting::getLastImportFromReg()
{
	QString strKey("Import XML");
	
	QString strValue = value(strKey, QString(".")).toString(); 
	return (strValue);
}

void YoudaoSetting::setLastImportFromReg(QString strImport)
{
	QString strKey("Import XML");
	
	setValue(strKey, strImport);
}

QString YoudaoSetting::getLastExportFromReg()
{
	QString strKey("Export XML");
	
	QString strValue = value(strKey, QString(".")).toString(); 
	return (strValue);
}

void YoudaoSetting::setNewDBFromReg(QString strNewDB)
{
	QString strKey("NewDB");
	
	QDir d = QFileInfo(strNewDB).absoluteDir();
	QString dirName = d.absolutePath();
	setValue(strKey, dirName);
}

QString YoudaoSetting::getNewDBFromReg()
{
	QString strKey("NewDB");
	
	QString strValue = value(strKey, QString(".")).toString() + "/NewBook.db"; 
	return (strValue);
}


void YoudaoSetting::setLastExportFromReg(QString strImport)
{
	QString strKey("Export XML");
	
	setValue(strKey, strImport);
}

int YoudaoSetting::getWordCountFromReg()
{
	QString strKey("Word Count");
	
	QString strValue = value(strKey, QString("40")).toString();
	int ret = strValue.toInt();

	return (ret);
}

void YoudaoSetting::setWordCountFromReg(int count)
{
	QString strKey("Word Count");
	QString	strValue;
	strValue.sprintf("%d", count);
	
	setValue(strKey, strValue);
}

int YoudaoSetting::getLevel1CountFromReg()
{
	QString strKey("Level1 Count");
	
	QString strValue = value(strKey, QString("20")).toString();
	int ret = strValue.toInt();

	return (ret);
}

void YoudaoSetting::setLevel1CountFromReg(int count)
{
	QString strKey("Level1 Count");
	QString	strValue;
	strValue.sprintf("%d", count);
	
	setValue(strKey, strValue);
}

bool YoudaoSetting::getLevel1SelectEnableFromReg()
{
	QString strKey("Level1 Select Enable");
	
	QString strValue = value(strKey, QString("0")).toString();
	int ret = strValue.toInt();

	return (ret == 1);
}

void YoudaoSetting::setLevel1SelectEnableFromReg(bool flag)
{
	QString strKey("Level1 Select Enable");
	QString	strValue;
	strValue.sprintf("%d", (int)flag);
	
	setValue(strKey, strValue);
}

bool YoudaoSetting::getMostFamilityFlagFromReg()
{
	QString strKey("Most Famility Flag");
	
	QString strValue = value(strKey, QString("0")).toString();
	int ret = strValue.toInt();

	return (ret == 1);
}

void YoudaoSetting::setMostFamilityFlagFromReg(bool flag)
{
	QString strKey("Most Famility Flag");
	QString	strValue;
	strValue.sprintf("%d", (int)flag);
	
	setValue(strKey, strValue);
}

int YoudaoSetting::getBookCount()
{
	QString strKey = QString("bookCount");

	QString strValue = value(strKey, QString("0")).toString();
	int ret = strValue.toInt();
	return (ret);
}

void YoudaoSetting::setBookCount(int bookCount)
{
	QString strKey = QString("bookCount");

	QString	strValue;
	strValue.sprintf("%d", (int)bookCount);
	
	setValue(strKey, strValue);
}

QString YoudaoSetting::getBookFileName(int index)
{
	QString strKey = QString("book") + QString::number(index);
	QString strBookFileName = value(strKey, QString("")).toString();
	qDebug() << "getBookFileName : " << index << " " << strBookFileName;

	return (strBookFileName);
}

void YoudaoSetting::setBookFileName(int index, QString strBookFileName)
{
	qDebug() << "setBookFileName : " << index << " " << strBookFileName;
	QString strKey = QString("book") + QString::number(index);
	setValue(strKey, strBookFileName);
}

QDate YoudaoSetting::getLastDate()
{
	QString lastDate = value("LastDate", QString("")).toString();
	if (lastDate.isEmpty())
	{
		return (QDate(2017, 3, 25));
	}
	else
	{
		return (QDate::fromString(lastDate));
	}
}

void YoudaoSetting::setLastDate(QDate lastDate)
{
	setValue("LastDate", lastDate.toString());
}

