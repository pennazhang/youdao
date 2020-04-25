#pragma once
#include <QWidget>
#include <QSettings>
#include <QDate>

class YoudaoSetting : public QSettings
{
public:
	YoudaoSetting(const QString &fileName, Format format, QObject *parent = 0);
	~YoudaoSetting(void);

	QString getCommentFromReg(int index);
	void setCommentFromReg(int index, QString strValue);

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

	int getWordCountFromReg();
	void setWordCountFromReg(int count);

	int getLevel1CountFromReg();
	void setLevel1CountFromReg(int count);

	bool getLevel1SelectEnableFromReg();
	void setLevel1SelectEnableFromReg(bool flag);

	bool getMostFamilityFlagFromReg();
	void setMostFamilityFlagFromReg(bool flag);

	int getBookCount();
	void setBookCount(int index);
	QString getBookFileName(int index);
	void setBookFileName(int index, QString strBookFileName);

	QDate getLastDate();
	void setLastDate(QDate lastDate);
};



extern YoudaoSetting * g_pRegSetting;

