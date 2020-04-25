#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class WordWidget : 	public QWidget
{
    Q_OBJECT

public:
	WordWidget(QWidget *parent);
	~WordWidget(void);
	QStringList getWordList();

protected:
	void resizeEvent(QResizeEvent *);

protected slots:

protected:
	QLabel *m_pSettingLabel, *m_pReminderLabel;
	QPushButton *m_pExportButton, *m_pViewButton;
	QLineEdit *m_pWordLine;
};

