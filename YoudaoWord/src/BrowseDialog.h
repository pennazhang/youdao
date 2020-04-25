#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QKeyEvent>

class BrowseDialog : public QDialog
{
    Q_OBJECT

public:
	BrowseDialog(QWidget *parent = 0);
	~BrowseDialog(void);

protected slots:
	void onOK(void);
	void onDelete(void);
	void onEdit(void);
	void onCancel(void);
	void refresh();
	void onSelectionChanged();

protected:
	void resizeEvent(QResizeEvent *);
	void keyPressEvent(QKeyEvent *event);

protected:
	QPushButton *m_pExitButton, *m_pDeleteButton, *m_pEditButton; 
	QCheckBox *m_pExplainCheckBox;
	QTextEdit *m_pTextView;
	QLabel *m_pTotalCount;
};

