#ifndef CHECKBUTTON_H
#define CHECKBUTTON_H

#include <QPushButton>
#include <QString>

class CheckButton : public QPushButton
{
	Q_OBJECT
public:
    CheckButton(QString buttonText, QWidget *pParent);

	void setCheckFlag(bool flag);
	bool getCheckFlag() { return m_checkFlag; }

private:
	bool m_checkFlag;
};

#endif // CHECKBUTTON_H
