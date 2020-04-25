#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class MiscWidget : 	public QWidget
{
    Q_OBJECT

public:
	MiscWidget(QWidget *parent);
	~MiscWidget(void);

protected:
	void resizeEvent(QResizeEvent *);

protected slots:
	void onCaptureChinese();

protected:
	QPushButton *m_pChineseButton;
};

