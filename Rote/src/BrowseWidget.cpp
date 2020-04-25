#include "settingwidget.h"
#include <qDebug>
#include "mainWindow.h"
#include <QMessageBox>
#include <QPainter>
#include "BrowseWidget.h"
#include "bookInfo.h"
#include "YoudaoDatabase.h"
#include <QIntValidator>
#include <QScroller>
#include <QScrollerProperties>
#include <QEasingCurve>
#include <QTextBlock>
#include <QApplication>
#include <QClipboard>

BrowseWidget::BrowseWidget(QWidget *parent)
    : QWidget(parent)
{
    m_pBrowser = new TextView(this);
//    m_pBrowser->setEnabled(false);
    m_pBrowser->setReadOnly(true);
//    m_pBrowser->setWordWrap(true);
	m_pBrowser->setObjectName(QStringLiteral("m_pBrowser"));
	connect(m_pBrowser, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

//	m_pBrowser->setObjectName(QStringLiteral("m_pBrowser"));

	m_pTotalCount = new QLabel(this);
    m_pTotalCount->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	m_pExplainButton = new QPushButton(QString(tr("Explain")), this);
	m_pExplainButton->setObjectName(QStringLiteral("CheckButton"));
	m_pExplainButton->setCheckable(true);
	m_pExplainButton->setChecked(true);
	m_pExplainButton->setFlat(true);
	m_pExplainButton->setAutoFillBackground(true);    
	connect(m_pExplainButton,  SIGNAL(clicked()), this, SLOT(onClickExplain()));

	m_pExitButton = new QPushButton(QString(tr("Exit")), this);
	connect(m_pExitButton, SIGNAL(clicked()), this, SLOT(onBrowseExit()));

	m_pDeleteButton = new QPushButton(QString(tr("Delete")), this);
    connect(m_pDeleteButton, SIGNAL(clicked()), this, SLOT(onDeleteWord()));
	m_pDeleteButton->setEnabled(false);

	m_background = QPixmap(":Background.png");
	m_pBrowserWord = nullptr;
	m_bDeleteFlag = false;
}

void BrowseWidget::onClickExplain()
{
	refresh();
}

void BrowseWidget::onSelectionChanged()
{
	if (m_pExplainButton->isChecked() == true)
	{
		m_pDeleteButton->setEnabled(false);
		return;
	}

	QTextCursor cursor = m_pBrowser->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    if(!cursor.hasSelection())
	{
		m_pDeleteButton->setEnabled(false);
        return; // No selection available
	}

	cursor.setPosition(start);
	int firstLine = cursor.blockNumber();
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	int lastLine = cursor.blockNumber();
//	qWarning() << "start: " << firstLine << " end: " << lastLine << endl;

	if ((firstLine & 1) != 0)
	{
		firstLine++;
	}

	if ((lastLine & 1) != 0)
	{
		lastLine--;
	}

	if (firstLine != lastLine)
	{
		m_pDeleteButton->setEnabled(false);
		return;
	}

	m_pDeleteButton->setEnabled(true);
}

void BrowseWidget::onDeleteWord()
{
	if (m_pExplainButton->isChecked() == true)
	{
		return;
	}

	QTextCursor cursor = m_pBrowser->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    if(!cursor.hasSelection())
        return; // No selection available

	cursor.setPosition(start);
	int firstLine = cursor.blockNumber();
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	int lastLine = cursor.blockNumber();
//	qWarning() << "start: " << firstLine << " end: " << lastLine << endl;

	if ((firstLine & 1) != 0)
	{
		firstLine++;
	}

	if ((lastLine & 1) != 0)
	{
		lastLine--;
	}

	if (firstLine != lastLine)
	{
		return;
	}

	int lineIndex = firstLine / 2;

	g_pMainWindow->deleteWord(m_pBrowserWord[lineIndex].m_strWord);
	
	removeWord(lineIndex);
	for (int line = lastLine + 1; line >= firstLine; line--)
	{
        QTextBlock block = m_pBrowser->document()->findBlockByNumber(line);
		if (block.isValid())
		{
			QTextCursor blockCursor(block);
			cursor.select(QTextCursor::BlockUnderCursor);
			cursor.deletePreviousChar();
			cursor.removeSelectedText();
		}
	}
	
}

void BrowseWidget::removeWord(int wordIndex)
{
	for (int i = wordIndex; i < m_wordCount - 1; i++)
	{
		m_pBrowserWord[i].m_strWord = m_pBrowserWord[i + 1].m_strWord;
		m_pBrowserWord[i].m_strExplain = m_pBrowserWord[i + 1].m_strExplain;
		m_pBrowserWord[i].m_phonetic = m_pBrowserWord[i + 1].m_phonetic;
	}
	m_wordCount--;

	m_bDeleteFlag = true;
//	refresh();
}

void BrowseWidget::onBrowseExit()
{
	g_pMainWindow->onBrowseExit();
}

BrowseWidget::~BrowseWidget()
{
	if (m_pBrowserWord != nullptr)
	{
		delete []m_pBrowserWord;
	}
}

void BrowseWidget::loadDatabase(QString dbFileName)
{
	clearDeleteFlag();
	m_wordCount = g_database.getBrowseWordCount(dbFileName);
	if (m_wordCount != 0)
	{
		if (m_pBrowserWord != nullptr)
		{
			delete []m_pBrowserWord;
		}
		m_pBrowserWord = new BrowseData[m_wordCount];

		m_wordCount = g_database.loadBrowseWord(dbFileName, m_pBrowserWord, m_wordCount);
	}

	refresh();
}

void BrowseWidget::loadForgottenWord()
{
	clearDeleteFlag();
	int totalCount = g_roteManager.getTotalWordCount();
	int i;
	int totalUnfinishedCount = 0; /* the word is been roted, but never passed */;
	int totalForgettenCount = 0; /* the word is failed at the first time, but is passed at last */

	for (i = 0; i < totalCount; i++)
	{
		int roteCount = g_roteManager.getRoteCount(i);
		if (roteCount == 0)
		{
			continue;
		}
		
		if (g_roteManager.isWordFinished(i) == true)
		{
			if (roteCount != -1)
			{
				totalForgettenCount++;
			}
		}
		else
		{
			totalUnfinishedCount++;
		}
	}

	if (totalUnfinishedCount != 0)
	{
		m_wordCount = totalUnfinishedCount;
	}
	else if (totalForgettenCount != 0)
	{
		m_wordCount = totalForgettenCount;
	}
	else
	{
		m_wordCount = 0;
	}

	if (m_pBrowserWord != nullptr)
	{
		delete []m_pBrowserWord;
		m_pBrowserWord = nullptr;
	}

	if (m_wordCount == 0)
	{
		return;
	}
	m_pBrowserWord = new BrowseData[m_wordCount];

	int browseIndex = 0;
	for (i = 0; i < g_roteManager.getTotalWordCount(); i++)
	{
		int roteCount = g_roteManager.getRoteCount(i);
		if (roteCount == 0)
		{
			continue;
		}

		if (totalUnfinishedCount != 0)
		{
			/* Unfinished words */
			if (g_roteManager.isWordFinished(i) == false)
			{
				YoudaoWord *pWord = g_roteManager.getWord(i);
				m_pBrowserWord[browseIndex].m_strWord = pWord->m_word;
				m_pBrowserWord[browseIndex].m_strExplain = pWord->m_trans.trimmed();
				m_pBrowserWord[browseIndex].m_phonetic = pWord->m_phonetic;
				browseIndex++;
			}
		}
		else
		{
			/* Forgotton words */
			if (roteCount != -1)
			{
				YoudaoWord *pWord = g_roteManager.getWord(i);
				m_pBrowserWord[browseIndex].m_strWord = pWord->m_word;
				m_pBrowserWord[browseIndex].m_strExplain = pWord->m_trans.trimmed();
				m_pBrowserWord[browseIndex].m_phonetic = pWord->m_phonetic;
				browseIndex++;			
			}
		}
	}

	refresh();
}

void BrowseWidget::loadRoteManager(RoteManager *pRoteManager)
{
	clearDeleteFlag();
	m_wordCount = pRoteManager->getTotalWordCount();
	if (m_wordCount != 0)
	{
		if (m_pBrowserWord != nullptr)
		{
			delete []m_pBrowserWord;
		}
		
		m_pBrowserWord = new BrowseData[m_wordCount];
	}
	
	for (int i = 0; i < m_wordCount; i++)
	{
		m_pBrowserWord[i].m_strWord = pRoteManager->getWord(i)->m_word;
//		qDebug() << m_pBrowserWord[i].m_strWord;
		m_pBrowserWord[i].m_strExplain = pRoteManager->getWord(i)->m_trans;
//		qDebug() << m_pBrowserWord[i].m_strExplain << endl;
		m_pBrowserWord[i].m_phonetic = pRoteManager->getWord(i)->m_phonetic;
//		qDebug() << m_pBrowserWord[i].m_strExplain << endl;

	}

	refresh();
}

void BrowseWidget::resizeEvent(QResizeEvent *)
{

	int w = width();
	int h = height();

	int top = h / 20 + 10;
	int left = w / 15;
	int unitHeight = h / 20 + 10;


	int leftMargin = w / 16;
	m_pDeleteButton->setGeometry(leftMargin, 0, w / 2 - 2 * leftMargin, top);
	m_pTotalCount->setGeometry(w - left - 200, 0, 200, top);
	m_pBrowser->setGeometry(left, top + 5, w - 2 * left, h * 0.89 - 1.4 * top);

	m_pExplainButton->setGeometry(leftMargin , h * 0.89, w / 2 - 2 * leftMargin, unitHeight);
	m_pExitButton->setGeometry(w / 2 + leftMargin , h * 0.89, w / 2 - 2 * leftMargin, unitHeight);
}

void BrowseWidget::paintEvent(QPaintEvent * /*event */)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_background.scaled(size()));
    painter.fillRect(0, 0, width(), height(), QBrush(QColor(255, 255, 255, 180)));
}

void BrowseWidget::refresh()
{
	QString msg;
	bool explainFlag = m_pExplainButton->isChecked();

	for (int i = 0; i < m_wordCount; i++)
	{
		msg += m_pBrowserWord[i].m_strWord;
		if (explainFlag == true)
		{
			msg += "    ";
			msg += m_pBrowserWord[i].m_phonetic;
		}
		msg += QString("\n");

		if (explainFlag == true)
		{
			msg += m_pBrowserWord[i].m_strExplain;
			msg += QString("\n");
		}
		msg += QString("\n");
	}
	m_pBrowser->setText(msg);

	QClipboard *pBoard = QApplication::clipboard();
	pBoard->setText(msg);

//    qDebug() << "Line = " << m_pBrowser->document()->lineCount();

	m_pTotalCount->setText(QString(tr("Total %1").arg(m_wordCount)));
}

