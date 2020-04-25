#include "TextView.h"
#include <QScrollEvent>
#include <qDebug>
#include <QScroller>
#include <QEasingCurve>

TextView::TextView(QWidget *parent)
	: QTextEdit(parent)
{
    setReadOnly(true);

	/* We need to add touch operation */
    QScroller *scroller = QScroller::scroller(this);
    QScrollerProperties prop = scroller->scrollerProperties();

    prop.setScrollMetric(QScrollerProperties::AxisLockThreshold, 0.66);
    prop.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve(QEasingCurve::OutExpo));
    prop.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.05);
    prop.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.635);
    prop.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.33);
    prop.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0.33);
    prop.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.93);
    prop.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);

    scroller->setScrollerProperties(prop);

    scroller->grabGesture(this, QScroller::TouchGesture);
    scroller->grabGesture(this, QScroller::LeftMouseButtonGesture);

}

bool TextView::event(QEvent *pEvent)
{
#if 0
    static QEvent::Type lastType = QEvent::Paint;
    QScrollEvent *pScrollEvent;

    if (pEvent->type() == QEvent::InputMethodQuery || pEvent->type() == QEvent::Enter)
    {
        pEvent->ignore();
        return true;
    }

    switch (pEvent->type())
    {
        case QEvent::Scroll:
            pScrollEvent = static_cast<QScrollEvent*>(pEvent);
            qDebug() << pScrollEvent->scrollState() << "(" << pScrollEvent->contentPos() << ")";
            break;
        case QEvent::Paint: /* Paint event shouldn't after ScrollPrepare */
            if (lastType == QEvent::ScrollPrepare)
            {
                pEvent->ignore();
                return (true);
            }
            break;
    }

    //    qDebug() << "crurent Type" << pEvent->type() << "   last Type " << lastType;
    qDebug() << "crurent Type" << pEvent->type();

    lastType = pEvent->type();
#endif
	return (QTextEdit::event(pEvent));
}
