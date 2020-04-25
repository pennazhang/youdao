#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class PrioritySettingDialog : public QDialog
{
    Q_OBJECT

public:
	PrioritySettingDialog(QWidget *parent = 0);
	~PrioritySettingDialog(void);
	void setA(int param) { m_A = param; m_pAEdit->setText(QString::number(param));}
	int getA(void) { return m_A;	}
	void setB(qreal param) { m_B = param; m_pBEdit->setText(QString::number(param));}
	qreal getB(void) { return m_B;	}

protected slots:
	void onOK(void);
	void onCancel(void);

protected:
	QLabel *m_pTLabel, *m_pnLabel;
	QLabel *m_pPriority, *m_pN, *m_pA, *m_pB;
	QPushButton *m_pOKButton, *m_pCancelButton;
	QLineEdit *m_pAEdit, *m_pBEdit;

	int m_A;
	qreal m_B;
};

