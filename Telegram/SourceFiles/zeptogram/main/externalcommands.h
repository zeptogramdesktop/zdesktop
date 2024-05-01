#ifndef EXTERNALCOMMANDS_H
#define EXTERNALCOMMANDS_H

#include <QSize>
#include <QPoint>
#include <QWidget>
#include <QMouseEvent>
#include <QApplication>

struct ClickOnWidget
{
    void operator()(QWidget* widget)
    {
        QSize widgetSize = widget->geometry().size();

        double x = (double)widgetSize.width() / 2;
        double y = (double)widgetSize.height() / 2;

        QPoint localPos = QPoint(x, y);
        QPoint windowPos = widget->mapToGlobal(localPos);
        QPoint screenPos = widget->mapToGlobal(localPos);


        QMouseEvent* pressEvent = new QMouseEvent(QEvent::MouseButtonPress, localPos, windowPos, screenPos,
            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

        QMouseEvent* releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, localPos, windowPos, screenPos,
            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

        (void)QApplication::instance()->sendEvent(widget, pressEvent);
        QApplication::processEvents();

        (void)QApplication::instance()->sendEvent(widget, releaseEvent);
        QApplication::processEvents();
    }
};

struct PointClickOnWidget
{
    void operator()(QWidget* widget, QPoint point)
    {
        QPoint windowPos = widget->mapToGlobal(point);
        QPoint screenPos = widget->mapToGlobal(point);

        QMouseEvent* pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, windowPos, screenPos,
            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

        QMouseEvent* releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, windowPos, screenPos,
            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

        (void)QApplication::instance()->sendEvent(widget, pressEvent);
        QApplication::processEvents();

        (void)QApplication::instance()->sendEvent(widget, releaseEvent);
        QApplication::processEvents();
    }
};

struct PointClickOnWidgetWithMouse
{
    void operator()(QWidget* widget, QPoint point, bool leftButton = true)
    {
        QPoint windowPos = widget->mapToGlobal(point);
        QPoint screenPos = widget->mapToGlobal(point);



        QMouseEvent* pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, windowPos, screenPos, 
            leftButton ? Qt::LeftButton : Qt::RightButton, leftButton ? Qt::LeftButton : Qt::RightButton, Qt::NoModifier);

        QMouseEvent* releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, windowPos, screenPos,
            leftButton ? Qt::LeftButton : Qt::RightButton, leftButton ? Qt::LeftButton : Qt::RightButton, Qt::NoModifier);

        (void)QApplication::instance()->sendEvent(widget, pressEvent);
        QApplication::processEvents();

        (void)QApplication::instance()->sendEvent(widget, releaseEvent);
        QApplication::processEvents();
    }
};

#endif // EXTERNALCOMMANDS_H
