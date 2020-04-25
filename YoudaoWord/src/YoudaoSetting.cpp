#include "YoudaoSetting.h"
#include <QDir>

YoudaoSetting * g_pRegSetting;


YoudaoSetting::YoudaoSetting(const QString &fileName, Format format, QObject *parent ) :
	QSettings(fileName, format, parent)
{

}



YoudaoSetting::~YoudaoSetting(void)
{
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

	if ((ret < 1) || (ret > 3))
	{
		ret = 1;
	}
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

QString YoudaoSetting::getBackupDir()
{
	QString strKey("BackupDir");
	
	QString strValue = value(strKey, QString(".")).toString(); 
	return (strValue);
}

void YoudaoSetting::setBackupDir(QString backupDir)
{
	QString strKey("BackupDir");
	
	setValue(strKey, backupDir);
}