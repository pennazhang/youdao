#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QResizeEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QPixmap>
#include <QListWidget>
#include "TextView.h"

class InfoWidget : public QWidget
{
	Q_OBJECT

public:
    explicit InfoWidget(QWidget *parent = 0);
    ~InfoWidget();
	void refresh();
	void refreshBook();
	QString getSelectedBook() { return m_selectBook; }
	int getCurrentBookIndex();

protected:
	void resizeEvent(QResizeEvent * /* event */);
	void paintEvent(QPaintEvent * /*event */);

private slots:
	void onClickRote();
	void onNextBook();
	void onSelectBook();
	void onClickNote();
	void onClickReview();
	void bookSelected(QListWidgetItem * item);

protected:
	QLabel *m_pCurrentBook, *m_pLevel0, *m_pLevel1, *m_pLevel2, *m_pLevel3, *m_pLevel4, *m_pLevel5, *m_pLevel6, *m_pAbove7, *m_pTotal;
	QPushButton *m_pNextButton, *m_pRoteButton, *m_pBrowseButton, *m_pNoteButton;
	QLabel *m_pBookName;
	int m_bookIndex;
	QPixmap m_background;
	QString m_selectBook;
	TextView *m_pBriefEdit;
	
	QListWidget *m_pListWidget;
	bool m_selectBookFlag;  // true: we are selecting a book.  false: we have selected a book.
};

#endif // INFOWIDGET_H
