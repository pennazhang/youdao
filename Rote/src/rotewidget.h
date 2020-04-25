#ifndef ROTEWIDGET_H
#define ROTEWIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QLabel>
#include "TextView.h"
#include "YoudaoDatabase.h"
#include "bookInfo.h"

class RoteWidget : public QWidget
{
	Q_OBJECT

public:
    explicit RoteWidget(QWidget *parent = 0);
	void startRote();
	int getForgottonWordCount() { return m_nForgottenWordCount; }
	void setMethod(int method) { m_method = method; m_methodStep = 0; }

protected:
	void resizeEvent(QResizeEvent * /* event */);
	void paintEvent(QPaintEvent * /*event */);
	void refresh();

protected slots:
	void onClickExit();
	void onClickMark();
	void onClickWordButton();
	void onClickExplainButton();
	void onClickRemember();
	void onClickForget();

protected:
    TextView *m_pWordBrowser, *m_pBigWordBrowser;
    TextView *m_pExplainBrowser;
	QPushButton *m_pForgetButton, *m_pRememberButton, *m_pExitButton, *m_pExplainButton, *m_pWordButton, *m_pMarkButton;
	QLabel	*m_pLeftLabel;
	QPixmap m_background;
	int m_currentWordIndex;
	int m_method, m_methodStep;	/* 0: Chinese to English, 1: English to Chinese, 2: Dictation */

	int m_nForgottenWordCount;
	BOOK_INFO m_oldBookInfo;
};




#endif // ROTEWIDGET_H
