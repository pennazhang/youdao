#ifndef BACKGROUND_WIDGET_H
#define BACKGROUND_WIDGET_H
#include <QFrame>
#include <qtimer.h>
#include <QPixmap>

class BackgroundWidget : public QFrame
{
    Q_OBJECT
public:
	BackgroundWidget(QString backgroundImage, QWidget *parent, int ms = 0, int stepAngle = 0);
	void rotateEnable(bool flag);

	int pixelWidth() { return m_background.width(); }
	int pixelHeight() { return m_background.height(); }


protected slots:
	void onTimerOut();

protected:
	void paintEvent(QPaintEvent *event);

	QPixmap m_background;
	QTimer *m_pTimer;

	int m_rotateAngle;
	int m_stepAngle;
	bool m_rotateFlag;
};

#endif
