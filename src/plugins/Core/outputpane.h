#pragma once

#include "core_global.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSplitter;
QT_END_NAMESPACE

namespace Core {

class OutputPanePlaceHolderPrivate;

class CORE_EXPORT OutputPanePlaceHolder : public QWidget
{
    Q_OBJECT

public:
    explicit OutputPanePlaceHolder(QSplitter *parent = nullptr, QString mode = "");
    ~OutputPanePlaceHolder() override;

    static OutputPanePlaceHolder *getCurrent();
    static bool isCurrentVisible();

    bool isMaximized() const;
    void setMaximized(bool maximize);
    void ensureSizeHintAsMinimum();
    int nonMaximizedSize() const;
    void showWidget(QWidget* widget,bool visible = true);
signals:
    void visibilityChangeRequested(bool visible);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *) override;

private:
    void setHeight(int height);
    void currentModeChanged(QString mode);

    OutputPanePlaceHolderPrivate *d;
    int m_outputPaneHeightSetting = 180;
};

} // namespace Core
