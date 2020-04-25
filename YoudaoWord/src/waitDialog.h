#pragma once

#include <QDialog>
#include <QLabel>
#include "backgroundWidget.h"

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
	WaitDialog(QWidget *parent = 0);
	~WaitDialog(void);

protected slots:

protected:
	void resizeEvent(QResizeEvent *);

protected:
	QLabel *m_pRemainder;
	BackgroundWidget *m_pWaitingWidget;
};

