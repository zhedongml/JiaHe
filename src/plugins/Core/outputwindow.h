#pragma once

#include "core_global.h"
#include "icontext.h"
#include "outputformat.h"

#include <QPlainTextEdit>


namespace Core {

class OutputFormatter;
class OutputWindowPrivate;

class CORE_EXPORT OutputWindow : public QPlainTextEdit
{
    Q_OBJECT

public:
    enum class FilterModeFlag {
        Default       = 0x00, // Plain text, non case sensitive, for initialization
        RegExp        = 0x01,
        CaseSensitive = 0x02,
        Inverted      = 0x04,
    };
    Q_DECLARE_FLAGS(FilterModeFlags, FilterModeFlag)

    OutputWindow(Context context, const QString &settingsKey, QWidget *parent = nullptr);
    ~OutputWindow() override;

    OutputFormatter *outputFormatter() const;

    void appendMessage(const QString &out,OutputFormat format);

    void grayOutOldContent();
    void clear();
    void flush();
    void reset();

    void scrollToBottom();

    void setMaxCharCount(int count);
    int maxCharCount() const;

    void setBaseFont(const QFont &newFont);
    float fontZoom() const;
    void setFontZoom(float zoom);
    void resetZoom() { setFontZoom(0); }
    void setWheelZoomEnabled(bool enabled);

    void updateFilterProperties(
            const QString &filterText,
            Qt::CaseSensitivity caseSensitivity,
            bool regexp,
            bool isInverted);

    void appendText(const QString& text, int format);

signals:
    void wheelZoom();
    void appendTextSignal(const QString& text, int format);

public slots:
    void setWordWrapEnabled(bool wrap);
    void appendTextSlot(const QString& text, int format);

private:
    QMimeData *createMimeDataFromSelection() const override;
    void keyPressEvent(QKeyEvent *ev) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void showEvent(QShowEvent *) override;
    void wheelEvent(QWheelEvent *e) override;

    using QPlainTextEdit::setFont; // call setBaseFont instead, which respects the zoom factor
    void enableUndoRedo();
    void filterNewContent();
    void handleNextOutputChunk();
    void handleOutputChunk(const QString &output,OutputFormat format);
    void updateAutoScroll();

    OutputWindowPrivate *d = nullptr;
};

} // namespace Core
