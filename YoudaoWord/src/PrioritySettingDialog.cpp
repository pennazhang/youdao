#include "PrioritySettingDialog.h"
#include <QMessageBox>


PrioritySettingDialog::PrioritySettingDialog(QWidget *parent)
	: QDialog(parent)
{
	m_pPriority = new QLabel(" Familiarity = -T + A*(B)", this);
	m_pPriority->setObjectName(QStringLiteral("Equalization"));
	m_pPriority->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	m_pPriority->setGeometry(10, 20, 226, 41);

	m_pN = new QLabel("n-1", this);
	m_pN->setGeometry(240, 28, 46, 13);

	m_pA = new QLabel("A", this);
	m_pA->setGeometry(30, 80, 16, 16);

	m_pB = new QLabel("B", this);
	m_pB->setGeometry(160, 80, 16, 16);

	m_pTLabel = new QLabel(tr("T: time interval between the last roted day and today"), this);
	m_pTLabel->setGeometry(30, 110, 200, 16);

	m_pnLabel = new QLabel(tr("n: Famility Level, start from 1"), this);
	m_pnLabel->setGeometry(30, 140, 200, 16);

	m_pOKButton = new QPushButton("OK", this);
	m_pOKButton->setGeometry(50, 170, 75, 23);
	connect(m_pOKButton, SIGNAL(clicked()), this, SLOT(onOK()));

	m_pCancelButton = new QPushButton("Cancel", this);
	m_pCancelButton->setGeometry(160, 170, 75, 23);
	connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

	m_pAEdit = new QLineEdit("10", this);
	m_pAEdit->setGeometry(50, 80, 61, 20);

	m_pBEdit = new QLineEdit("1.2", this);
	m_pBEdit->setGeometry(180, 80, 61, 20);

	setFixedSize(279, 199);
}


PrioritySettingDialog::~PrioritySettingDialog(void)
{
}

void PrioritySettingDialog::onOK(void)
{
	int a = 0;
	qreal b = 0;
	a = m_pAEdit->text().toInt();
	if ((a < 1) || (a > 1000))
	{
		QMessageBox::warning(this, "Invalid value for A", "Please re-enter the valid value for parameter: A, it should be 1 - 1000", "OK");
		return;
	}

	b = m_pBEdit->text().toFloat();
	if ((b < 1) || (b > 3))
	{
		QMessageBox::warning(this, "Invalid value for B", "Please re-enter the valid value for parameter: B, it should be 1.0 - 3.0", "OK");
		return;
	}

	m_A = a;
	m_B = b;

	accept();
}

void PrioritySettingDialog::onCancel(void)
{
	reject();
}