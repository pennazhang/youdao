#include "MiscWidget.h"
#include "widget.h"
#include <QDebug>
#include <QMessageBox>
#include "WordCaptureDialog.h"

MiscWidget::MiscWidget(QWidget *parent) :
    QWidget(parent)
{
	m_pChineseButton = new QPushButton(tr("Capture Chinese"), this);
	connect(m_pChineseButton, SIGNAL(clicked()), this, SLOT(onCaptureChinese()));
}


MiscWidget::~MiscWidget(void)
{
}

void MiscWidget::resizeEvent(QResizeEvent *)
{
	m_pChineseButton->setGeometry(200, 150, 141, 23);
}

void MiscWidget::onCaptureChinese()
{
	WordCaptureDialog dlg;
	dlg.exec();
}
