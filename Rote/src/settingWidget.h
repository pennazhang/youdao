#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QResizeEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QPixmap>
#include "YoudaoDatabase.h"
#include <QSpinBox>
#include "NumberButton.h"

class SettingWidget : public QWidget
{
	Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = 0);
    ~SettingWidget();
	bool loadDatabase(QString strDBName);
    void refresh();
	void saveRoteSetting();

protected:
    void resizeEvent(QResizeEvent * /* event */);
    void paintEvent(QPaintEvent * /*event */);
	void refreshMethod();

private slots:
	void onBrowse();
    void onCancel();
    void onStart();
	void onTotalCountChanged(int);
	void onL0CountChanged(int);
	void onL1CountChanged(int);
	void onL2CountChanged(int);
	void onHighestLevelChanged(int);
	void onClickLevel1();
	void onClickLevel2();
	void onClickMethodButton();

protected:
	bool m_refreshFlag;
	QLabel * m_pTotalEditLabel, *m_pLevel0EditLabel, *m_pLevel1EditLabel, *m_pLevel2EditLabel, *m_pHighestLevelEditLabel; 
	QLabel *m_pLevel0, *m_pLevel1, *m_pLevel2, *m_pLevel3, *m_pLevel4, *m_pLevel5, *m_pLevel6, *m_pAbove7, *m_pTotal;
	QPushButton *m_pCancelButton, *m_pStartButton, *m_pBrowseButton;
	QPushButton *m_pLevel1Button, *m_pLevel2Button, *m_pMethodButton;
	NumberButton *m_pTotalEdit, *m_pLevel0Edit, *m_pLevel1Edit, *m_pLevel2Edit, *m_pHighestLevelEdit;
//	int m_bookIndex;
	QPixmap m_background;
	QFrame *m_pLine;

	YoudaoWord *m_pAllBookWord;
	QString		m_dbFileName;
	int			m_wordCount, m_level0Count, m_level1Count, m_level2Count;
	int			m_methodIndex;	/* 0: Chinese to English, 1: English to Chinese, 2: Dictation */
		
	int m_total, m_l0Count, m_l1Count, m_l2Count, m_highestLevel;
	bool		m_l1Flag, m_l2Flag;
};

#endif // INFOWIDGET_H
