#include "settingwidget.h"
#include <qDebug>
#include "mainWindow.h"
#include <QMessageBox>
#include <QPainter>
#include "YoudaoSetting.h"
#include "bookInfo.h"
#include "YoudaoDatabase.h"
#include <QIntValidator>

SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent)
{
	m_pAllBookWord = nullptr;
	m_wordCount = 0;

	m_pLine = new QFrame(this);
//    m_pLine->setObjectName(QStringLiteral("m_pLine1"));
    m_pLine->setFrameShape(QFrame::HLine);

	m_pTotalEditLabel = new QLabel(tr("Total:"), this);
	m_pTotalEditLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	m_pLevel0EditLabel = new QLabel(tr("L0:"), this);
	m_pLevel1EditLabel = new QLabel(tr("L1:"), this);
	m_pLevel2EditLabel = new QLabel(tr("L2:"), this);
	m_pHighestLevelEditLabel = new QLabel(tr("Highest Level"), this);
	
	m_pTotalEdit = new NumberButton(this);
    m_pTotalEdit->setRange(0, 1000);
	connect(m_pTotalEdit, SIGNAL(valueChanged(int)), this, SLOT(onTotalCountChanged(int)));

	m_pLevel0Edit = new NumberButton(this);
    m_pLevel0Edit->setRange(0, 1000);
	connect(m_pLevel0Edit, SIGNAL(valueChanged(int)), this, SLOT(onL0CountChanged(int)));

	m_pLevel1Edit = new NumberButton(this);
    m_pLevel1Edit->setRange(0, 1000);
	connect(m_pLevel1Edit, SIGNAL(valueChanged(int)), this, SLOT(onL1CountChanged(int)));

	m_pLevel2Edit = new NumberButton(this);
    m_pLevel2Edit->setRange(0, 1000);
	connect(m_pLevel2Edit, SIGNAL(valueChanged(int)), this, SLOT(onL2CountChanged(int)));

	m_pHighestLevelEdit = new NumberButton(this);
    m_pHighestLevelEdit->setRange(0, 20);
	connect(m_pHighestLevelEdit, SIGNAL(valueChanged(int)), this, SLOT(onHighestLevelChanged(int)));

	m_pLevel1Button = new QPushButton(QString(""), (QWidget *)this);
	m_pLevel1Button->setObjectName(QStringLiteral("CheckButton"));
	m_pLevel1Button->setCheckable(true);
	m_pLevel1Button->setChecked(true);
	connect(m_pLevel1Button,  SIGNAL(clicked()), this, SLOT(onClickLevel1()));

	m_pLevel2Button = new QPushButton(QString(""), (QWidget *)this);
	m_pLevel2Button->setObjectName(QStringLiteral("CheckButton"));
	m_pLevel2Button->setCheckable(true);
	m_pLevel2Button->setChecked(true);
	connect(m_pLevel2Button,  SIGNAL(clicked()), this, SLOT(onClickLevel2()));

	m_pMethodButton = new QPushButton(this);
	connect(m_pMethodButton, SIGNAL(clicked()), this, SLOT(onClickMethodButton()));

//	m_pBookName->setEnabled(false);
	m_pLevel0 = new QLabel(tr("0:"), this);
	m_pLevel1 = new QLabel(tr("1:"), this);
	m_pLevel2 = new QLabel(tr("2:"), this);
	m_pLevel3 = new QLabel(tr("3:"), this);
	m_pLevel4 = new QLabel(tr("4:"), this);
	m_pLevel5 = new QLabel(tr("5:"), this);
	m_pLevel6 = new QLabel(tr("6:"), this);
	m_pAbove7 = new QLabel(tr("7+:"), this);
	m_pTotal = new QLabel(tr(""), this);

	m_pCancelButton = new QPushButton(tr("Cancel"), this);
	connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));
	m_pStartButton = new QPushButton(tr("Start"), this);
	connect(m_pStartButton, SIGNAL(clicked()), this, SLOT(onStart()));
	m_pBrowseButton = new QPushButton(tr("Browse"), this);
	connect(m_pBrowseButton, SIGNAL(clicked()), this, SLOT(onBrowse()));
	
	m_background = QPixmap(":Background.png");

	m_refreshFlag = false;
}

void SettingWidget::onL0CountChanged(int value)
{
	m_l0Count = value;
	if (m_refreshFlag == true)
	{
		refresh();
	}
}

void SettingWidget::onL1CountChanged(int value)
{
	m_l1Count = value;
	if (m_refreshFlag == true)
	{
		refresh();
	}
}

void SettingWidget::onL2CountChanged(int value)
{
	m_l2Count = value;
	if (m_refreshFlag == true)
	{
		refresh();
	}
}

void SettingWidget::onClickMethodButton()
{
	m_methodIndex++;
	if (m_methodIndex >= 3)
	{
		m_methodIndex = 0;
	}
	refreshMethod();
}

void SettingWidget::refreshMethod()
{
	/*	0: Chinese to English, 1: English to Chinese, 2: Dictation */
	switch (m_methodIndex)
	{
	case 0:
		m_pMethodButton->setText(tr("Explanation first"));
		break;
	case 1:
		m_pMethodButton->setText(tr("Word first"));
		break;
	default:
		m_pMethodButton->setText(tr("Dictation"));
		break;
	}
	g_pMainWindow->setMethod(m_methodIndex);
}

void SettingWidget::onHighestLevelChanged(int value)
{
	m_highestLevel = value;
	refresh();
}

void SettingWidget::onTotalCountChanged(int value)
{
	m_total = value;
	if (m_refreshFlag == true)
	{
		refresh();
	}
}

void SettingWidget::onClickLevel1()
{
	m_l1Flag = m_pLevel1Button->isChecked();
	if (m_refreshFlag == true)
	{
		refresh();
	}
}

void SettingWidget::onClickLevel2()
{
	m_l2Flag = m_pLevel2Button->isChecked();
	if (m_refreshFlag == true)
	{
		refresh();
	}
}

SettingWidget::~SettingWidget()
{
	if (m_pAllBookWord != nullptr)
	{
		delete []m_pAllBookWord;
	}
}

bool SettingWidget::loadDatabase(QString dbFileName)
{
	int nTotalCount, level0, level1, level2, level3, level4, level5, level6, level7, totalScore;

	if (g_database.getDBSummary(dbFileName, nTotalCount, level0, level1, level2, level3, level4, level5, level6, level7, totalScore) != true)
	{
		return (false);
	}

	m_dbFileName = dbFileName;
	if (m_pAllBookWord != nullptr)
	{
		delete []m_pAllBookWord;
	}
	m_pAllBookWord = new YoudaoWord[nTotalCount];
	m_wordCount = 0;
	if (m_pAllBookWord == nullptr)
	{
		return false;
	}

	m_level0Count = level0;
	m_level1Count = level1;
	m_level2Count = level2;

    if (!g_database.getPriority(dbFileName, g_param1, g_param2))
    {
        return (false);
    }
    if (!g_database.LoadFromDBByPriority(dbFileName, m_pAllBookWord, m_wordCount, nTotalCount))
	{
		return (false);
	}

	if (!g_database.getRoteSetting(dbFileName, m_total, m_l1Flag, m_l2Flag, m_l0Count, m_l1Count, m_l2Count, m_highestLevel, m_methodIndex))
	{
		m_total = 60;
		m_l1Flag = true;
		m_l2Flag = false;
		m_l0Count = 5;
		m_l1Count = 15;
		m_l2Count = 10;
		m_highestLevel = 7;
		m_methodIndex = 2;
	}

	m_refreshFlag = false;
	m_pTotalEdit->setValue(m_total);
	m_pLevel0Edit->setValue(m_l0Count);
	m_pLevel1Edit->setValue(m_l1Count);
	m_pLevel2Edit->setValue(m_l2Count);
	m_pHighestLevelEdit->setValue(m_highestLevel);
	m_pLevel1Button->setChecked(m_l1Flag);
	m_pLevel2Button->setChecked(m_l2Flag);
	m_refreshFlag = true;

	refresh();
	refreshMethod();
	return (true);
}

void SettingWidget::resizeEvent(QResizeEvent *)
{
	int w = width();
	int h = height();

	int left = w / 15 - 5;
//	int right = w * 14 / 15;
	int unitHeight = h / 15;
	if (unitHeight < 35)
	{
		unitHeight = 35;
	}
	int unitWidth = w / 4 - left;
	if (unitWidth < 60)
	{
		unitWidth = 60;
	}

	m_pTotalEditLabel->setGeometry(left, h * 0.03, w / 4 - left, unitHeight);
    m_pTotalEdit->setGeometry( w / 4, h * 0.03, w / 4 - left, unitHeight);
	
	m_pLevel0EditLabel->setGeometry(w / 2 + left, h * 0.03, w / 4 - left, unitHeight);
	m_pLevel0Edit->setGeometry(w * 3 / 4, h * 0.03, w / 4 - left, unitHeight);

	m_pLevel1Button->setGeometry(w / 2 - left - unitWidth, h * 0.13, unitWidth, unitHeight);
	m_pLevel1EditLabel->setGeometry(w / 2 + left, h * 0.13, w / 4 - left, unitHeight);
	m_pLevel1Edit->setGeometry(w * 3 / 4, h * 0.13, w / 4 - left, unitHeight);
	
	m_pLevel2Button->setGeometry(w / 2 - left - unitWidth, h * 0.23, unitWidth, unitHeight);
	m_pLevel2EditLabel->setGeometry(w / 2 + left, h * 0.23, w / 4 - left, unitHeight);
	m_pLevel2Edit->setGeometry(w * 3 / 4, h * 0.23, w / 4 - left, unitHeight);

	m_pMethodButton->setGeometry(left, h * 0.33, w / 2 - 2 * left, unitHeight);
	m_pHighestLevelEditLabel->setGeometry(w / 2 + left, h * 0.33, w / 4 - left, unitHeight);
	m_pHighestLevelEdit->setGeometry(w * 3 / 4, h * 0.33, w / 4 - left, unitHeight);

	int leftMargin = w / 16;
	m_pTotal->setGeometry(left, h * 0.47, w / 2 - left, unitHeight);
	m_pBrowseButton->setGeometry(w / 2 + leftMargin , h * 0.47, w / 2 - 2 * leftMargin, unitHeight);

	m_pLine->setGeometry(QRect(left , (h * 0.33 + unitHeight + h * 0.47 ) / 2, w - 2 * left, 1));

	QLabel *pLabel[8] = {m_pLevel0, m_pLevel1, m_pLevel2, m_pLevel3, m_pLevel4, m_pLevel5, m_pLevel6, m_pAbove7 };
	for (int i = 0; i < 4; i++)
	{
		pLabel[2 * i]->setGeometry(left, h * (0.55 + 0.08 * i), w / 2 - left, unitHeight);
		pLabel[2 * i + 1]->setGeometry(left + w / 2, h * (0.55 + 0.08 * i), w / 2 - left, unitHeight);
	}
	m_pStartButton->setGeometry(leftMargin , h * 0.88, w / 2 - 2 * leftMargin, unitHeight);
	m_pCancelButton->setGeometry(w / 2 + leftMargin , h * 0.88, w / 2 - 2 * leftMargin, unitHeight);
}

void SettingWidget::paintEvent(QPaintEvent * /*event */)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_background.scaled(size()));
    painter.fillRect(0, 0, width(), height(), QBrush(QColor(255, 255, 255, 180)));
}

void SettingWidget::onCancel()
{
//	saveRoteSetting();
	g_pMainWindow->onSettingCancel();
}

void SettingWidget::onBrowse()
{
	g_pMainWindow->onClickBrowse();
}

void SettingWidget::saveRoteSetting()
{
	g_database.setRoteSetting(m_dbFileName, m_total, m_l1Flag, m_l2Flag, m_l0Count, m_l1Count, m_l2Count, m_highestLevel, m_methodIndex);
}

void SettingWidget::onStart()
{
	saveRoteSetting();
	g_pMainWindow->onSettingStart();
}

void SettingWidget::refresh()
{
	int i;
	YoudaoWord pNewWordBook[1000];  // We can select total 1000 word at most.

	if (m_total > m_wordCount)
	{
		m_total = m_wordCount;
	}

	if (m_l0Count > m_level0Count)
	{
		m_l0Count = m_level0Count;
	}
	if (m_l1Count > m_level1Count)
	{
		m_l1Count = m_level1Count;
	}
	if (m_l2Count > m_level2Count)
	{
		m_l2Count = m_level2Count;
	}
	if (m_highestLevel > 20)
	{
		m_highestLevel = 20;
	}

	g_roteManager.clear();

	int l0Count = m_l0Count;
	int l1Count = m_l1Count;
	if (m_l1Flag == false)
	{
		l1Count = 0;
	}

	int l2Count = m_l2Count;
	if (m_l2Flag == false)
	{
		l2Count = 0;
	}

	int otherCount;

	if (l0Count > m_total)
	{
		l0Count = m_total;
		l1Count = l2Count = otherCount = 0;
	}
	else if (l0Count + l1Count > m_total)
	{
		l1Count = m_total - l0Count;
		l2Count = 0;
		otherCount = 0;
	}
	else if (l0Count + l1Count + l2Count > m_total)
	{
		l2Count = m_total - l0Count - l1Count;
		otherCount = 0;
	}
	else
	{
		otherCount = m_total - l0Count - l1Count - l2Count;
	}

	int l0Index = 0, l1Index = 0, l2Index = 0, otherIndex = 0;
	int currentIndex = 0;
	for (i = 0; i < m_wordCount; i++)
	{
		if (m_pAllBookWord[i].m_progress > m_highestLevel)
		{
			continue;
		}

		switch (m_pAllBookWord[i].m_progress)
		{

			case 0:
				if (l0Index < l0Count)
				{
					pNewWordBook[currentIndex] = m_pAllBookWord[i];
					l0Index++;
					currentIndex++;
				}
				break;
			case 1:
				{
					/* Must import Level 1 words */
					if (m_l1Flag == true)
					{
						if (l1Index < l1Count)
						{
							pNewWordBook[currentIndex] = m_pAllBookWord[i];
							l1Index++;
							currentIndex++;						
						}
					}
					else
					{
						if (otherIndex < otherCount)
						{
							pNewWordBook[currentIndex] = m_pAllBookWord[i];
							otherIndex++;
							currentIndex++;											
						}
					}
				}
				break;
			case 2:
				{
					/* Must import Level 2 words */
					if (m_l2Flag == true)
					{
						if (l2Index < l2Count)
						{
							pNewWordBook[currentIndex] = m_pAllBookWord[i];
							l2Index++;
							currentIndex++;									
						}
					}
					else
					{
						if (otherIndex < otherCount)
						{
							pNewWordBook[currentIndex] = m_pAllBookWord[i];
							otherIndex++;
							currentIndex++;											
						}				
					}
				}
				break;
			default:
				if (otherIndex < otherCount)
				{
					pNewWordBook[currentIndex] = m_pAllBookWord[i];
					otherIndex++;
					currentIndex++;											
				}
		}
	}

	int l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
	for (i = 0; i < currentIndex; i++)
	{
		switch (pNewWordBook[i].m_progress)
		{
		case 0:
			l0++;
			pNewWordBook[i].m_progress = 1;
			break;
		case 1:
			l1++;
			break;
		case 2:
			l2++;
			break;
		case 3:
			l3++;
			break;
		case 4:
			l4++;
			break;
		case 5:
			l5++;
			break;
		case 6:
			l6++;
			break;
		default:
			l7++;
			break;
		}
		g_roteManager.addWord(pNewWordBook + i);
	}

	m_pLevel0->setText(QString("L0: %1").arg(l0));
	m_pLevel1->setText(QString("L1: %1").arg(l1));
	m_pLevel2->setText(QString("L2: %1").arg(l2));
	m_pLevel3->setText(QString("L3: %1").arg(l3));
	m_pLevel4->setText(QString("L4: %1").arg(l4));
	m_pLevel5->setText(QString("L5: %1").arg(l5));
	m_pLevel6->setText(QString("L6: %1").arg(l6));
	m_pAbove7->setText(QString("L7: %1").arg(l7));

	m_pTotal->setText(QString(tr("Total: %1").arg(currentIndex)));
}

#if 0
void SettingWidget::onNextBook()
{
	m_bookIndex++;
	if (m_bookIndex >= g_pBookInfo->getBookCount())
	{
		m_bookIndex = 0;
	}

	refresh();
}
#endif
