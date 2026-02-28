
#pragma once

#include "core_global.h"

#include <QList>
#include <QObject>
#include <QPointer>
#include <QWidget>

#include <functional>

namespace Core {

class CORE_EXPORT Context
{
public:
    Context() = default;

    explicit Context(QString c1) { add(c1); }
    Context(QString c1, QString c2) { add(c1); add(c2); }
    Context(QString c1, QString c2, QString c3) { add(c1); add(c2); add(c3); }
    bool contains(QString c) const { return d.contains(c); }
    int size() const { return d.size(); }
    bool isEmpty() const { return d.isEmpty(); }
    QString at(int i) const { return d.at(i); }

    // FIXME: Make interface slimmer.
    using const_iterator = QList<QString>::const_iterator;
    const_iterator begin() const { return d.begin(); }
    const_iterator end() const { return d.end(); }
    int indexOf(QString c) const { return d.indexOf(c); }
    void removeAt(int i) { d.removeAt(i); }
    void prepend(QString c) { d.prepend(c); }
    void add(const Context &c) { d += c.d; }
    void add(QString c) { d.append(c); }
    bool operator==(const Context &c) const { return d == c.d; }

private:
    QList<QString> d;
};

class CORE_EXPORT IContext : public QObject
{
    Q_OBJECT
public:
    IContext(QObject *parent = nullptr) : QObject(parent) {}

    virtual Context context() const { return m_context; }
    virtual QWidget *widget() const { return m_widget; }
    virtual void setContext(const Context &context) { m_context = context; }
    virtual void setWidget(QWidget *widget) { m_widget = widget; }

    virtual QWidget* logicWidget() const { return m_logicWidget; }
    virtual void setLogicWidget(QWidget* widget) { m_logicWidget = widget; }

    virtual QWidget* resultWidget() const { return m_resultWidget; }
    virtual void setResultWidget(QWidget* widget) { m_resultWidget = widget; }

protected:
    Context m_context;
    QPointer<QWidget> m_widget;
    QPointer<QWidget> m_logicWidget=nullptr;
    QPointer<QWidget> m_resultWidget = nullptr;
};

} // namespace Core
CORE_EXPORT QDebug operator<<(QDebug debug, const Core::Context &context);
