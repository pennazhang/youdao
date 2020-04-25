
#include "checkbutton.h"
#include <QVariant>

CheckButton::CheckButton(QString text, QWidget *parent)
    : QPushButton(text, parent)
{
	m_checkFlag = false;
	setFixedSize(87, 35);
}

void CheckButton::setCheckFlag(bool flag) 
{ 
	m_checkFlag = flag;

	if (flag == true)
	{
		setStyleSheet("CheckButton { background: url(:/select.png); }");
	}
	else
	{
		setStyleSheet("CheckButton { background: url(:/unselect.png); }");	
	}
}
