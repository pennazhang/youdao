#include "BrowseDialog.h"
#include <QMessageBox>
#include <QTextBlock>
#include "YoudaoDatabase.h"
#include <QKeyEvent>
#include "widget.h"

BrowseDialog::BrowseDialog(QWidget *parent)
	: QDialog(parent)
{
	m_pExplainCheckBox = new QCheckBox(tr("Explain"), this);
	connect(m_pExplainCheckBox, SIGNAL(clicked()), this, SLOT(refresh()));

	m_pExitButton = new QPushButton(tr("Exit"), this);
	connect(m_pExitButton, SIGNAL(clicked()), this, SLOT(onCancel()));

	m_pDeleteButton = new QPushButton(tr("Delete"), this);
	connect(m_pDeleteButton, SIGNAL(clicked()), this, SLOT(onDelete()));

	m_pEditButton = new QPushButton(tr("Edit"), this);
	connect(m_pEditButton, SIGNAL(clicked()), this, SLOT(onEdit()));

	m_pTextView = new QTextEdit(this);
	m_pTextView->setReadOnly(true);
	m_pTextView->setObjectName(QStringLiteral("m_pTextView"));
	m_pTextView->setFont(QFont("Tahoma", 10));
	connect(m_pTextView, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

	m_pTotalCount = new QLabel(this);
	m_pTotalCount->setAlignment(Qt::AlignRight);

	setMinimumSize(480, 640);

	refresh();
}


BrowseDialog::~BrowseDialog(void)
{
}

void BrowseDialog::resizeEvent(QResizeEvent *)
{
	int buttonWidth = width() / 12;

	m_pExplainCheckBox->setGeometry(20, 5, 200, 20);
	m_pTextView->setGeometry(20, 25, width() - 40, height() - 80);
	m_pTotalCount->setGeometry(width() - 200, 5, 180, 20);
	m_pExitButton->setGeometry(width () * 3 / 4 - buttonWidth, height() - 40, buttonWidth * 2, 25);
	m_pDeleteButton->setGeometry(width () / 4 - buttonWidth, height() - 40, buttonWidth * 2, 25);
	m_pEditButton->setGeometry(width () / 2 - buttonWidth, height() - 40, buttonWidth * 2, 25);
}

void BrowseDialog::onOK(void)
{
	accept();
}

void BrowseDialog::onDelete(void)
{
	QTextCursor cursor = m_pTextView->textCursor();
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

	if (firstLine > lastLine)
	{
		return;
	}

	int lineIndex = firstLine / 2;
	QString word = g_roteManager.getWord(lineIndex)->m_word;

	if (QMessageBox::warning(this, tr("Delete Word"), tr("You have select %1 words. Do you want to delete them all: %2...").arg((lastLine - firstLine) / 2 + 1).arg(word), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
	{
		return;
	}

	for (int index = lastLine; index >= firstLine; index -= 2)
	{
		lineIndex = index / 2;
		word = g_roteManager.getWord(lineIndex)->m_word;
		qDebug() << "Delete word " << word;
		g_pMainWidget->deleteWord(word);
		g_roteManager.deleteWord(lineIndex);
	}

#if 0
	refresh();
#else
	for (int line = lastLine + 1; line >= firstLine; line--)
	{
		QTextBlock block = m_pTextView->document()->findBlockByNumber(line);
		if (block.isValid())
		{
			QTextCursor blockCursor(block);
			cursor.select(QTextCursor::BlockUnderCursor);
			cursor.deletePreviousChar();
			cursor.removeSelectedText();
		}
	}
#endif
}

void BrowseDialog::onEdit(void)
{
	QTextCursor cursor = m_pTextView->textCursor();
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
	QString word = g_roteManager.getWord(lineIndex)->m_word;
	QString newWord, newTrans, phoentic;
	bool changedFlag = false;

	if (word.isEmpty() == false)
	{
		changedFlag = g_pMainWidget->editWord(word, newWord, phoentic, newTrans);
		if (changedFlag == true)
		{
			g_roteManager.getWord(lineIndex)->m_word = newWord;
			g_roteManager.getWord(lineIndex)->m_phonetic = phoentic;
			g_roteManager.getWord(lineIndex)->m_trans = newTrans;
			refresh();
		}
	}
}

void BrowseDialog::onCancel(void)
{
	reject();
}

void BrowseDialog::refresh(void)
{
	QString msg;
	bool explainFlag = m_pExplainCheckBox->isChecked();

	for (int i = 0; i < g_roteManager.getTotalWordCount(); i++)
	{
		if (explainFlag == true)
		{
			msg += g_roteManager.getWord(i)->m_word + "  " + g_roteManager.getWord(i)->m_phonetic;
			msg += QString("\n");
			msg += g_roteManager.getWord(i)->m_trans;
			msg += QString("\n");
		}
		else
		{
			msg += g_roteManager.getWord(i)->m_word;
			msg += QString("\n");		
		}

		/* We did not expect the last \n */
		if (i < g_roteManager.getTotalWordCount() - 1)
		{
			msg += QString("\n");
		}
	}
	m_pTextView->setText(msg);

    msg.sprintf("Total %d", g_roteManager.getTotalWordCount());
	m_pTotalCount->setText(msg);

	onSelectionChanged();
}

void BrowseDialog::onSelectionChanged()
{
	if (m_pExplainCheckBox->isChecked() == true)
	{
		m_pDeleteButton->setEnabled(false);
		m_pEditButton->setEnabled(false);
		return;
	}

	QTextCursor cursor = m_pTextView->textCursor();
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

	m_pEditButton->setEnabled(firstLine == lastLine);
	m_pDeleteButton->setEnabled(firstLine <= lastLine);
	return;
}

void BrowseDialog::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Delete:
		if (m_pDeleteButton->isEnabled() == true)
		{
			emit m_pDeleteButton->click();
		}
		break;
	}
}
