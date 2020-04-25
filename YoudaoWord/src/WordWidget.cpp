#include "WordWidget.h"
#include "widget.h"
#include <QDebug>

WordWidget::WordWidget(QWidget *parent) :
    QWidget(parent)
{
	m_pSettingLabel = new QLabel(tr("Input one or more words:"), this);
	m_pReminderLabel = new QLabel(tr("(Each word should be separated by Comma (,))"), this);

	m_pExportButton = new QPushButton(tr("Export Words"), this);
	connect(m_pExportButton, SIGNAL(clicked()), g_pMainWidget, SLOT(onExportWords()));

	m_pViewButton = new QPushButton(tr("View Words"), this);
	connect(m_pViewButton, SIGNAL(clicked()), g_pMainWidget, SLOT(onViewWords()));

	m_pWordLine = new QLineEdit(this);
}


WordWidget::~WordWidget(void)
{
}

void WordWidget::resizeEvent(QResizeEvent *)
{
	m_pSettingLabel->setGeometry(10, 40, 250, 16);
	m_pReminderLabel->setGeometry(10, 60, 250, 16);
	m_pWordLine->setGeometry(10, 100, 320, 21);

	m_pViewButton->setGeometry(160, 150, 81, 23);
	m_pExportButton->setGeometry(260, 150, 81, 23);
}

QStringList WordWidget::getWordList()
{
	QString strBuffer = m_pWordLine->text().trimmed();
	strBuffer.replace('\n', ';');
	strBuffer.replace(',', ';');
	QStringList list = strBuffer.split(';');
	QStringList newList;
	for (int i = 0; i < list.count(); i++)
	{
		QString word = list.at(i).trimmed();
		if (!word.isEmpty())
		{
			newList.append(word);
		}
	}

	return (newList);
}