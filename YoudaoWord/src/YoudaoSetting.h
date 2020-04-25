#pragma once
#include <QWidget>
#include <QSettings>

class YoudaoSetting : public QSettings
{
public:
	YoudaoSetting(const QString &fileName, Format format, QObject *parent = 0);
	~YoudaoSetting(void);

	QString getDBNameFromReg(int index);
	void setDBNameFromReg(int index, QString strDBName);

	int getSelectionFromReg();
	void setSelectionFromReg(int selection);

	QString getLastImportFromReg();
	void setLastImportFromReg(QString strImport);

	QString getLastExportFromReg();
	void setLastExportFromReg(QString strImport);

	QString getNewDBFromReg();
	void setNewDBFromReg(QString strNewDB);

	QString getBackupDir();
	void setBackupDir(QString backupDir);
};

extern YoudaoSetting * g_pRegSetting;