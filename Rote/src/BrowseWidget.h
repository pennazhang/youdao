#ifndef BROWSEWIDGET_H
#define BROWSEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QResizeEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QPixmap>
#include "YoudaoDatabase.h"
#include <QSpinBox>
#include "TextView.h"

class BrowseWidget : public QWidget
{
	Q_OBJECT

public:
    explicit BrowseWidget(QWidget *parent = 0);
    ~BrowseWidget();
	void loadDatabase(QString strDBName);
	void loadRoteManager(RoteManager *pRoteManager);
//	void loadForgottenWord(BrowseData *pForgottonWord, int forgottonWordCount);
	void loadForgottenWord();
    void refresh();
	void removeWord(int wordIndex);
	void clearDeleteFlag()  { m_bDeleteFlag = false; }
	bool getDeleteFlag()	{ return m_bDeleteFlag; }

protected:
    void resizeEvent(QResizeEvent * /* event */);
    void paintEvent(QPaintEvent * /*event */);

private slots:
    void onDeleteWord();
    void onClickExplain();
	void onBrowseExit();
	void onSelectionChanged();

protected:
    TextView *m_pBrowser;
    QPushButton *m_pExplainButton, *m_pExitButton, *m_pDeleteButton;
//	QLabel * m_pTotalEditLabel, *m_pLevelEditLabel, *m_pReservedLabel; 
//	QLabel *m_pLevel1, *m_pLevel2, *m_pLevel3, *m_pLevel4, *m_pLevel5, *m_pLevel6, *m_pAbove7, *m_pTotal;
//	QLabel *m_pLevel1Count, *m_pLevel2Count, *m_pLevel3Count, *m_pLevel4Count, *m_pLevel5Count, *m_pLevel6Count, *m_pAboveCount, *m_pTotalCount;
//	QPushButton *m_pCancelButton, *m_pStartButton;
//	QPushButton *m_pLevel1Button, *m_pReverseButton;
//	QSpinBox *m_pTotalEdit, *m_pLevel1Edit;
	QPixmap m_background;
	BrowseData *m_pBrowserWord;
	QLabel *m_pTotalCount;
	int m_wordCount;

	bool m_bDeleteFlag;
//	YoudaoWord *m_pAllBookWord;
//	QString		m_dbFileName;
//	int			m_wordCount, m_l1Count;
//	bool		m_refreshFlag;
};

#endif // INFOWIDGET_H
