#include "NumberButton.h"
#include "mainwindow.h"

NumberButton::NumberButton(QWidget *parent)
    : QPushButton("0", parent)
{
	m_value = 0;
	m_min = 0;
	m_max = 10000;

	setObjectName(QStringLiteral("NumberButton"));
	connect(this,  SIGNAL(clicked()), this, SLOT(onClickButton()));
}

void NumberButton::setValue(int value)
{
	m_value = value;
	this->setText(QString::number(value));
}

int NumberButton::getValue()
{
	return (m_value);
}

void NumberButton::setRange(int min, int max)
{
	if (min <= max)
	{
		m_min = min;
		m_max = max;
	}
}

void NumberButton::onClickButton()
{
	NumberDialog  numberDialog(g_pMainWindow);
	numberDialog.setInitValue(m_value, m_min, m_max);
	numberDialog.setModal(true);
	if (numberDialog.exec() == QDialog::Rejected)
	{
		return;
	}

	int value = numberDialog.getValue();
	if (value != m_value)
	{
		m_value = value;
		emit (valueChanged(value));
		setText(QString::number(m_value));
	}
}

NumberDialog::NumberDialog(QWidget *parent) 
	: QDialog(parent)
{
	m_buttonClickCount = 0;

	m_value = 0;
	m_min = 0;
	m_max = 10000;

	m_pLineEdit = new QLineEdit(this);
	m_pLineEdit->setReadOnly(true);
	m_pLineEdit->setObjectName(QStringLiteral("NumberDialogEditLine"));


	m_pOKButton = new QPushButton(tr("OK"), this);
	connect(m_pOKButton,  SIGNAL(clicked()), this, SLOT(onClickButtonOK()));

	m_pCancelButton = new QPushButton(tr("Cancel"), this);
	connect(m_pCancelButton,  SIGNAL(clicked()), this, SLOT(onClickButtonCancel()));

	m_pButton0 = new QPushButton("0", this);
	connect(m_pButton0,  SIGNAL(clicked()), this, SLOT(onClickButton0()));

	m_pButton1 = new QPushButton("1", this);
	connect(m_pButton1,  SIGNAL(clicked()), this, SLOT(onClickButton1()));

	m_pButton2 = new QPushButton("2", this);
	connect(m_pButton2,  SIGNAL(clicked()), this, SLOT(onClickButton2()));

	m_pButton3 = new QPushButton("3", this);
	connect(m_pButton3,  SIGNAL(clicked()), this, SLOT(onClickButton3()));

	m_pButton4 = new QPushButton("4", this);
	connect(m_pButton4,  SIGNAL(clicked()), this, SLOT(onClickButton4()));

	m_pButton5 = new QPushButton("5", this);
	connect(m_pButton5,  SIGNAL(clicked()), this, SLOT(onClickButton5()));

	m_pButton6 = new QPushButton("6", this);
	connect(m_pButton6,  SIGNAL(clicked()), this, SLOT(onClickButton6()));

	m_pButton7 = new QPushButton("7", this);
	connect(m_pButton7,  SIGNAL(clicked()), this, SLOT(onClickButton7()));

	m_pButton8 = new QPushButton("8", this);
	connect(m_pButton8,  SIGNAL(clicked()), this, SLOT(onClickButton8()));

	m_pButton9 = new QPushButton("9", this);
	connect(m_pButton9,  SIGNAL(clicked()), this, SLOT(onClickButton9()));

	m_pButtonBackSpace = new QPushButton(tr("<--"), this);
	connect(m_pButtonBackSpace,  SIGNAL(clicked()), this, SLOT(onClickButtonBackSpace()));

	m_pButtonClear = new QPushButton(tr("Clear"), this);
	connect(m_pButtonClear,  SIGNAL(clicked()), this, SLOT(onClickButtonClear()));

	m_pButtonDot = new QPushButton(".", this);
	m_pButtonDot->setEnabled(false);
	connect(m_pButtonDot,  SIGNAL(clicked()), this, SLOT(onClickButtonDot()));

	setFixedSize(g_pMainWindow->width(), g_pMainWindow->height());
}

int NumberDialog::getValue()
{
	return (m_value);
}

void NumberDialog::setInitValue(int value, int min, int max)
{
	m_value = value;
	m_min = min;
	m_max = max;

	m_pLineEdit->setText(QString::number(value));
}

void NumberDialog::onClickButton0()
{
	onClickButton(0);
}

void NumberDialog::onClickButton1()
{
	onClickButton(1);
}

void NumberDialog::onClickButton2()
{
	onClickButton(2);
}

void NumberDialog::onClickButton3()
{
	onClickButton(3);
}

void NumberDialog::onClickButton4()
{
	onClickButton(4);
}

void NumberDialog::onClickButton5()
{
	onClickButton(5);
}

void NumberDialog::onClickButton6()
{
	onClickButton(6);
}

void NumberDialog::onClickButton7()
{
	onClickButton(7);
}

void NumberDialog::onClickButton8()
{
	onClickButton(8);
}

void NumberDialog::onClickButton9()
{
	onClickButton(9);
}

void NumberDialog::onClickButton(int value)
{
	if (m_buttonClickCount == 0)
	{
		m_buttonClickCount = 1;
		m_value = 0;
	}
	if (m_value * 10 + value < m_max)
	{
		m_value = m_value * 10 + value;
		m_pLineEdit->setText(QString::number(m_value));
	}
}

void NumberDialog::onClickButtonBackSpace()
{
	m_buttonClickCount = 1;
	m_value = m_value / 10;
	m_pLineEdit->setText(QString::number(m_value));
}

void NumberDialog::onClickButtonClear()
{
	m_buttonClickCount = 1;
	m_value = 0;
	m_pLineEdit->setText("0");
}


void NumberDialog::onClickButtonOK()
{
	accept();
}

void NumberDialog::onClickButtonCancel()
{
	reject();
}

void NumberDialog::resizeEvent(QResizeEvent * /* event */)
{
	int intervalH = height() / 50;
	int intervalW = width() / 50;

	int buttonWidth = 75;
	int buttonHeight = 25;

	if (intervalH < 5)
	{
		intervalH = 5;
	}
	if (intervalW < 5)
	{
		intervalW = 5;
	}

	buttonHeight = (height() - 6 * intervalH) / 5.2;
	buttonWidth = (width() - 5 * intervalW) / 5;

	int labelHeight = height() - 6 * intervalH - 4 * buttonHeight;

	int buttonY1 = intervalW * 2 + labelHeight;
	m_pLineEdit->setGeometry(intervalH, intervalW, width() - 2 * intervalH, labelHeight);
	m_pButton1->setGeometry(intervalH, buttonY1, buttonWidth,  buttonHeight);
	m_pButton2->setGeometry(intervalH * 2 + buttonWidth, buttonY1, buttonWidth,  buttonHeight);
	m_pButton3->setGeometry(intervalH * 3 + 2 * buttonWidth, buttonY1, buttonWidth,  buttonHeight);
	m_pButtonBackSpace->setGeometry(intervalH * 4 + 3 * buttonWidth, buttonY1, width() - intervalH * 5 - 3 * buttonWidth ,  buttonHeight);

	int buttonY2 = buttonY1 + buttonHeight + intervalH;
	m_pButton4->setGeometry(intervalH, buttonY2, buttonWidth,  buttonHeight);
	m_pButton5->setGeometry(intervalH * 2 + buttonWidth, buttonY2, buttonWidth,  buttonHeight);
	m_pButton6->setGeometry(intervalH * 3 + 2 * buttonWidth, buttonY2, buttonWidth,  buttonHeight);
	m_pButtonClear->setGeometry(intervalH * 4 + 3 * buttonWidth, buttonY2, width() - intervalH * 5 - 3 * buttonWidth ,  buttonHeight);

	int buttonY3 = buttonY2 + buttonHeight + intervalH;
	m_pButton7->setGeometry(intervalH, buttonY3, buttonWidth,  buttonHeight);
	m_pButton8->setGeometry(intervalH * 2 + buttonWidth, buttonY3, buttonWidth,  buttonHeight);
	m_pButton9->setGeometry(intervalH * 3 + 2 * buttonWidth, buttonY3, buttonWidth,  buttonHeight);
	m_pOKButton->setGeometry(intervalH * 4 + 3 * buttonWidth, buttonY3, width() - intervalH * 5 - 3 * buttonWidth ,  buttonHeight);

	int buttonY4 = buttonY3 + buttonHeight + intervalH;
	m_pButton0->setGeometry(intervalH, buttonY4, buttonWidth * 2 + intervalH,  buttonHeight);
	m_pButtonDot->setGeometry(intervalH * 3 + 2 * buttonWidth, buttonY4, buttonWidth,  buttonHeight);
	m_pCancelButton->setGeometry(intervalH * 4 + 3 * buttonWidth, buttonY4, width() - intervalH * 5 - 3 * buttonWidth ,  buttonHeight);

	//	m_pOKButton->setGeometry(10, 10, buttonWidth, buttonHeight);
//	m_pCancelButton->setGeometry(10, 40, buttonWidth, buttonHeight);
}