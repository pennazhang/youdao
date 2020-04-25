#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QString>

class ImportWidget : public QWidget
{
    Q_OBJECT

public:
	explicit ImportWidget(QWidget *parent = 0);
	~ImportWidget() {}; 
	bool getReplaceFlag();
	void setButtonEnable(bool flag);
	void setRestoreEnable(bool flag);

protected:
	void resizeEvent(QResizeEvent *);

protected:
	QLabel *m_pSettingLabel;
	QPushButton *m_pImportButton, *m_pRestoreButton;
	QRadioButton *m_pRadioButton1, *m_pRadioButton2;
};
