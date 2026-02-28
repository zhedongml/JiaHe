#ifndef FANCYTAB_H
#define FANCYTAB_H

#include <QString>
#include <QIcon>
#include <QPropertyAnimation>
namespace Core {
    class FancyTab : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(qreal fader READ fader WRITE setFader)

    public:
        FancyTab(QWidget* tabbar)
            : m_tabbar(tabbar)
        {
            m_animator.setPropertyName("fader");
            m_animator.setTargetObject(this);
        }

        qreal fader() const { return m_fader; }
        void setFader(qreal qreal);

        void fadeIn();
        void fadeOut();

        QIcon icon;
        QString text;
        QString toolTip;
        bool enabled = true;
        bool hasMenu = false;

        int fullpageStackId = -1;
        int resultPartStackId = -1;
        int logicPartStackId = -1;

    private:
        QPropertyAnimation m_animator;
        QWidget* m_tabbar;
        qreal m_fader = 0;
    };
}
#endif // FANCYTAB_H
