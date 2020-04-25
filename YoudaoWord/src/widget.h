#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QResizeEvent>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QSettings>
#include <QXmlStreamReader>
#include <QDate>
#include <QSqlDatabase>
#include <QTabWidget>
#include <QFrame>
#include "ImportWidget.h"
#include "ExportWidget.h"
#include "YoudaoDatabase.h"
#include "WordWidget.h"
#include "DeviceWidget.h"
#include "MiscWidget.h"

#define SOFTWARE_RELEASE_VERSION "2.0.7.0"

namespace Ui {
class Widget;
}


class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
	QString getCurrentDBName() { return m_pFileName->text(); }
	bool getExportSetting(int &totalCount, bool &l1Flag, bool&l2Flag, int &l0Count, int &l1Count, int &l2Count, int &highestLevel);
	bool setExportSetting(int totalCount, bool l1Flag, bool l2Flag, int l0Count, int l1Count, int l2Count, int highestLevel);
	bool selectDatabase();
	bool AddRecordToFile(QString dbFileName);
	void deleteWord(QString word);
	bool editWord(QString word, QString &newWord, QString &phonetic, QString &newTrans);

protected slots:
	void onRestoreDB();
	void onImportXML();
	void onExportXML();
	void onBrowse();
	void onCheck();
	void onPrioritySetting();
	void onNewDatabase();
	void onSelectDatabase();
	void onReminder1Clicked();
	void onRadio1Changed(bool bFlag);
	void onRadio2Changed(bool bFlag);
	void onRadio3Changed(bool bFlag);
	void onFileNameChanged(const QString &text);
	void onViewWords();
	void onExportWords();
	void onSelectBrief();
	void onPreview();
	void onUnmark();

protected:
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent * event);
	bool readWordElement(QXmlStreamReader *pReader);
	bool readNextStartElement(QXmlStreamReader *pReader);
	bool readNextEndElement(QXmlStreamReader *pReader);
	bool readNextCharacter(QXmlStreamReader *pReader);
	bool ExportToXML(QString fileName, YoudaoWord *pWord, int wordCount);
	bool nativeEvent(const QByteArray & eventType, void * message, long * result);
	void about();
	void onRadioChanged(int i, bool bFlag);
	void setCurrentDB(QString strDBFileName);
	void browse(bool showBrwoseDialog);

private:
//    QPushButton *m_pDeleteButton, *m_pImportButton, *m_pExportButton;
	QPushButton *m_pSelectDBButton, *m_pNewDBButton, *m_pBriefButton, *m_pPreviewButton, *m_pUnmarkButton;
    QRadioButton *m_pRadioButton1, *m_pRadioButton2, *m_pRadioButton3;
    QLabel *m_pDataBaseLabel;

    QLineEdit *m_pReminder1, *m_pReminder2, *m_pReminder3;
    QLineEdit *m_pFileName;

	YoudaoDatabase m_database;
	int m_nCurrentBookIndex;

	QTabWidget *m_pTabWidget;
	ExportWidget *m_pExportWidget;
	ImportWidget *m_pImportWidget;
	WordWidget *m_pWordWidget;
	DeviceWidget *m_pDeviceWidget;
	MiscWidget *m_pMiscWidget;

	QLabel *m_pReminderLabel;
	QTextEdit *m_pBriefEdit;

	QLabel *m_pTotalLabel, *m_pLevelLabel0, *m_pLevelLabel1, *m_pLevelLabel2, *m_pLevelLabel3, *m_pLevelLabel4, *m_pLevelLabel5, *m_pLevelLabel6, *m_pLevelLabel7;
	QLineEdit * m_pTotalEdit, *m_pLevelEdit0, *m_pLevelEdit1, *m_pLevelEdit2, *m_pLevelEdit3, *m_pLevelEdit4, *m_pLevelEdit5, *m_pLevelEdit6, *m_pLevelEdit7;

	QLabel *m_pEmphasisLabel;
	QLineEdit *m_pEmphasisCountEdit;
	QFrame *m_pLineFrame1, *m_pLineFrame2;
};

extern Widget * g_pMainWidget;

extern YoudaoWord g_word[100000];
extern int g_wordNum;

#endif // WIDGET_H
