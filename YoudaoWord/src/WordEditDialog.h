#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>



class WordEditDialog : public QDialog
{
    Q_OBJECT

public:
	explicit WordEditDialog(QWidget *parent);
	~WordEditDialog(void);
	void setWord(QString word, QString phonetic, QString explain);
	QString getWord();
	QString getTrans();
	QString getPhoentic();

protected slots:
	void onUpdate();
	void onCancel();

protected:
	QLabel *m_pWordLabel, *m_pExplainLabel, *m_phoneticLabel;
	QPushButton *m_pUpdateButton, *m_pCancelButton;
	QTextEdit *m_pWordEdit, *m_pExplainEdit, *m_phoneticEdit;

	QString m_word, m_phonetic, m_explain;
};

