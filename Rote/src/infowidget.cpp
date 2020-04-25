#include "infowidget.h"
#include <qDebug>
#include "mainWindow.h"
#include <QMessageBox>
#include <QPainter>
#include "YoudaoSetting.h"
#include "bookInfo.h"
#include "YoudaoDatabase.h"

InfoWidget::InfoWidget(QWidget *parent)
    : QWidget(parent)
{
	m_pCurrentBook = new QLabel(tr("Current Book"), this);

    m_pBookName = new QLabel(this);
    m_pBookName->setObjectName(QStringLiteral("BigLabel"));
	QString strBook = g_pRegSetting->getLastExportFromReg();
	m_pBookName->setText(strBook);

	m_pBriefEdit = new TextView(this);
	m_pBriefEdit->setObjectName(QStringLiteral("m_pBriefEdit"));
	m_pBriefEdit->setEnabled(false);
	m_pBriefEdit->setReadOnly(false);

//	m_pBookName->setEnabled(false);
	m_pLevel0 = new QLabel(tr("0:"), this);
	m_pLevel1 = new QLabel(tr("1:"), this);
	m_pLevel2 = new QLabel(tr("2:"), this);
	m_pLevel3 = new QLabel(tr("3:"), this);
	m_pLevel4 = new QLabel(tr("4:"), this);
	m_pLevel5 = new QLabel(tr("5:"), this);
	m_pLevel6 = new QLabel(tr("6:"), this);
	m_pAbove7 = new QLabel(tr("7+:"), this);
	m_pTotal = new QLabel(tr("Total:"), this);

#if 0
	m_pLevel0Count = new QLabel("10", this);
	m_pLevel1Count = new QLabel("21", this);
	m_pLevel2Count = new QLabel("432", this);
	m_pLevel3Count = new QLabel("312", this);
	m_pLevel4Count = new QLabel("47", this);
	m_pLevel5Count = new QLabel("556", this);
	m_pLevel6Count = new QLabel("634", this);
	m_pAboveCount =  new QLabel("209", this);
//	m_pTotalCount =  new QLabel("1583", this);
#endif

	m_pNextButton = new QPushButton(tr("Another Book"), this);
	connect(m_pNextButton, SIGNAL(clicked()), this, SLOT(onSelectBook()));

	 
	m_pNoteButton = new QPushButton(tr("Note"), this);
	connect(m_pNoteButton, SIGNAL(clicked()), this, SLOT(onClickNote()));

	m_pRoteButton = new QPushButton(tr("Rote"), this);
	connect(m_pRoteButton, SIGNAL(clicked()), this, SLOT(onClickRote()));

	m_pBrowseButton = new QPushButton(tr("Review"), this);
	connect(m_pBrowseButton, SIGNAL(clicked()), this, SLOT(onClickReview()));

	m_background = QPixmap(":Background.png");

	m_pListWidget = new QListWidget(this);
	m_pListWidget->setObjectName(QStringLiteral("m_pListWidget"));
	connect(m_pListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(bookSelected(QListWidgetItem *)));
	m_selectBookFlag = false;
	m_pListWidget->hide();

	m_bookIndex = g_pRegSetting->getSelectionFromReg();
	refresh();
}

InfoWidget::~InfoWidget()
{

}

void InfoWidget::resizeEvent(QResizeEvent *)
{
	int w = width();
	int h = height();

	int left = w / 15;
	int right = w * 14 / 15;
	int unitHeight = h / 15;
	if (unitHeight < 40)
	{
		unitHeight = 40;
	}
	int unitWidth = w / 5;
	if (unitWidth < 150)
	{
		unitWidth = 150;
	}

	qDebug() << "w = " << w << ", h = " << h;

//	m_pCurrentBook->setGeometry(left, h * 0.22 - unitHeight - 10 , w * 0.4 , unitHeight);
//	m_pNextButton->setGeometry(right - unitWidth, h * 0.22 - unitHeight - 10, unitWidth, unitHeight);
//	m_pBookName->setGeometry(left, h * 0.2 - 5, right - left, unitHeight);

	m_pCurrentBook->setGeometry(left, h * 0.02, w * 0.4 , unitHeight);
	m_pNextButton->setGeometry(right - w * 0.4, h * 0.02, w * 0.4, unitHeight);

	m_pBookName->setGeometry(left, h * 0.115, right - left, unitHeight);

	m_pBriefEdit->setGeometry(left, h * 0.21 - 5, right - left, 0.21 * h);
	m_pNoteButton->setGeometry(right - w * 0.2, h * 0.42, w * 0.2, unitHeight - 10);

	m_pTotal->setGeometry(left, h * 0.42, right - left, unitHeight);

#if 1
	QLabel *pLabel[] = {m_pLevel0, m_pLevel1, m_pLevel2, m_pLevel3, m_pLevel4, m_pLevel5, m_pLevel6, m_pAbove7};


	for (int i = 0; i < 4; i++)
	{
		pLabel[2 * i]->setGeometry(left, h * 0.09 * (i + 5.5), w / 3 - left, unitHeight);
		pLabel[2 * i + 1]->setGeometry(left + w / 2, h * 0.09 * (i + 5.5), w / 3 - left, unitHeight);
	}
#endif

	int leftMargin = w / 16;
	m_pRoteButton->setGeometry(leftMargin , h * 0.87, w / 2 - 2 * leftMargin, unitHeight);
	m_pBrowseButton->setGeometry(w / 2 + leftMargin , h * 0.87, w / 2 - 2 * leftMargin, unitHeight);
	m_pListWidget->setGeometry(left, h * 0.115, right - left, h * 0.55);
}


void InfoWidget::paintEvent(QPaintEvent * /*event */)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_background.scaled(size()));
	painter.fillRect(0, 0, width(), height(), QBrush(QColor(255, 255, 255, 180)));
}

void InfoWidget::onClickRote()
{
#if 0
	SettingDlg settingDlg(this);
	settingDlg.setModal(true);
	settingDlg.exec();
#else
	QString strBook = m_pBookName->text();
	qDebug() << strBook;
	g_pRegSetting->setLastExportFromReg(strBook);

	g_pMainWindow->onClickRote();
#endif
}

void InfoWidget::onClickReview()
{
	g_pMainWindow->onClickReview();
}

void InfoWidget::refreshBook()
{
	g_pBookInfo->refreshBookInfo();
	refresh();
}

int InfoWidget::getCurrentBookIndex()
{
	if (m_bookIndex >= g_pBookInfo->getBookCount())
	{
		m_bookIndex = g_pBookInfo->getBookCount() - 1;
	}

    if (m_bookIndex < 0)
    {
        m_bookIndex = 0;
    }
    
	return (m_bookIndex);
}

void InfoWidget::refresh()
{
	if (g_pBookInfo->getBookCount() == 0)
	{
		m_bookIndex = 0;
		m_pCurrentBook->setText(QString("Book %1").arg(g_pBookInfo->getBookCount()));
		m_pLevel0->setText("0:");
		m_pLevel1->setText("1:");
		m_pLevel2->setText("2:");
		m_pLevel3->setText("3:");
		m_pLevel4->setText("4:");
		m_pLevel5->setText("5:");
		m_pLevel6->setText("6:");
		m_pAbove7->setText("7+:");
		m_pTotal->setText(tr("Total:"));
		m_pBookName->setText("");
		m_pRoteButton->setEnabled(false);
		m_pBrowseButton->setEnabled(false);
		m_selectBook = "";
		qDebug() << "No book is selected";
		return;
	}

    if (m_bookIndex < 0)
    {
        m_bookIndex = 0;
    }
    
	if (m_bookIndex >= g_pBookInfo->getBookCount())
	{
		m_bookIndex = g_pBookInfo->getBookCount() - 1;
	}

	BOOK_INFO *pBookINFO = g_pBookInfo->getBookInfo(m_bookIndex);
	m_pCurrentBook->setText(QString("Book %1 : %2").arg(g_pBookInfo->getBookCount()).arg(m_bookIndex + 1));
	m_pLevel0->setText(QString("0: %1").arg(pBookINFO->m_level0));
	m_pLevel1->setText(QString("1: %1").arg(pBookINFO->m_level1));
	m_pLevel2->setText(QString("2: %1").arg(pBookINFO->m_level2));
	m_pLevel3->setText(QString("3: %1").arg(pBookINFO->m_level3));
	m_pLevel4->setText(QString("4: %1").arg(pBookINFO->m_level4));
	m_pLevel5->setText(QString("5: %1").arg(pBookINFO->m_level5));
	m_pLevel6->setText(QString("6: %1").arg(pBookINFO->m_level6));
	m_pAbove7->setText(QString("7+: %1").arg(pBookINFO->m_level7));
//	int above = pBookINFO->m_wordCount - pBookINFO->m_level1 - pBookINFO->m_level2 - pBookINFO->m_level3 - pBookINFO->m_level4 - pBookINFO->m_level5 - pBookINFO->m_level6;
	m_pTotal->setText(QString(tr("Total: %1 - %2")).arg(pBookINFO->m_wordCount).arg(pBookINFO->m_totalScore));
	m_pBookName->setText(pBookINFO->m_bookFileName);
	m_pRoteButton->setEnabled(true);
	m_pBrowseButton->setEnabled(true);
	m_selectBook = pBookINFO->m_bookFileName;

	QString brief;
	g_database.getBrief(pBookINFO->m_bookFileName, brief);
	m_pBriefEdit->setText(brief);

	qDebug() << "Book: " << m_selectBook << "is selected";
}

void InfoWidget::onNextBook()
{
	m_bookIndex++;
	if (m_bookIndex >= g_pBookInfo->getBookCount())
	{
		m_bookIndex = 0;
	}
	g_pRegSetting->setSelectionFromReg(m_bookIndex);

	refresh();
	m_pBriefEdit->setEnabled(false);
	m_pNoteButton->setText(tr("Note"));
}

void InfoWidget::onSelectBook()
{
	if (m_selectBookFlag == false)
	{
		/* Now we will select a book */
		m_pListWidget->show();
		m_pListWidget->clear();
		g_pBookInfo->refreshBookInfo();

		if (g_pBookInfo->getBookCount() == 0)
		{
			return;
		}

		for (int i = 0; i < g_pBookInfo->getBookCount(); i++)
		{
			m_pListWidget->addItem(g_pBookInfo->getBookFileName(i));
		}

		if (m_bookIndex >= g_pBookInfo->getBookCount())
		{
			m_bookIndex = 0;
		}
		m_pListWidget->setCurrentRow(m_bookIndex);

		m_pNextButton->setText(tr("select"));
		m_pRoteButton->setEnabled(false);
		m_pBrowseButton->setEnabled(false);
		m_pNoteButton->setEnabled(false);
	}
	else
	{
		/* A book is already selected */
		m_bookIndex = m_pListWidget->currentRow();

		g_pRegSetting->setSelectionFromReg(m_bookIndex);

		refresh();
		m_pBriefEdit->setEnabled(false);
		m_pNoteButton->setText(tr("Note"));

		m_pRoteButton->setEnabled(true);
		m_pBrowseButton->setEnabled(true);
		m_pNoteButton->setEnabled(true);

		m_pListWidget->hide();
		m_pNextButton->setText(tr("Another Book"));
	}

	m_selectBookFlag = !m_selectBookFlag;
}

void InfoWidget::onClickNote()
{
	if (g_pBookInfo->getBookCount() == 0)
	{
		return;
	}

	if (m_bookIndex >= g_pBookInfo->getBookCount())
	{
		m_bookIndex = g_pBookInfo->getBookCount() - 1;
	}
	BOOK_INFO *pBookINFO = g_pBookInfo->getBookInfo(m_bookIndex);

	if (m_pBriefEdit->isEnabled())
	{
		QString fileName = pBookINFO->m_bookFileName;
		QString brief = m_pBriefEdit->toPlainText();
		g_database.setBrief(fileName, brief);
		m_pBriefEdit->setEnabled(false);
		m_pNoteButton->setText(tr("Note"));
	}
	else
	{
		m_pBriefEdit->setEnabled(true);
		m_pNoteButton->setText(tr("Save"));
	}
}

void InfoWidget::bookSelected(QListWidgetItem * /*item*/)
{
	qDebug() << "Item selected!";
}
