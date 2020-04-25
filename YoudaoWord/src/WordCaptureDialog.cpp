#include "WordCaptureDialog.h"
#include <QMessageBox>
#include <QTextBlock>
#include "YoudaoDatabase.h"
#include <QKeyEvent>
#include "widget.h"

WordCaptureDialog::WordCaptureDialog(QWidget *parent)
	: QDialog(parent)
{
	m_pExitButton = new QPushButton(tr("Exit"), this);
	connect(m_pExitButton, SIGNAL(clicked()), this, SLOT(onCancel()));

	m_pChineseButton = new QPushButton(tr("Chinese"), this);
	connect(m_pChineseButton, SIGNAL(clicked()), this, SLOT(onCpatureChinese()));

	m_pEnglishButton = new QPushButton(tr("English"), this);
	connect(m_pEnglishButton, SIGNAL(clicked()), this, SLOT(onCpatureEnglish()));

	m_pTextView = new QTextEdit(this);
	m_pTextView->setObjectName(QStringLiteral("m_pTextView"));
	m_pTextView->setFont(QFont("Tahoma", 10));
	connect(m_pTextView, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

	m_pWordView = new QTextEdit(this);
	m_pWordView->setReadOnly(true);
	m_pWordView->setObjectName(QStringLiteral("m_pWordView"));
	m_pWordView->setFont(QFont("Tahoma", 10));
//	connect(m_pTextView, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

	m_pWordLable = new QLabel(tr("Word:"), this);
//	m_pWordLable->setAlignment(Qt::AlignRight);

	m_pTextLable = new QLabel(tr("Text:"), this);

	setMinimumSize(480, 640);
	onTextChanged();
}


WordCaptureDialog::~WordCaptureDialog(void)
{
}

void WordCaptureDialog::resizeEvent(QResizeEvent *)
{
	int buttonWidth = width() / 12;

	m_pTextLable->setGeometry(10, 5, 200, 20);
	m_pTextView->setGeometry(10, 25, width() - 130, height() - 80);
	m_pWordView->setGeometry(width() - 110, 25, 100, height() - 80);
	m_pWordLable->setGeometry(width() - 110, 5, 180, 20);
	m_pExitButton->setGeometry(width () * 3 / 4 - buttonWidth, height() - 40, buttonWidth * 2, 25);
	m_pChineseButton->setGeometry(width () / 4 - buttonWidth, height() - 40, buttonWidth * 2, 25);
	m_pEnglishButton->setGeometry(width () / 2 - buttonWidth, height() - 40, buttonWidth * 2, 25);
}

void WordCaptureDialog::onOK(void)
{
	accept();
}

void WordCaptureDialog::onCpatureChinese(void)
{
	QString strText = m_pTextView->toPlainText();
	int len = strText.length();
	int i;
	QString newText;
	QStringList newWordList;
	for (i = 0; i < len; i++)
	{
		QChar ch = strText.at(i);
	    ushort uni = ch.unicode();   
		if((uni >= 0x4E00) && (uni <= 0x9FA5))   
		{
			/* This is a chinese word */
			int index = newWordList.indexOf(ch);
			if (index == -1)
			{
				newWordList.append(ch);
				newText += ch;
				newText += QString("\r\n");
			}
		}
	}
	m_pWordView->setText(newText);
}

void WordCaptureDialog::onCpatureEnglish(void)
{
	QString strText = m_pTextView->toPlainText();
	int len = strText.length();
	int i;
	ushort uni_a = QChar('a').unicode();
	ushort uni_z = QChar('z').unicode();
	ushort uni_A = QChar('A').unicode();
	ushort uni_Z = QChar('Z').unicode();

	for (i = 0; i < len; i++)
	{
		QChar ch = strText.at(i);
		ushort uni = ch.unicode();

		if ((uni >= uni_a) && (uni <= uni_z))
		{
			/* This is a lower letter, we will keep it unchanged */
		}
		else if ((uni >= QChar('A').unicode()) && (uni <= QChar('Z').unicode()))
		{
			strText[i] = ch.toLower();
		}
		else
		{
			strText[i] = QChar(' ');
		}
	}

	QStringList list = strText.split(' ');

	QStringList newWordList;
	QString newText;
	for (i = 0; i < list.count(); i++)
	{
		QString newWord = list.at(i);
		if (newWord.length() >= 2)
		{
			int index = newWordList.indexOf(newWord);
			if (index == -1)
			{
				newWordList.append(newWord);
				newText += newWord;
				newText += QString("\r\n");
			}
		}
	}
	m_pWordView->setText(newText);
}

void WordCaptureDialog::onCancel(void)
{
	reject();
}

void WordCaptureDialog::onTextChanged()
{
	if (m_pTextView->toPlainText().isEmpty())
	{
		m_pChineseButton->setEnabled(false);
		m_pEnglishButton->setEnabled(false);
	}
	else
	{
		m_pChineseButton->setEnabled(true);
		m_pEnglishButton->setEnabled(true);
	}
}

void WordCaptureDialog::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Delete:
		if (m_pChineseButton->isEnabled() == true)
		{
			emit m_pChineseButton->click();
		}
		break;
	}
}
