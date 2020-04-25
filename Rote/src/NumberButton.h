#ifndef NUMBERBUTTON_H
#define NUMBERBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QDialog>
#include <QLineEdit>

class NumberDialog : public QDialog
{
	Q_OBJECT

public:
    explicit NumberDialog(QWidget *parent = 0);
	void setInitValue(int value, int min, int max);
	int getValue();
		
protected:
    void resizeEvent(QResizeEvent * /* event */);
	void onClickButton(int value);

private slots:
	void onClickButtonOK();
	void onClickButtonCancel();
	void onClickButton0();
	void onClickButton1();
	void onClickButton2();
	void onClickButton3();
	void onClickButton4();
	void onClickButton5();
	void onClickButton6();
	void onClickButton7();
	void onClickButton8();
	void onClickButton9();
	void onClickButtonBackSpace();
	void onClickButtonClear();

protected:
	int m_value, m_min, m_max, m_buttonClickCount;
	QLineEdit *m_pLineEdit; 
	QPushButton *m_pButton0, *m_pButton1, *m_pButton2, *m_pButton3, *m_pButton4, 
		*m_pButton5, *m_pButton6, *m_pButton7, *m_pButton8, *m_pButton9, *m_pButtonBackSpace, *m_pButtonDot,
		*m_pButtonClear, *m_pOKButton, *m_pCancelButton;
};

class NumberButton : public QPushButton
{
	Q_OBJECT

public:
    explicit NumberButton(QWidget *parent = 0);
	void setValue(int value);
	int getValue();
	void setRange(int min, int max);


signals:
	void valueChanged(int);

private slots:
	void onClickButton();


protected:
	int m_value, m_min, m_max;
};

#endif