#include "backgroundWidget.h"
#include <QPainter>

BackgroundWidget::BackgroundWidget(QString backgroundImage, QWidget *parent, int ms, int stepAngle)
    : QFrame(parent)
{
	m_background = QPixmap(backgroundImage);
	m_rotateAngle = 0;
	m_stepAngle = stepAngle;

	if ((ms != 0) && (stepAngle != 0))
	{
		m_pTimer = new QTimer(this);
		m_pTimer->start(ms);
		connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
		m_rotateFlag = true;
	}
	else
	{
		m_rotateFlag = false;
	}
}

void BackgroundWidget::paintEvent(QPaintEvent * /*event */)
{
    QPainter painter(this);
	painter.translate(width() / 2, height() / 2); //��ͼƬ��������Ϊ��ת������
    painter.rotate(m_rotateAngle); //˳ʱ����ת90��
	painter.translate(-width() / 2, -height() / 2); //��ͼƬ��������Ϊ��ת������
    painter.drawPixmap(0, 0, m_background.scaled(size()));
}

void BackgroundWidget::onTimerOut()
{
	if (m_rotateFlag == true)
	{
		m_rotateAngle += m_stepAngle;
		repaint();
	}
}

void BackgroundWidget::rotateEnable(bool flag)
{
	m_rotateFlag = flag;
}