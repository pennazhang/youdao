#include "WordEditDialog.h"

WordEditDialog::WordEditDialog(QWidget *pParent)
	: QDialog(pParent)
{
	m_pWordLabel = new QLabel(tr("word:"), this);
	m_pExplainLabel = new QLabel(tr("Explain:"), this);
	m_phoneticLabel = new QLabel(tr("Phonetic:"), this);

	m_pUpdateButton = new QPushButton(tr("Update"), this);
	connect(m_pUpdateButton, SIGNAL(clicked()), this, SLOT(onUpdate()));

	m_pCancelButton = new QPushButton(tr("Cancel"), this);
	connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

	m_pWordEdit = new QTextEdit(this);
	m_pWordEdit->setFontPointSize(10);
	m_pWordEdit->setAcceptRichText(false);

	m_pExplainEdit = new QTextEdit(this);
	m_pExplainEdit->setFontPointSize(10);
	m_pExplainEdit->setAcceptRichText(false);

	m_phoneticEdit = new QTextEdit(this);
	m_phoneticEdit->setFontPointSize(10);
	m_phoneticEdit->setAcceptRichText(false);

	int left = 20;
	int width = 640;
	int height = 480;
	int buttonWidth = 100;

	m_pWordLabel->setGeometry(left, 10, 120, 20);
	m_pWordEdit->setGeometry(left, 35, width - 2 * left, 50);

	m_phoneticLabel->setGeometry(left, 90, 120, 20);
	m_phoneticEdit->setGeometry(left, 115, width - 2 * left, 50);

	m_pExplainLabel->setGeometry(left, 170, 120, 20);
	m_pExplainEdit->setGeometry(left, 195, width - 2 * left, height - 195 - 40);
	m_pUpdateButton->setGeometry(width / 3 - buttonWidth / 2, height - 30, buttonWidth, 23);
	m_pCancelButton->setGeometry(width * 2 / 3 - buttonWidth / 2, height - 30, buttonWidth, 23);
	setFixedSize(width, height);
}

WordEditDialog::~WordEditDialog(void)
{

}

void WordEditDialog::onUpdate(void)
{
	m_word = m_pWordEdit->toPlainText();
	m_phonetic = m_phoneticEdit->toPlainText();
	m_explain = m_pExplainEdit->toPlainText();

	accept();
}

void WordEditDialog::onCancel(void)
{
	reject();
}


void WordEditDialog::setWord(QString word, QString phonetic, QString explain)
{
	m_word = word;
	m_pWordEdit->setText(word);

	m_phonetic = phonetic;
	m_phoneticEdit->setText(phonetic);

	m_explain = explain;
	m_pExplainEdit->setText(explain);
}

QString WordEditDialog::getWord()
{
	return m_word;
}

QString WordEditDialog::getTrans()
{
	return m_explain;
}

QString WordEditDialog::getPhoentic()
{
	return m_phonetic;
}