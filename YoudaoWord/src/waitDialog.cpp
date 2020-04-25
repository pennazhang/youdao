#include "WaitDialog.h"
#include <QMessageBox>
#include <QTextBlock>
#include "YoudaoDatabase.h"
#include <QKeyEvent>
#include "widget.h"

WaitDialog::WaitDialog(QWidget *parent)
	: QDialog(parent)
{
	m_pRemainder = new QLabel(tr("Copying..."), this);
	m_pRemainder->setAlignment(Qt::AlignHCenter);

	m_pWaitingWidget = new BackgroundWidget(QString(":ActivityIndicator.png"), this, 20, 2);

	setMinimumSize(160, 110);
}


WaitDialog::~WaitDialog(void)
{
}

void WaitDialog::resizeEvent(QResizeEvent *)
{
	int w = 60;
	m_pRemainder->setGeometry(width () / 2 - w, height() / 2 + 25, w * 2, 25);

	w = m_pWaitingWidget->pixelWidth();
	int h =  m_pWaitingWidget->pixelHeight();

	m_pWaitingWidget->setGeometry((width() - w) / 2, (height() - h) / 2 - 15, w, h);
}

