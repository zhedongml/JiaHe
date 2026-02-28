#include "fancytab.h"
#include <QWidget>
#include <QDebug>
using namespace Core;

void FancyTab::fadeIn()
{
    m_animator.stop();
    m_animator.setDuration(80);
    m_animator.setEndValue(1);
    m_animator.start();
}

void FancyTab::fadeOut()
{
    m_animator.stop();
    m_animator.setDuration(160);
    m_animator.setEndValue(0);
    m_animator.start();
}

void FancyTab::setFader(qreal value)
{
    m_fader = value;
    m_tabbar->update();
}
