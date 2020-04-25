#include "rotewidget.h"
#include "mainwindow.h"
#include <QPainter>
#include <QMessageBox>
#include "YoudaoDatabase.h"
#include "bookInfo.h"

RoteWidget::RoteWidget(QWidget *parent)
    : QWidget(parent)
{
	m_pWordBrowser = new TextView(this);
//	m_pWordBrowser->setAlignment(Qt::AlignBottom);
	m_pWordBrowser->setObjectName(QStringLiteral("m_pWordBrowser"));
	
    m_pBigWordBrowser = new TextView(this);
	m_pBigWordBrowser->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_pBigWordBrowser->setObjectName(QStringLiteral("m_pBigWordBrowser"));
	m_pBigWordBrowser->hide();

	m_pLeftLabel = new QLabel(this);
	m_pLeftLabel->setAlignment(Qt::AlignRight);

    m_pExplainBrowser = new TextView(this);
	m_pExplainBrowser->setObjectName(QStringLiteral("m_pWordBrowser"));

	m_pForgetButton = new QPushButton(tr("Forget"), this);
	m_pRememberButton = new QPushButton(tr("Remember"), this);

	m_pExitButton = new QPushButton(tr("Exit"), this);
	m_pMarkButton = new QPushButton(tr("Mark"), this);

	m_pWordButton = new QPushButton(tr("Word"), this);
	m_pExplainButton = new QPushButton(tr("Explain"), this);
	connect(m_pWordButton, SIGNAL(clicked()), this, SLOT(onClickWordButton()));
	connect(m_pExplainButton, SIGNAL(clicked()), this, SLOT(onClickExplainButton()));

	m_background = QPixmap(":Background.png");
	m_method = 2;
	m_methodStep = 0;

	connect(m_pRememberButton, SIGNAL(clicked()), this, SLOT(onClickRemember()));
	connect(m_pForgetButton, SIGNAL(clicked()), this, SLOT(onClickForget()));

	connect(m_pExitButton, SIGNAL(clicked()), this, SLOT(onClickExit()));
	connect(m_pMarkButton, SIGNAL(clicked()), this, SLOT(onClickMark()));

}

void RoteWidget::startRote()
{
	m_currentWordIndex = 0;
	m_nForgottenWordCount = 0;
	refresh();
	g_pMainWindow->getCurrentBookInfo(m_oldBookInfo);
}

void RoteWidget::refresh()
{
	YoudaoWord word;
	QString strMessage;

	m_currentWordIndex = g_roteManager.getNextRoteWord(&word);
	if (m_currentWordIndex == -1)
	{
		BOOK_INFO newBookInfo;

		/* Get New book info */
		g_roteManager.saveRoteResult();
		g_pBookInfo->refreshBookInfo();
		g_pMainWindow->getCurrentBookInfo(newBookInfo);

		/* Print Summary*/
		QString summary;
		summary.sprintf("Total score: %d\nL0: %d\nL1: %d\nL2: %d\nL3: %d\nL4: %d\nL5: %d\nL6: %d\nL7+: %d"
			, newBookInfo.m_totalScore - m_oldBookInfo.m_totalScore
			, newBookInfo.m_level0 - m_oldBookInfo.m_level0
			, newBookInfo.m_level1 - m_oldBookInfo.m_level1
			, newBookInfo.m_level2 - m_oldBookInfo.m_level2
			, newBookInfo.m_level3 - m_oldBookInfo.m_level3
			, newBookInfo.m_level4 - m_oldBookInfo.m_level4
			, newBookInfo.m_level5 - m_oldBookInfo.m_level5
			, newBookInfo.m_level6 - m_oldBookInfo.m_level6
			, newBookInfo.m_level7 - m_oldBookInfo.m_level7 );

		if (getForgottonWordCount() == 0)
		{
			strMessage = QString(tr("The roting is finished. No words is forgetton!"));
		}
		else
		{
			strMessage = QString(tr("The roting is finished. There are %1 words forgotton. Click OK button to review the failed words.").arg(getForgottonWordCount()));
		}
		QMessageBox::information(this, tr("Roting Finished"), QString(tr("Software Version: %1.").arg(SOFT_VERSION)) + QString("\n") + strMessage + QString("\n") + summary, QMessageBox::Ok);
		g_pMainWindow->onClickExitRote();
		return;
	}
	else
	{
		int left = g_roteManager.getLeftWordCount();
		int failed = getForgottonWordCount();
		int count = g_roteManager.getTotalWordCount();
		QString strLeft;
		if (failed >= left)
		{
			strLeft.sprintf("%d/%d", left, count);
		}
		else
		{
			strLeft.sprintf("%d/%d/%d", failed, left, count);
		}
		m_pLeftLabel->setText(strLeft);
		m_pWordBrowser->setText(word.m_word + QString("\r\n\r\n%1").arg(word.m_phonetic));
		m_pBigWordBrowser->setText(word.m_word);
		m_pExplainBrowser->setText(word.m_trans);

		switch (m_method)
		{
		case 0: // Chinese to English
			if (m_methodStep == 0)
			{
				m_pWordBrowser->hide();
				m_pExplainBrowser->show();
				m_pExitButton->hide();
				m_pMarkButton->hide();
				m_pWordButton->show();
				m_pForgetButton->hide();
				m_pRememberButton->hide();
				m_pExplainButton->hide();
			}
			else
			{
				m_pWordBrowser->show();
				m_pExplainBrowser->show();
				m_pExitButton->show();
				m_pMarkButton->show();
				m_pWordButton->hide();
				m_pForgetButton->show();
				m_pRememberButton->show();
				m_pExplainButton->hide();
			}
			break;

		case 1:// English to Chinese
			if (m_methodStep == 0)
			{
				m_pWordBrowser->hide();
				m_pBigWordBrowser->show();
				m_pExplainBrowser->hide();
				m_pExitButton->hide();
				m_pMarkButton->hide();
				m_pWordButton->hide();
				m_pForgetButton->hide();
				m_pRememberButton->hide();
				m_pExplainButton->show();
			}
			else
			{
				m_pWordBrowser->show();
				m_pBigWordBrowser->hide();
				m_pExplainBrowser->show();
				m_pExitButton->show();
				m_pMarkButton->show();
				m_pWordButton->hide();
				m_pForgetButton->show();
				m_pRememberButton->show();
				m_pExplainButton->hide();
			}
			break;

		default:
			m_pWordButton->hide();
			m_pExplainButton->hide();
			break;
		}
	}
}

void RoteWidget::onClickWordButton()
{
	m_methodStep = 1;
	m_pExitButton->show();
	m_pMarkButton->show();
	m_pWordButton->hide();
	m_pWordBrowser->show();
	m_pBigWordBrowser->hide();
	m_pForgetButton->show();
	m_pRememberButton->show();
	m_pExplainBrowser->show();
}

void RoteWidget::onClickExplainButton()
{
	m_methodStep = 1;
	m_pWordBrowser->show();
	m_pBigWordBrowser->hide();
	m_pExplainBrowser->show();
	m_pExitButton->show();
	m_pMarkButton->show();
	m_pForgetButton->show();
	m_pRememberButton->show();
	m_pExplainButton->hide();
}

void RoteWidget::onClickExit()
{
	QString softwareVersion("Rote ");

	int ret;
	int failedWord = getForgottonWordCount();

	BOOK_INFO newBookInfo;

	/* Get New book info */
	g_roteManager.saveRoteResult();
	g_pBookInfo->refreshBookInfo();
	g_pMainWindow->getCurrentBookInfo(newBookInfo);

	/* Print Summary*/
	QString summary;
	summary.sprintf("Total score: %d\nL0: %d\nL1: %d\nL2: %d\nL3: %d\nL4: %d\nL5: %d\nL6: %d\nL7+: %d"
		, newBookInfo.m_totalScore - m_oldBookInfo.m_totalScore
		, newBookInfo.m_level0 - m_oldBookInfo.m_level0
		, newBookInfo.m_level1 - m_oldBookInfo.m_level1
		, newBookInfo.m_level2 - m_oldBookInfo.m_level2
		, newBookInfo.m_level3 - m_oldBookInfo.m_level3
		, newBookInfo.m_level4 - m_oldBookInfo.m_level4
		, newBookInfo.m_level5 - m_oldBookInfo.m_level5
		, newBookInfo.m_level6 - m_oldBookInfo.m_level6
		, newBookInfo.m_level7 - m_oldBookInfo.m_level7 );
	
	/* There is still some words is forgotten */
	if (failedWord != 0)
	{
		QString message(tr("Roting will be abort. There are still %1 failed words. Do you want to review the failed words?").arg(failedWord));
		ret = QMessageBox::warning(this, tr("warning"), QString(tr("Software Version: %1.").arg(SOFT_VERSION)) + QString("\n") + message + QString("\n") + summary, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
		if (ret == QMessageBox::Yes)
		{
			/* We will rote the failed words */
			g_roteManager.clearUnrotedWord();
			refresh();
		}
		else if (ret == QMessageBox::No)
		{
			/* We will abort the roting, and no word will be roted any more */
			g_pMainWindow->onClickExitRote();
		}
		else
		{
			/* We select cancel, which means we will continue to rote the book */
		}
	}
	else
	{
		QString message(tr("Are you sure to exit roting?"));
		ret = QMessageBox::warning(this, tr("warning"), QString(tr("Software Version: %1.").arg(SOFT_VERSION)) + QString("\n") + message + QString("\n") + summary, QMessageBox::Yes, QMessageBox::Cancel);
		if (ret == QMessageBox::Yes)
		{
			g_pMainWindow->onClickExitRote();
		}
		else
		{
			/* We select cancel, which means we will continue to rote the book */
		}		
	}
}

void RoteWidget::resizeEvent(QResizeEvent *)
{
	int w = width();
	int h = height();

	int left = w / 20;
	int right = w * 19 / 20;
	int buttonWidth = w / 5;

    m_pLeftLabel->setGeometry(right - buttonWidth * 2, 0, buttonWidth * 2, h / 30 + 20);

	m_pWordBrowser->setGeometry(left, 5, right - left, h / 4 + h / 14 - 15);
	m_pBigWordBrowser->setGeometry(left, 5, right - left, h / 4  + h / 14 - 15);
//	m_pWordBrowser->setGeometry(left, 20, right - left, h / 4 - 40);

	m_pExitButton->setGeometry(right - buttonWidth, h / 4, buttonWidth, h / 14);	
	m_pMarkButton->setGeometry(right - buttonWidth - buttonWidth - w / 20, h / 4, buttonWidth, h / 14);	

//	m_pExitButton->setGeometry(left, h / 4 - 5, 80, h / 14);	

	int buttonTop = h * 3 / 4 + 30;
	m_pExplainBrowser->setGeometry(left, h / 4  + h / 14, right - left, buttonTop - h / 4 - h / 14 - 20);
//	m_pExplainBrowser->setGeometry(left, h / 4 , right - left, 2);

	m_pRememberButton->setGeometry(left, buttonTop, w / 2 - left, h / 4 - 40);
	m_pForgetButton->setGeometry(w / 2, buttonTop, w / 2 - left, h / 4 - 40);


	m_pWordButton->setGeometry(w / 4, buttonTop , w / 2, h / 4 - 40);

	m_pExplainButton->setGeometry(w / 8, h * 3 / 8, w * 3 / 4, h / 2);
}

void RoteWidget::paintEvent(QPaintEvent * /*event */)
{
    QPainter painter(this);
	QLinearGradient linearGradient(QPointF(0, 0), QPointF(height(), 0));         
	linearGradient.setColorAt(0.0, QColor(215, 215, 215));
	linearGradient.setColorAt(0.2, QColor(245, 245, 245));
	linearGradient.setColorAt(0.4, QColor(215, 215, 215));
	linearGradient.setColorAt(0.6, QColor(245, 245, 245));
	linearGradient.setColorAt(0.8, QColor(215, 215, 215));
	linearGradient.setColorAt(1.0, QColor(245, 245, 245));
	linearGradient.setSpread(QGradient::PadSpread);  
	painter.setBrush(linearGradient); 

	painter.drawRect(0, 0, width(), height());
}

void RoteWidget::onClickForget()
{
	YoudaoWord *pWord = g_roteManager.getWord(m_currentWordIndex);
	/* If the word is roted for the first time, we need to record it in the browser widget */
	if ((g_roteManager.getRoteCount(m_currentWordIndex) == 0) && (m_nForgottenWordCount < TOTAL_ROTE_COUNT))
	{
		m_nForgottenWordCount++;
	}

	g_roteManager.setRoteResult(m_currentWordIndex, false);
	m_methodStep = 0;
	refresh();
}

void RoteWidget::onClickRemember()
{
	g_roteManager.setRoteResult(m_currentWordIndex, true);
	m_methodStep = 0;
	refresh();
}


void RoteWidget::onClickMark()
{
	YoudaoWord *pWord = g_roteManager.getWord(m_currentWordIndex);
	g_database.addBrowseWord(g_pMainWindow->getSelectedBook(), pWord->m_word);
}
