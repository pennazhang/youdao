#define NOMINMAX
#include "windows.h"
#include "widget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QXMLStreamReader>
#include <QDebug>
#include <QtXml>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include "YoudaoSetting.h"
#include "PrioritySettingDialog.h"
#include <QFileInfo>
#include "BrowseDialog.h"
#include "YoudaoDatabase.h"
#include "WordEditDialog.h"
#include <QAxObject>

#define IDM_ABOUTBOX	0x100

Widget * g_pMainWidget;

YoudaoWord g_word[100000];
int g_wordNum = 0;

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
	g_pMainWidget = this;

	/* New Database */
	m_pNewDBButton = new QPushButton(tr("New Database"), this);
	connect(m_pNewDBButton, SIGNAL(clicked()), this, SLOT(onNewDatabase()));
    
	m_pSelectDBButton = new QPushButton(tr("..."), this);
	connect(m_pSelectDBButton, SIGNAL(clicked()), this, SLOT(onSelectDatabase()));

    m_pRadioButton1 = new QRadioButton(this);
    m_pRadioButton2 = new QRadioButton(this);
    m_pRadioButton3 = new QRadioButton(this);

	m_pBriefButton = new QPushButton(tr("Brief"), this);
	connect(m_pBriefButton, SIGNAL(clicked()), this, SLOT(onSelectBrief()));

	connect(m_pRadioButton1, SIGNAL(toggled(bool)), this, SLOT(onRadio1Changed(bool)));
	connect(m_pRadioButton2, SIGNAL(toggled(bool)), this, SLOT(onRadio2Changed(bool)));
	connect(m_pRadioButton3, SIGNAL(toggled(bool)), this, SLOT(onRadio3Changed(bool)));

	connect(m_pRadioButton1, SIGNAL(clickd()), this, SLOT(onReminder1Clicked()));

    m_pDataBaseLabel = new QLabel(tr("Select your archived Database:"), this);

    m_pFileName = new QLineEdit(this);
	m_pFileName->setEnabled(true);
	m_pFileName->setReadOnly(true);
	connect(m_pFileName, SIGNAL(textChanged(const QString &)), this, SLOT(onFileNameChanged(const QString &)));

    m_pReminder1 = new QLineEdit(this);
	m_pReminder1->setEnabled(FALSE);
	m_pReminder1->setObjectName(QStringLiteral("DisableEdit"));
	m_pReminder1->setReadOnly(true);

    m_pReminder2 = new QLineEdit(this);
	m_pReminder2->setEnabled(FALSE);
	m_pReminder2->setObjectName(QStringLiteral("DisableEdit"));
	m_pReminder2->setReadOnly(true);

	m_pReminder3 = new QLineEdit(this);
	m_pReminder3->setEnabled(FALSE);
	m_pReminder3->setObjectName(QStringLiteral("DisableEdit"));
	m_pReminder3->setReadOnly(true);

	m_pTabWidget = new QTabWidget(this);
	m_pExportWidget = new ExportWidget(m_pTabWidget);
	m_pImportWidget = new ImportWidget(m_pTabWidget);
	m_pWordWidget = new WordWidget(m_pTabWidget);
	m_pDeviceWidget = new DeviceWidget(m_pTabWidget);
	m_pMiscWidget = new MiscWidget(m_pTabWidget);

	m_pTabWidget->addTab(m_pImportWidget, tr("Import XML"));
	m_pTabWidget->addTab(m_pExportWidget, tr("Export XML"));
	m_pTabWidget->addTab(m_pWordWidget, tr("Words"));
	m_pTabWidget->addTab(m_pDeviceWidget, tr("Device"));
	m_pTabWidget->addTab(m_pMiscWidget, tr("Misc"));

//	m_pCommentLabel = new QLabel(tr("Brief:"), this);
	m_pReminderLabel = new QLabel(tr("Famility Level: "), this);
	m_pTotalLabel = new QLabel(tr("Total Count:"), this);
	m_pTotalLabel->setAlignment(Qt::AlignCenter);
	m_pLevelLabel0 = new QLabel(tr("Level 0:"), this);
	m_pLevelLabel0->setAlignment(Qt::AlignRight);
	m_pLevelLabel1 = new QLabel(tr("Level 1:"), this);
	m_pLevelLabel1->setAlignment(Qt::AlignRight);
	m_pLevelLabel2 = new QLabel(tr("Level 2:"), this);
	m_pLevelLabel2->setAlignment(Qt::AlignRight);
	m_pLevelLabel3 = new QLabel(tr("Level 3:"), this);
	m_pLevelLabel3->setAlignment(Qt::AlignRight);
	m_pLevelLabel4 = new QLabel(tr("Level 4:"), this);
	m_pLevelLabel4->setAlignment(Qt::AlignRight);
	m_pLevelLabel5 = new QLabel(tr("Level 5:"), this);
	m_pLevelLabel5->setAlignment(Qt::AlignRight);
	m_pLevelLabel6 = new QLabel(tr("Level 6:"), this);
	m_pLevelLabel6->setAlignment(Qt::AlignRight);
	m_pLevelLabel7 = new QLabel(tr("Level 7+:"), this);
	m_pLevelLabel7->setAlignment(Qt::AlignRight);

	m_pEmphasisLabel = new QLabel(tr("Emphasis:"), this);
	m_pEmphasisCountEdit = new QLineEdit("23", this);
	m_pEmphasisCountEdit->setEnabled(false);

	m_pPreviewButton = new QPushButton(tr("Preview"), this);
	connect(m_pPreviewButton, SIGNAL(clicked()), this, SLOT(onPreview()));

	m_pUnmarkButton = new QPushButton(tr("Unmark"), this);
	connect(m_pUnmarkButton, SIGNAL(clicked()), this, SLOT(onUnmark()));
	
//	m_pEmphasisLabel->setAlignment(Qt

//	m_pTotalEdit = new QLineEdit(this);
//	m_pTotalEdit->setObjectName(QStringLiteral("DisableEdit"));
//	m_pTotalEdit->setEnabled(false);
	
	m_pLevelEdit0 = new QLineEdit(this);
	m_pLevelEdit0->setEnabled(false);

	m_pLevelEdit1 = new QLineEdit(this);
	m_pLevelEdit1->setEnabled(false);

	m_pLevelEdit2 = new QLineEdit(this);
	m_pLevelEdit2->setEnabled(false);

	m_pLevelEdit3 = new QLineEdit(this);
	m_pLevelEdit3->setEnabled(false);

	m_pLevelEdit4 = new QLineEdit(this);
	m_pLevelEdit4->setEnabled(false);

	m_pLevelEdit5 = new QLineEdit(this);
	m_pLevelEdit5->setEnabled(false);

	m_pLevelEdit6 = new QLineEdit(this);
	m_pLevelEdit6->setEnabled(false);

	m_pLevelEdit7 = new QLineEdit(this);
	m_pLevelEdit7->setEnabled(false);

	m_pBriefEdit = new QTextEdit(this);
	m_pBriefEdit->setEnabled(false);
	m_pBriefEdit->setObjectName(QStringLiteral("m_pBriefEdit"));

	m_pLineFrame1 = new QFrame(this);
	m_pLineFrame1->setObjectName(QStringLiteral("m_pLineFrame"));
    m_pLineFrame1->setFrameShape(QFrame::HLine);

	m_pLineFrame2 = new QFrame(this);
	m_pLineFrame2->setObjectName(QStringLiteral("m_pLineFrame"));
    m_pLineFrame2->setFrameShape(QFrame::HLine);

	setFixedSize(410, 620);

	int selection = g_pRegSetting->getSelectionFromReg();
	m_nCurrentBookIndex = selection - 1; 
	switch (selection)
	{
	case 1:
		m_pRadioButton1->setChecked(true);
		break;
	case 2:
		m_pRadioButton2->setChecked(true);
		break;
	case 3:
		m_pRadioButton3->setChecked(true);
		break;
	}

	QFileInfo fi;
	QString filePath, fileTitle;
	QLineEdit *pLineEdit[3] = {m_pReminder1, m_pReminder2, m_pReminder3 };

	for (int i = 0; i < 3; i++)
	{
		filePath = g_pRegSetting->getDBNameFromReg(i + 1);
		fi.setFile(filePath);
		if (fi.exists())
		{
			pLineEdit[i]->setText(fi.fileName());
		}
	}

	onFileNameChanged(m_pFileName->text());

#if 1
    HMENU hMenu = ::GetSystemMenu((HWND)winId(), FALSE);
	if (hMenu != NULL)
	{
		::AppendMenuA(hMenu, MF_SEPARATOR, 0, 0);
		::AppendMenuA(hMenu, MF_STRING, IDM_ABOUTBOX, "About MyApp...");
	}
#endif

}

bool Widget::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	MSG *m = (MSG *)message;
	if (m->message == WM_SYSCOMMAND)
	{
	   if ((m->wParam & 0xfff0) == IDM_ABOUTBOX)
	   {
	      *result = 0;
	
	      // open About dialog
	      about();
	
	      return (true);
	   }
	}
	return (QWidget::nativeEvent(eventType, message, result));
}

void Widget::about()
{
	QString version = QString(tr("Release Version: %1.    Release Time: %2.").arg(SOFTWARE_RELEASE_VERSION).arg(__DATE__));
	QMessageBox::information(this, tr("About YoudaoWord"), QString(tr("<h3>This application is written for Jacky Zhang, my dear son!</h3>\n")) + version, "OK");
}

Widget::~Widget()
{

}

void Widget::closeEvent(QCloseEvent *)
{
	m_pExportWidget->saveExportSetting();

	if (m_pRadioButton1->isChecked())
	{
		g_pRegSetting->setSelectionFromReg(1);
		g_pRegSetting->setDBNameFromReg(1, m_pFileName->text());
	}
	else if (m_pRadioButton2->isChecked())
	{
		g_pRegSetting->setSelectionFromReg(2);
		g_pRegSetting->setDBNameFromReg(2, m_pFileName->text());
	}
	else if (m_pRadioButton3->isChecked())
	{
		g_pRegSetting->setSelectionFromReg(3);
		g_pRegSetting->setDBNameFromReg(3, m_pFileName->text());
	}
}

void Widget::resizeEvent(QResizeEvent *)
{
    m_pRadioButton1->setGeometry(20, 50, 82, 17);
    m_pRadioButton2->setGeometry(20, 80, 82, 17);
    m_pRadioButton3->setGeometry(20, 110, 82, 17);

    m_pDataBaseLabel->setGeometry(20, 20, 181, 16);
    m_pNewDBButton->setGeometry(310, 20, 81, 23);

    m_pSelectDBButton->setGeometry(20, 140, 21, 20);
    m_pFileName->setGeometry(41, 140, 350, 20);
    m_pReminder1->setGeometry(40, 50, 351, 20);
    m_pReminder2->setGeometry(40, 80, 351, 20);
    m_pReminder3->setGeometry(40, 110, 351, 20);

	m_pTabWidget->setGeometry(20, 385, 371, 221);

	m_pReminderLabel->setGeometry(30, 245, 81, 16);

	m_pTotalLabel->setGeometry(200, 245, 171, 16);
	m_pTotalLabel->setAlignment(Qt::AlignRight);

	m_pLevelLabel0->setGeometry(30, 270, 50, 16);
	m_pLevelLabel1->setGeometry(120, 270, 50, 16);
	m_pLevelLabel2->setGeometry(210, 270, 50, 16);
	m_pLevelLabel3->setGeometry(300, 270, 50, 16);

	m_pLevelLabel4->setGeometry(30, 300, 50, 16);
	m_pLevelLabel5->setGeometry(120, 300, 50, 16);
	m_pLevelLabel6->setGeometry(210, 300, 50, 16);
	m_pLevelLabel7->setGeometry(300, 300, 50, 16);

	m_pLevelEdit0->setGeometry(80, 270, 36, 20);
	m_pLevelEdit1->setGeometry(170, 270, 36, 20);
	m_pLevelEdit2->setGeometry(260, 270, 36, 20);
	m_pLevelEdit3->setGeometry(350, 270, 36, 20);
	m_pLevelEdit4->setGeometry(80, 300, 36, 20);
	m_pLevelEdit5->setGeometry(170, 300, 36, 20);
	m_pLevelEdit6->setGeometry(260, 300, 36, 20);
	m_pLevelEdit7->setGeometry(350, 300, 36, 20);

	m_pLineFrame1->setGeometry(30, 330, width() - 60, 1);

	m_pEmphasisLabel->setGeometry(30, 340, 70, 20);
	m_pEmphasisCountEdit->setGeometry(100, 340, 36, 20);
	m_pPreviewButton->setGeometry(220, 340, 80, 23);
	m_pUnmarkButton->setGeometry(310, 340, 80, 23);

	m_pLineFrame2->setGeometry(30, 370, width() - 60, 1);

	m_pBriefButton->setGeometry(20, 170, 50, 23);
	m_pBriefEdit->setGeometry(80, 170, 311, 65);
}

void Widget::setCurrentDB(QString dbFile)
{
	QLineEdit *pLineEdit[] = { m_pReminder1, m_pReminder2, m_pReminder3 };

	m_pFileName->setText(dbFile);
	QFileInfo fi(dbFile);
	pLineEdit[m_nCurrentBookIndex]->setText(fi.fileName());
	int select = 1;
	if (m_pRadioButton1->isChecked())
	{
		select = 1;
	}
	else if (m_pRadioButton2->isChecked())
	{
		select = 2;
	}
	else if (m_pRadioButton3->isChecked())
	{
		select = 3;
	}
	else
	{
		m_pRadioButton1->setChecked(true);
	}
	g_pRegSetting->setDBNameFromReg(select, dbFile);
}

void Widget::onSelectDatabase()
{

	QString strDBName = QFileDialog::getOpenFileName(
		this,
		tr("Select archived database:"),
		m_pFileName->text(),
		"Youdao DB(*.db);; All files (*.*)"
	);

	if (strDBName.length() != 0)
	{
		if (m_database.isValidDB(strDBName) && m_database.getPriority(strDBName, g_param1, g_param2))
		{
			setCurrentDB(strDBName);
		}
		else
		{
			QMessageBox::warning(this, tr("Invalid Database file"), strDBName + QString(tr(" is not a valid database file!")), "OK");
		}
	}
}

void Widget::onNewDatabase()
{
	QString strLog;

	QString fileName = QFileDialog::getSaveFileName(
		this,
		tr("New Database"),
		g_pRegSetting->getNewDBFromReg(),
		"Youdao DB(*.db);; All files (*.*)"
	);

	if (fileName.length() == 0)
	{
		return;
	}
	if (QFile::exists(fileName))
	{
		QFile::remove(fileName);
	}

	g_pRegSetting->setNewDBFromReg(fileName);

	if (!m_database.newDB(fileName))
	{
		QMessageBox::warning(this, tr("New DataBase failed"), tr("Failed to create new datebase"), "OK");
		return;
	}
	else
	{
		QMessageBox::information(this, tr("New Database success"), QString(tr("The database: %1 is created successfully!")).arg(fileName), "OK");
	}

	setCurrentDB(fileName);
//	onFileNameChanged(m_pFileName->text());
}

void Widget::onReminder1Clicked()
{
	qDebug() << "onReminder1Clicked called here...";
}

void Widget::onSelectBrief()
{
	if (m_pBriefEdit->isEnabled())
	{
		QString fileName = m_pFileName->text();
		m_database.setBrief(fileName, m_pBriefEdit->toPlainText());
		m_pBriefEdit->setEnabled(false);
		m_pBriefButton->setText(tr("Brief"));
	}
	else
	{
		m_pBriefEdit->setEnabled(true);
		m_pBriefButton->setText(tr("Save"));
	}
}

void Widget::onRadio1Changed(bool bFlag)
{
	onRadioChanged(1, bFlag);
	m_pReminder1->setEnabled(bFlag);
}

void Widget::onRadio2Changed(bool bFlag)
{
	onRadioChanged(2, bFlag);
	m_pReminder2->setEnabled(bFlag);
}

void Widget::onRadio3Changed(bool bFlag)
{
	onRadioChanged(3, bFlag);
	m_pReminder3->setEnabled(bFlag);
}

// i = 1, 2, 3
void Widget::onRadioChanged(int i, bool bFlag)
{
	QString strDBName;

	m_nCurrentBookIndex = i - 1;
	if (bFlag == true)
	{
		strDBName = g_pRegSetting->getDBNameFromReg(i);
		if (QFile::exists(strDBName) && m_database.isValidDB(strDBName) && m_database.getPriority(strDBName, g_param1, g_param2))
		{
			m_pFileName->setText(strDBName);
		}
		else
		{
			m_pFileName->setText("");
			QLineEdit *pLineEdit[] = { m_pReminder1, m_pReminder2, m_pReminder3 };
			pLineEdit[m_nCurrentBookIndex]->setText("");
		}
	}
	else
	{
		strDBName = m_pFileName->text();
		if (QFile::exists(strDBName))
		{
			g_pRegSetting->setDBNameFromReg(i, strDBName);
		}
	}
}

void Widget::onFileNameChanged(const QString &fileName)
{
	int nTotalCount, level0, level1, level2, level3, level4, level5, level6, above7, totalScore = 0;

	if (fileName.length() != 0)
	{
		m_database.getDBSummary(fileName, nTotalCount, level0, level1, level2, level3, level4, level5, level6, above7, totalScore);
		m_pTotalLabel->setText(tr("total: %1 - %2").arg(nTotalCount).arg(totalScore));
		m_pLevelEdit0->setText(QString::number(level0));
		m_pLevelEdit1->setText(QString::number(level1));
		m_pLevelEdit2->setText(QString::number(level2));
		m_pLevelEdit3->setText(QString::number(level3));
		m_pLevelEdit4->setText(QString::number(level4));
		m_pLevelEdit5->setText(QString::number(level5));
		m_pLevelEdit6->setText(QString::number(level6));
		m_pLevelEdit7->setText(QString::number(above7));
		m_pImportWidget->setButtonEnable(true);
		m_pExportWidget->setButtonEnable(true);	
		m_pExportWidget->onEnterWidget();
		m_pImportWidget->setRestoreEnable(m_database.canRestoreDB(fileName));

		nTotalCount = m_database.getBrowseWordCount(fileName);
		m_pEmphasisCountEdit->setText(QString::number(nTotalCount));
		m_pPreviewButton->setEnabled(nTotalCount != 0);
		m_pUnmarkButton->setEnabled(nTotalCount != 0);

		QString brief;
		m_database.getBrief(fileName, brief);
		m_pBriefEdit->setText(brief);
	}
	else
	{
		m_pTotalLabel->setText(tr("total:"));
		m_pLevelEdit0->setText("");
		m_pLevelEdit1->setText("");
		m_pLevelEdit2->setText("");
		m_pLevelEdit3->setText("");
		m_pLevelEdit4->setText("");
		m_pLevelEdit5->setText("");
		m_pLevelEdit6->setText("");
		m_pLevelEdit7->setText("");
		m_pImportWidget->setButtonEnable(false);
		m_pExportWidget->setButtonEnable(false);	
		m_pImportWidget->setRestoreEnable(false);
		m_pPreviewButton->setEnabled(false);
		m_pUnmarkButton->setEnabled(false);

		m_pBriefEdit->setText("");
		m_pEmphasisCountEdit->setText("");
	}
}

void Widget::onPrioritySetting()
{
	if (!m_database.getPriority(m_pFileName->text(), g_param1, g_param2))
	{
		QMessageBox::warning(this, tr("Can't set priority"), tr("Please select another database"), "OK");
	}
	PrioritySettingDialog settingDlg(this);
	settingDlg.setModal(true);
	settingDlg.setA(g_param1);
	settingDlg.setB(g_param2);

	if (settingDlg.exec() == QDialog::Accepted)
	{
		g_param1 = settingDlg.getA();
		g_param2 = settingDlg.getB();

		m_database.setPriority(m_pFileName->text(), g_param1, g_param2);
	}
}

void Widget::onViewWords()
{
	QString dbFileName = m_pFileName->text();
	QStringList wordList = m_pWordWidget->getWordList();

	if (wordList.count() == 0)
	{
		return;
	}
	qDebug() << wordList;

	if (!m_database.exportWords(m_pFileName->text(), wordList, g_word, g_wordNum) || (g_wordNum == 0))
	{
		QMessageBox::warning(this, tr("View Words"), tr("Can't find any words from the database!"), tr("OK"));
		return;
	}

	g_roteManager.clear();
	for (int i = 0; i < g_wordNum; i++)
	{
		g_roteManager.addWord(g_word + i);
	}

	BrowseDialog browseDialog;
	browseDialog.exec();
	onFileNameChanged(m_pFileName->text());
	return;
}

void Widget::onExportWords()
{
	bool ret = false;
	QString dbFileName = m_pFileName->text();
	QStringList wordList = m_pWordWidget->getWordList();

	if (wordList.count() == 0)
	{
		return;
	}

	QString strInformation(tr("Do you want to export the following word(s) from current database?\n    "));
	int nTotalCount = wordList.count();
	if (nTotalCount > 7)
	{
		nTotalCount = 7;
	}

	for (int i = 0; i < nTotalCount; i++)
	{
		strInformation += QString("%1, ").arg(wordList.at(i));
	}
	if (wordList.count() > 7)
	{
		strInformation += " ...";
	}

	if (QMessageBox::warning(this, tr("Export Words"), strInformation, "Yes", "No") != 0)
	{
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(
		this,
		"Export to Youdao XML File",
		g_pRegSetting->getLastExportFromReg(),
		"Youdao XML File(*.xml);; All files (*.*)"
	);
	if (fileName.length() == 0)
	{
		return;
	}

	if (m_database.exportWords(m_pFileName->text(), wordList, g_word, g_wordNum))
	{
		if (ExportToXML(fileName, g_word, g_wordNum))
		{
			ret = true;
		}
	}

	int count[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	for (int i = 0; i < g_wordNum; i++)
	{
		int progress = g_word[i].m_progress;
		if (progress <= 0)
		{
			progress = 0;
		}
		if (progress >= 7)
		{
			progress = 7;
		}

		count[progress]++;
	}

	if (ret == true)
	{
		QMessageBox::information(this, tr("Export Success"), QString(tr("The words have been exported into file: %1\n      Level 0 = %2, Level 1 = %3,    level 2 = %4,    level 3 = %5\n      level 4 = %6,    level 5 = %7,    level 6 = %8, above 7 = %9")).arg(fileName).arg(count[0], 3).arg(count[1], 3).arg(count[2], 3).arg(count[3], 3).arg(count[4], 3).arg(count[5], 3).arg(count[6], 3).arg(count[7], 3), "OK");
		g_pRegSetting->setLastExportFromReg(fileName);
	}
	else
	{
		QMessageBox::warning(this, tr("Export failed"), QString(tr("Failed to export the words!")), "OK");
	}

	onFileNameChanged(m_pFileName->text());
	return;
}

void Widget::onCheck()
{
	browse(false);
}

void Widget::onBrowse()
{
	browse(true);
}

void Widget::browse(bool showBrwoseDialog)
{
	QString strLog;
	int i;

	// We just add desc, and we need to use desc to do something here...

	int totalCount, l0Count, l1Count, l2Count, highestLevel;
	bool l1Flag, l2Flag;

	m_pExportWidget->saveExportSetting();
	QString dbFileName = m_pFileName->text();
	if (!m_database.getRoteSetting(dbFileName, totalCount, l1Flag, l2Flag, l0Count, l1Count, l2Count, highestLevel))
	{
		return;
	}
	if (l1Flag == false)
	{
		l1Count = 0;
	}
	if (l2Flag == false)
	{
		l2Count = 0;
	}

	int nTotalCount, level0, level1, level2, level3, level4, level5, level6, above7, totalScore;
	m_database.getDBSummary(dbFileName, nTotalCount, level0, level1, level2, level3, level4, level5, level6, above7, totalScore);
	if ((!m_database.LoadFromDBByPriority(dbFileName, g_word, g_wordNum, nTotalCount)) || (!m_database.getPriority(dbFileName, g_param1, g_param2)))
	{
		QMessageBox::warning(this, tr("Read database failed"), QString(tr("Can not read the database file: %1")).arg(dbFileName), "OK");
		return;
	}
	if (totalCount > nTotalCount)
	{
		totalCount = nTotalCount;
	}

	if (l0Count > level0)
	{
		l0Count = level0;
	}

	if (l1Count > level1)
	{
		l1Count = level1;
	}

	if (l2Count > level2)
	{
		l2Count = level2;
	}

	int othersCount = totalCount - l0Count - l1Count - l2Count;
	if (othersCount < 0)
	{
		othersCount = 0;
		l2Count = totalCount - l0Count - l1Count;
		if (l2Count < 0)
		{
			l2Count = 0;
			l1Count = totalCount - l0Count;
			if (l1Count < 0)
			{
				l1Count = 0;
				l0Count = totalCount;
			}
		}
	}

	YoudaoWord *pNewWordBook = new YoudaoWord[totalCount];
	int currentIndex = 0, l0Index = 0, l1Index = 0, l2Index = 0, otherIndex = 0;

	/* First we will get the word of Level 0  */
	for (i = 0; i < g_wordNum; i++)
	{
		int progress = g_word[i].m_progress;
		if (progress > highestLevel)
		{
			continue;
		}
		switch (progress)
		{
		case 0:
			if (l0Index < l0Count)
			{
				pNewWordBook[currentIndex] = g_word[i];
//				pNewWordBook[currentIndex].m_progress = 1;
				l0Index++;
				currentIndex++;
			}
			break;
		case 1:
			{
				/* Must import Level 1 words */
				if (l1Flag == true)
				{
					if (l1Index < l1Count)
					{
						pNewWordBook[currentIndex] = g_word[i];
						l1Index++;
						currentIndex++;						
					}
				}
				else
				{
					if (otherIndex < othersCount)
					{
						pNewWordBook[currentIndex] = g_word[i];
						otherIndex++;
						currentIndex++;											
					}
				}
			}
			break;
		case 2:
			{
				/* Must import Level 2 words */
				if (l2Flag == true)
				{
					if (l2Index < l2Count)
					{
						pNewWordBook[currentIndex] = g_word[i];
						l2Index++;
						currentIndex++;									
					}
				}
				else
				{
					if (otherIndex < othersCount)
					{
						pNewWordBook[currentIndex] = g_word[i];
						otherIndex++;
						currentIndex++;											
					}				
				}
			}
			break;
		default:
			if (otherIndex < othersCount)
			{
				pNewWordBook[currentIndex] = g_word[i];
				otherIndex++;
				currentIndex++;											
			}		
		}

		if ((l0Index >= l0Count) && (l1Index >= l1Count) && (l2Index >= l2Count) && (otherIndex >= othersCount))
		{
			break;
		}
	}

	g_wordNum = currentIndex;
	g_roteManager.clear();
	int level[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int count = currentIndex;
	for (i = 0; i < currentIndex; i++)
	{
		g_word[i] = pNewWordBook[i];
		g_roteManager.addWord(g_word + i);

		if (g_word[i].m_progress < 9)
		{
			level[g_word[i].m_progress]++;
		}
		else
		{
			level[9]++;
		}
	}
	delete []pNewWordBook;
	m_pExportWidget->setLevelCount(count, level);

	if (showBrwoseDialog == false)
	{
		return;
	}

	g_roteManager.setDBName(dbFileName);

	BrowseDialog browseDialog;
	browseDialog.exec();
}

void Widget::deleteWord(QString word)
{
	/*  */
	QStringList stringList;
	stringList.append(word);
	m_database.removeWord(m_pFileName->text(), stringList);
	m_database.deleteBrowseWord(m_pFileName->text(), word);

	onFileNameChanged(m_pFileName->text());
}

bool Widget::editWord(QString word, QString &newWord, QString &phoentic, QString &newTrans)
{
	QStringList wordList;
	int wordCount = 0;
	YoudaoWord pWord[1];

	wordList.append(word);
	m_database.exportWords(m_pFileName->text(), wordList, pWord, wordCount);
	if (wordCount != 1)
	{
		QMessageBox::warning(this, tr("Can't find the word"), tr("Can't find the word in the database :\n %1").arg(word), QMessageBox::Ok);
		return (false);
	}

	WordEditDialog dlg(this);
	dlg.setWord(word, pWord[0].m_phonetic, pWord[0].m_trans);
	dlg.setModal(true);

	if (dlg.exec() == QDialog::Accepted)
	{
		if (m_database.editWord(m_pFileName->text(), word, dlg.getWord(), dlg.getPhoentic(), dlg.getTrans()) == true)
		{
			newWord = dlg.getWord();
			phoentic = dlg.getPhoentic();
			newTrans = dlg.getTrans();
			return (true);
		}
		else
		{
			QMessageBox::warning(this, tr("Warning"), tr("Failed to modify the word"), tr("OK"));
		}
	}

	return (false);
}

void Widget::onExportXML()
{
	QString strLog;
	int i;

	// We just add desc, and we need to use desc to do something here...

	int totalCount, l0Count, l1Count, l2Count, highestLevel;
	bool l1Flag, l2Flag;

	m_pExportWidget->saveExportSetting();
	QString dbFileName = m_pFileName->text();
	if (!m_database.getRoteSetting(dbFileName, totalCount, l1Flag, l2Flag, l0Count, l1Count, l2Count, highestLevel))
	{
		return;
	}
	if (l1Flag == false)
	{
		l1Count = 0;
	}
	if (l2Flag == false)
	{
		l2Count = 0;
	}

	int nTotalCount, level0, level1, level2, level3, level4, level5, level6, above7, totalScore;
	m_database.getDBSummary(dbFileName, nTotalCount, level0, level1, level2, level3, level4, level5, level6, above7, totalScore);
	if ((!m_database.LoadFromDBByPriority(dbFileName, g_word, g_wordNum, nTotalCount)) || (!m_database.getPriority(dbFileName, g_param1, g_param2)))
	{
		QMessageBox::warning(this, tr("Read database failed"), QString(tr("Can not read the database file: %1")).arg(dbFileName), "OK");
		return;
	}
	if (totalCount > nTotalCount)
	{
		totalCount = nTotalCount;
	}

	if (l0Count > level0)
	{
		l0Count = level0;
	}

	if (l1Count > level1)
	{
		l1Count = level1;
	}

	if (l2Count > level2)
	{
		l2Count = level2;
	}

	int othersCount = totalCount - l0Count - l1Count - l2Count;
	if (othersCount < 0)
	{
		othersCount = 0;
		l2Count = totalCount - l0Count - l1Count;
		if (l2Count < 0)
		{
			l2Count = 0;
			l1Count = totalCount - l0Count;
			if (l1Count < 0)
			{
				l1Count = 0;
				l0Count = totalCount;
			}
		}
	}

	YoudaoWord *pNewWordBook = new YoudaoWord[totalCount];
	int currentIndex = 0, l0Index = 0, l1Index = 0, l2Index = 0, otherIndex = 0;

	/* First we will get the word of Level 0  */
	for (i = 0; i < g_wordNum; i++)
	{
		int progress = g_word[i].m_progress;
		if (progress > highestLevel)
		{
			continue;
		}

		switch (progress)
		{
		case 0:
			if (l0Index < l0Count)
			{
				pNewWordBook[currentIndex] = g_word[i];
//				pNewWordBook[currentIndex].m_progress = 1;
				l0Index++;
				currentIndex++;
			}
			break;
		case 1:
			{
				/* Must import Level 1 words */
				if (l1Flag == true)
				{
					if (l1Index < l1Count)
					{
						pNewWordBook[currentIndex] = g_word[i];
						l1Index++;
						currentIndex++;						
					}
				}
				else
				{
					if (otherIndex < othersCount)
					{
						pNewWordBook[currentIndex] = g_word[i];
						otherIndex++;
						currentIndex++;											
					}
				}
			}
			break;
		case 2:
			{
				/* Must import Level 2 words */
				if (l2Flag == true)
				{
					if (l2Index < l2Count)
					{
						pNewWordBook[currentIndex] = g_word[i];
						l2Index++;
						currentIndex++;									
					}
				}
				else
				{
					if (otherIndex < othersCount)
					{
						pNewWordBook[currentIndex] = g_word[i];
						otherIndex++;
						currentIndex++;											
					}				
				}
			}
			break;
		default:
			if (otherIndex < othersCount)
			{
				pNewWordBook[currentIndex] = g_word[i];
				otherIndex++;
				currentIndex++;											
			}		
		}

		if ((l0Index >= l0Count) && (l1Index >= l1Count) && (l2Index >= l2Count) && (otherIndex >= othersCount))
		{
			break;
		}
	}

	g_wordNum = currentIndex;
	for (i = 0; i < currentIndex; i++)
	{
		g_word[i] = pNewWordBook[i];
	}
	delete []pNewWordBook;


	QString fileName = QFileDialog::getSaveFileName(
		this,
		"Export to Youdao XML File",
		g_pRegSetting->getLastExportFromReg(),
		"Youdao XML File(*.xml);; All files (*.*)"
	);
	if (fileName.length() == 0)
	{
		return;
	}

	int count[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for (int i = 0; i < g_wordNum; i++)
	{
		int progress = g_word[i].m_progress;
		if (progress < 0)
		{
			progress = 0;
		}

		if (progress < 9)
		{
			count[progress]++;
		}
		else
		{
			count[9]++;
		}		
	}

	if (ExportToXML(fileName, g_word, g_wordNum) == false)
	{
		strLog = QString(tr("The words failed to be exported to file: ")) + fileName;
		QMessageBox::warning(this, tr("Export Failed"), strLog, "OK");
	}
	else
	{
		/* Now we can read the word */
		QMessageBox::information(this, tr("Export Success"), QString(tr("The words have been exported into file: %1\n      level 0 = %2,    Level 1 = %3,    level 2 = %4,    level 3 = %5\n      level 4 = %6,    level 5 = %7,    level 6 = %8,    level 7 = %9,    level 8 = %10,    level 9+ = %11")).arg(fileName).arg(count[0], 3).arg(count[1], 3).arg(count[2], 3).arg(count[3], 3).arg(count[4], 3).arg(count[5], 3).arg(count[6], 3).arg(count[7], 3).arg(count[8], 3).arg(count[9], 3), "OK");
		g_pRegSetting->setLastExportFromReg(fileName);
	}
}

bool Widget::ExportToXML(QString fileName, YoudaoWord *pWord, int wordCount)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) 
	{
		return false;
	}
	QTextStream stream(&file);
	QXmlStreamWriter writer;
	writer.setDevice(&file);
	// Write XML to file
	writer.writeStartElement("wordbook");
	for (int i = 0; i < wordCount; i++)
	{
		// <item>
		writer.writeCharacters("\n    ");
		writer.writeStartElement("item");

		// <word>
		writer.writeCharacters("\n        ");
		writer.writeStartElement("word");
		writer.writeCharacters(pWord[i].m_word);
		writer.writeEndElement();

		// <trans>
		writer.writeCharacters("\n        ");
		writer.writeStartElement("trans");
		writer.writeCharacters(pWord[i].m_trans);
		writer.writeEndElement();

		// <phonetic>
		writer.writeCharacters("\n        ");
		writer.writeStartElement("phonetic");
		writer.writeCharacters(pWord[i].m_phonetic);
		writer.writeEndElement();

		// <tags>
		writer.writeCharacters("\n        ");
		writer.writeStartElement("tags");
		writer.writeCharacters(pWord[i].m_tags);
		writer.writeEndElement();

		// <date>
		writer.writeCharacters("\n        ");
		writer.writeStartElement("date");
		writer.writeCharacters(QString("%1-%2-%3").arg(pWord[i].m_Date.year(), 4, 10, QChar('0')).arg(pWord[i].m_Date.month(), 2, 10, QChar('0')).arg(pWord[i].m_Date.day(), 2,  10, QChar('0')));
		writer.writeEndElement();

		// <progress>
		writer.writeCharacters("\n        ");
		writer.writeStartElement("progress");
		writer.writeCharacters(QString::number(pWord[i].m_progress));
		writer.writeEndElement();

		// </item>
		writer.writeCharacters("\n    ");
		writer.writeEndElement();
		writer.writeCharacters("\n");
	}
	writer.writeCharacters("\n");
	writer.writeEndElement();

	file.close();
	return true;
}

void Widget::onRestoreDB()
{
	int response = QMessageBox::warning(this, tr("Restore Confirm"), tr("Do you want to undo the last import, and restore to the database before last import?"), "Yes", "No");
	if (response == 0)
	{
		m_database.restoreDB(m_pFileName->text());
	}

	onFileNameChanged(m_pFileName->text());
}

bool Widget::selectDatabase()
{
	bool ret = false;
	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Add data records"),
		g_pRegSetting->getLastImportFromReg(),
		"Youdao XML File(*.xml);; Youdao Database File(*.db);; Excel File(*.xls *.xlsx);; All files (*.*)"
	);
	

	QString strLog;

	g_wordNum = 0;

	if (fileName.length() == 0)
	{
		return false;
	}

	QString extension = QFileInfo(fileName).suffix();

	/* We will add XML File here */
	if (extension.toLower() == QString("xml"))
	{
		QFile file(fileName);
		if (!file.open(QFile::ReadOnly | QFile::Text)) 
		{        
			strLog = QString("Cannot read file: ") + fileName + QString(", ") + file.errorString();
			QMessageBox::warning(this, "Error", strLog, "OK");
			return false;
		}

		QXmlStreamReader reader;
		reader.setDevice(&file);

		// <wordbook>
		if (readNextStartElement(&reader) == true)
		{ 
			if (reader.name() == "wordbook")
			{
				for (;;)
				{
					// <item>
					if (!readNextStartElement(&reader) || (reader.name() != "item"))
					{
						break;
					}

					// Word Element
					if (readWordElement(&reader) != true)
					{
						break;
					}

					// <item>
					if (!readNextEndElement(&reader) || (reader.name() != "item"))
					{
						break;
					}
				}

				if (reader.isEndElement() && (reader.name() == "wordbook"))
				{
					// if the last element is </wordbook>, then everything is OK.
					ret = true;
				}
			}
		}
	
		file.close();
	}
	else if (extension.toLower() == QString("db"))
	{
		/* We will add DB File here */
		ret = m_database.LoadFromDB(fileName, g_word, g_wordNum);
	}
	else if ((extension.toLower() == QString("xlsx")) || (extension.toLower() == QString("xls")))
	{
		QString strMessage = QString(tr("Please prepare the first sheet of the wordbook as below:")) + QString("\n    ") 
			+ QString(tr("The first column is the word (not NULL)")) + QString("\n    ") 
			+ QString(tr("The second column is the explatation")) + QString("\n    ")
			+ QString(tr("The third column is the pronouncing")) + QString("\n    ")
			+ QString(tr("The fourth column is the data (2016-01-01)")) + QString("\n    ")
			+ QString(tr("The fifth column is the famility level (0 - 7)"));

		if (QMessageBox::warning(this, tr("Wordbook format"), strMessage, QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
		{
			QAxObject* excel = new QAxObject( "Excel.Application", 0 );
			QAxObject* workbooks = excel->querySubObject( "Workbooks" );
			QAxObject* workbook = workbooks->querySubObject( "Open(const QString&)", fileName );
			QAxObject* sheets = workbook->querySubObject( "Worksheets" );
	
			//worksheets count
//			int count = sheets->dynamicCall("Count()").toInt();
			int sheetIndex = 1;
			{
				//sheet pointer
				QAxObject* sheet = sheets->querySubObject( "Item( int )", sheetIndex);
	
//				QAxObject* rows = sheet->querySubObject( "Rows" );
//				int rowCount = rows->dynamicCall( "Count()" ).toInt(); //unfortunately, always returns 255,
																		// so you have to check somehow validity of cell values
//				QAxObject* columns = sheet->querySubObject( "Columns" );
//				int columnCount = columns->dynamicCall( "Count()" ).toInt(); //similarly, always returns 65535
	
				//One of possible ways to get column count
				int currentColumnIndex = 1;
				int rowIndex;
				for (rowIndex = 1; rowIndex < 65535; rowIndex++)
				{
					QAxObject* cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 1);	// column Index = 1 [word]
					QVariant value = cell->dynamicCall( "Value()" );
					if (value.toString().trimmed().isEmpty())
					{
						break;
					}
					g_word[rowIndex - 1].m_word = value.toString().trimmed();

					cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 2);		// column Index = 2 [Trans]
					value = cell->dynamicCall( "Value()" );
					g_word[rowIndex - 1].m_trans = value.toString().trimmed();

					cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 3);		// column Index = 3 [pronouncing]
					value = cell->dynamicCall( "Value()" );
					g_word[rowIndex - 1].m_phonetic = value.toString().trimmed();

					g_word[rowIndex - 1].m_Date = QDate::currentDate();
					cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 4);		// column Index = 4 [date]
					value = cell->dynamicCall( "Value()" );
					if (value.canConvert(Qt::ISODate) != true)
					{
						g_word[rowIndex - 1].m_Date = value.toDate();
					}

					cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 5);		// column Index = 5 [Famility Level]
					value = cell->dynamicCall( "Value()" );
					bool flag;
					g_word[rowIndex - 1].m_progress = value.toInt(&flag);
					if (!flag)
					{
						g_word[rowIndex - 1].m_progress = 1;
					}

					qDebug() << g_word[rowIndex - 1].m_Date.toString("yyyy-MM-dd");
					qDebug() << QString("%1:%2 = %3 %4 %5 %6").arg(rowIndex).arg(currentColumnIndex).arg(g_word[rowIndex - 1].m_word).arg(g_word[rowIndex - 1].m_progress).arg(g_word[rowIndex - 1].m_Date.toString("yyyy-MM-dd")).arg(g_word[rowIndex - 1].m_trans);
				}
				g_wordNum = rowIndex - 1;
			}
	
			workbook->dynamicCall("Close()");
			excel->dynamicCall("Quit()");
			if (g_wordNum == 0)
			{
				ret = false;
			}
			else
			{
				ret = true;
			}
		}
	}

	if (ret == false)
	{
		strLog = QString("Read file %1 failed: Invalid Youdao Database File!").arg(fileName);
		QMessageBox::warning(this, "Select Database  Error", strLog, "OK");
		return (false);
	}

	for (int index = 0; index < g_wordNum; index++)
	{
		if (g_word[index].m_progress == 1)
		{
			g_word[index].m_progress = 0;
		}
	}

	return (true);
}

bool Widget::AddRecordToFile(QString dbFileName)
{
	bool ret = m_database.addtoDB(dbFileName, g_word, g_wordNum, 0);
	return (ret);
}

void Widget::onImportXML()
{
	bool ret = false;
	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Import data records"),
		g_pRegSetting->getLastImportFromReg(),
		"Youdao XML File(*.xml);; Youdao Database File(*.db);; Excel File(*.xls *.xlsx);;All files (*.*)"
	);

	QString strLog;

	g_wordNum = 0;

	if (fileName.length() == 0)
	{
		return;
	}

	QString extension = QFileInfo(fileName).suffix();

	/* We will add XML File here */
	if (extension.toLower() == QString("xml"))
	{
		QFile file(fileName);
		if (!file.open(QFile::ReadOnly | QFile::Text)) 
		{        
			strLog = QString("Cannot read file: ") + fileName + QString(", ") + file.errorString();
			QMessageBox::warning(this, "Error", strLog, "OK");
			return;
		}

		QXmlStreamReader reader;
		reader.setDevice(&file);

		// <wordbook>
		if (readNextStartElement(&reader) == true)
		{ 
			if (reader.name() == "wordbook")
			{
				for (;;)
				{
					// <item>
					if (!readNextStartElement(&reader) || (reader.name() != "item"))
					{
						break;
					}

					// Word Element
					if (readWordElement(&reader) != true)
					{
						break;
					}

					// <item>
					if (!readNextEndElement(&reader) || (reader.name() != "item"))
					{
						break;
					}
				}

				if (reader.isEndElement() && (reader.name() == "wordbook"))
				{
					// if the last element is </wordbook>, then everything is OK.
					ret = true;
				}
			}
		}
	
		file.close();

		if (ret == false)
		{
			strLog = QString("Read file %1 failed: Invalid Youdao XML File!").arg(fileName);
			QMessageBox::warning(this, "Import Error", strLog, "OK");
		}
	}
	else if (extension.toLower() == QString("db"))
	{
		/* We will add DB File here */
		ret = m_database.LoadFromDB(fileName, g_word, g_wordNum);

		if (ret == false)
		{
			strLog = QString("Read file %1 failed: Invalid Youdao Database File!").arg(fileName);
			QMessageBox::warning(this, "Import Error", strLog, "OK");
		}
	}
	else if ((extension.toLower() == QString("xlsx")) || (extension.toLower() == QString("xls")))
	{
		QString strMessage = QString(tr("Please prepare the first sheet of the wordbook as below:")) + QString("\n    ") 
			+ QString(tr("The first column is the word (not NULL)")) + QString("\n    ") 
			+ QString(tr("The second column is the explatation")) + QString("\n    ")
			+ QString(tr("The third column is the pronouncing")) + QString("\n    ")
			+ QString(tr("The fourth column is the data (2016-01-01)")) + QString("\n    ")
			+ QString(tr("The fifth column is the famility level (0 - 7)"));

		if (QMessageBox::warning(this, tr("Wordbook format"), strMessage, QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
		{
			QAxObject* excel = new QAxObject( "Excel.Application", 0 );
			QAxObject* workbooks = excel->querySubObject( "Workbooks" );
			QAxObject* workbook = workbooks->querySubObject( "Open(const QString&)", fileName );
			QAxObject* sheets = workbook->querySubObject( "Worksheets" );
	
			//worksheets count
//			int count = sheets->dynamicCall("Count()").toInt();
			int sheetIndex = 1;
			{
				//sheet pointer
				QAxObject* sheet = sheets->querySubObject( "Item( int )", sheetIndex);
	
//				QAxObject* rows = sheet->querySubObject( "Rows" );
//				int rowCount = rows->dynamicCall( "Count()" ).toInt(); //unfortunately, always returns 255,
																		// so you have to check somehow validity of cell values
//				QAxObject* columns = sheet->querySubObject( "Columns" );
//				int columnCount = columns->dynamicCall( "Count()" ).toInt(); //similarly, always returns 65535
	
				//One of possible ways to get column count
				int currentColumnIndex = 1;
				int rowIndex;
				for (rowIndex = 1; rowIndex < 65535; rowIndex++)
				{
					QAxObject* cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 1);	// column Index = 1 [word]
					QVariant value = cell->dynamicCall( "Value()" );
					if (value.toString().trimmed().isEmpty())
					{
						break;
					}
					g_word[rowIndex - 1].m_word = value.toString().trimmed();

					cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 2);		// column Index = 2 [Trans]
					value = cell->dynamicCall( "Value()" );
					g_word[rowIndex - 1].m_trans = value.toString().trimmed();

					cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 3);		// column Index = 3 [Pronounce]
					value = cell->dynamicCall( "Value()" );
					g_word[rowIndex - 1].m_phonetic = value.toString().trimmed();

					g_word[rowIndex - 1].m_Date = QDate::currentDate();
					cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 4);		// column Index = 4 [date]
					value = cell->dynamicCall( "Value()" );
					if (value.canConvert(Qt::ISODate) == true)
					{
						g_word[rowIndex - 1].m_Date = value.toDate();
					}

					cell = sheet->querySubObject( "Cells( int, int )", rowIndex, 5);		// column Index = 5 [Famility Level]
					value = cell->dynamicCall( "Value()" );
					bool flag;
					g_word[rowIndex - 1].m_progress = value.toInt(&flag);
					if (!flag)
					{
						g_word[rowIndex - 1].m_progress = 1;
					}


					qDebug() << g_word[rowIndex - 1].m_Date.toString("yyyy-MM-dd");
					qDebug() << QString("%1:%2 = %3 %4 %5 %6").arg(rowIndex).arg(currentColumnIndex).arg(g_word[rowIndex - 1].m_word).arg(g_word[rowIndex - 1].m_progress).arg(g_word[rowIndex - 1].m_Date.toString("yyyy-MM-dd")).arg(g_word[rowIndex - 1].m_trans);
				}
				g_wordNum = rowIndex - 1;
			}
	
			workbook->dynamicCall("Close()");
			excel->dynamicCall("Quit()");
			if (g_wordNum == 0)
			{
				ret = false;
			}
			else
			{
				ret = true;
			}
		}
	}

	/* when we want to add new word, we should set the data to 2010-01-01 */
	if ((ret == true) && (g_wordNum != 0))
	{
		/* Now we can read the word */
		g_pRegSetting->setLastImportFromReg(fileName);

		if (m_database.addtoDB(m_pFileName->text(), g_word, g_wordNum, m_pImportWidget->getReplaceFlag()))
		{
#if 0
			if (m_pImportWidget->getReplaceFlag() == false)
			{
				ExportToXML(fileName, g_word, g_wordNum);
			}
#endif
			if (g_roteManager.getTotalWordCount() != 0)
			{
				if (m_pImportWidget->getReplaceFlag())
				{
					strLog = QString(tr("The words have been imported into database: %1, Press OK to check the forgetten words.")).arg(m_pFileName->text());
				}
				else
				{
					strLog = QString(tr("The words have been imported into database: %1, Press OK to check the new words.")).arg(m_pFileName->text());
				}

				QMessageBox::warning(this, tr("Import Success"), strLog, "OK");
				BrowseDialog browseDialog;
				browseDialog.exec();
			}
		}
		else 
		{
			strLog = QString("The words failed to be imported to database: ") + m_pFileName->text();
			QMessageBox::warning(this, "Import failed", strLog, "OK");
		}
	}

	onFileNameChanged(m_pFileName->text());
}

bool Widget::readNextStartElement(QXmlStreamReader *pReader)
{
	pReader->readNext();
	while (!pReader->atEnd()) 
	{
		if (pReader->isStartElement())
		{
			return (true);
		}
		else if (pReader->isEndElement())
		{
			return (false);
		}
		else if (pReader->isStartDocument() || pReader->isEndDocument())
		{
			pReader->readNext();			
		}
		else if (pReader->isCharacters())
		{
			if (pReader->text().toString().simplified().length() == 0)
			{
				pReader->readNext();			
			}
			else
			{
				return (false);
			}
		}
		else 
		{
			return (false);
		}
	}

	return (false);
}

bool Widget::readNextCharacter(QXmlStreamReader *pReader)
{
	pReader->readNext();
	if (!pReader->atEnd()) 
	{
		if (pReader->isCharacters())
		{
			return (true);
		}
	}
	return (false);
}

bool Widget::readNextEndElement(QXmlStreamReader *pReader)
{
	pReader->readNext();
	while (!pReader->atEnd()) 
	{
		if (pReader->isStartElement())
		{
			return (false);
		}
		else if (pReader->isEndElement())
		{
			return (true);
		}
		else if (pReader->isStartDocument() || pReader->isEndDocument())
		{
			pReader->readNext();			
		}
		else if (pReader->isCharacters())
		{
			if (pReader->text().toString().simplified().length() == 0)
			{
				pReader->readNext();			
			}
			else
			{
				return (false);
			}
		}
		else 
		{
			return (false);
		}
	}

	return (false);
}

bool Widget::readWordElement(QXmlStreamReader *pReader)
{
	QStringList elementList;
	QString value[6];
	elementList << "word" << "trans" << "phonetic" << "tags" << "date" << "progress";
	QString element;
	int i, j;

	for (i = 0; i < elementList.count(); i++)
	{
		if (!readNextStartElement(pReader))
		{
			return (false);
		}

		/* Sometimes the element may missed, such as phoentic, tags or progress  */
		for (j = i; j < elementList.count(); j++)
		{
			element = elementList.at(j);
			if (pReader->name() != element)
			{
				continue;
			}
			else
			{
				break;
			}
		}

		if (j >= elementList.count())
		{
			return (false);
		}
		i = j;

		//word
		if (!readNextCharacter(pReader))
		{
			if ((pReader->isEndElement() && pReader->name() == element))
			{
				value[i] = "";	
				qDebug() << element << "= " << value[i];
				continue;
			}
			else
			{
				return (false);
			}
		}
		value[i] = pReader->text().toString();
		qDebug() << element << "= " << value[i];

		//</word>
		if (!readNextEndElement(pReader) || (pReader->name() != element))
		{
			return (false);
		}
	}

	/* word & trans mustn't be null */
	if (value[0].length() == 0)
	{
		return (false);
	}
	if (value[5].length() == 0)
	{
		value[5] = QString("1");	//default progress is 1
	}
	g_word[g_wordNum].m_word = value[0];
	g_word[g_wordNum].m_trans = value[1];
	g_word[g_wordNum].m_phonetic = value[2];
	g_word[g_wordNum].m_tags = value[3];
	g_word[g_wordNum].m_progress = value[5].toInt();
	if (g_word[g_wordNum].m_progress < 0)
	{
		g_word[g_wordNum].m_progress = 0;
	}	
	if	(g_word[g_wordNum].m_progress >= 100)
	{
		g_word[g_wordNum].m_progress = 99;
	}

	if (value[4].length() == 0)
	{
		g_word[g_wordNum].m_Date = QDate::currentDate();
	}
	else
	{
		g_word[g_wordNum].m_Date = QDate::fromString(value[4], "yyyy-MM-dd");
		qDebug() << value[4] << "is" << g_word[g_wordNum].m_Date.toString();
		if (g_word[g_wordNum].m_Date.year() <= 0)
		{
			g_word[g_wordNum].m_Date = QDate::currentDate();		
		}
	}
	g_wordNum++;

	return (true);
}

bool Widget::getExportSetting(int &totalCount, bool &l1Flag, bool&l2Flag, int &l0Count, int &l1Count, int &l2Count, int &highestLevel)
{
	QString dbName = getCurrentDBName();

	bool ret = m_database.getRoteSetting(dbName, totalCount, l1Flag, l2Flag, l0Count, l1Count, l2Count, highestLevel);
	return (ret);
}

bool Widget::setExportSetting(int totalCount, bool l1Flag, bool l2Flag, int l0Count, int l1Count, int l2Count, int highestLevel)
{
	QString dbName = getCurrentDBName();

	bool ret = m_database.setRoteSetting(dbName, totalCount, l1Flag, l2Flag, l0Count, l1Count, l2Count, highestLevel);
	return (ret);
}

void Widget::onPreview()
{
	QString dbName = getCurrentDBName();

	/* Copy all the emphasis words to g_roteManager */
	bool ret = m_database.getEmphasisRecord(dbName);
	if (ret)
	{
		BrowseDialog browseDialog;
		browseDialog.exec();
	}
}

void Widget::onUnmark()
{
	QString fileName = getCurrentDBName();

	/* Copy all the emphasis words to g_roteManager */
	m_database.deleteAllBrowseWord(fileName);

	int nTotalCount = m_database.getBrowseWordCount(fileName);
	m_pEmphasisCountEdit->setText(QString::number(nTotalCount));
	m_pPreviewButton->setEnabled(nTotalCount != 0);
	m_pUnmarkButton->setEnabled(nTotalCount != 0);
}
