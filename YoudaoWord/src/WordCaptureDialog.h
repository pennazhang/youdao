#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QKeyEvent>

class WordCaptureDialog : public QDialog
{
    Q_OBJECT

public:
	WordCaptureDialog(QWidget *parent = 0);
	~WordCaptureDialog(void);

protected slots:
	void onOK(void);
	void onCpatureChinese(void);
	void onCpatureEnglish(void);
	void onCancel(void);
	void onTextChanged();

protected:
	void resizeEvent(QResizeEvent *);
	void keyPressEvent(QKeyEvent *event);

protected:
	QPushButton *m_pExitButton, *m_pChineseButton, *m_pEnglishButton; 
	QCheckBox *m_pExplainCheckBox;
	QTextEdit *m_pTextView, *m_pWordView;
	QLabel *m_pTextLable, *m_pWordLable;
};

