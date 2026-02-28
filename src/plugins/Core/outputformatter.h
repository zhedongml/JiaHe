#pragma once

#include "core_global.h"
#include "outputformat.h"

#include <QObject>

#include <functional>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QRegularExpressionMatch;
class QTextCharFormat;
class QTextCursor;
QT_END_NAMESPACE

namespace Core {
class FormattedText;

class  OutputFormatter : public QObject
{
    Q_OBJECT
public:
    OutputFormatter();
    ~OutputFormatter() override;

    QPlainTextEdit *plainTextEdit() const;
    void setPlainTextEdit(QPlainTextEdit *plainText);

    void appendMessage(const QString &text, OutputFormat format);
    void flush(); // Flushes in-flight data.
    void clear(); // Clears the text edit, if there is one.
    void reset(); // Wipes everything except the text edit.

    void setBoldFontEnabled(bool enabled);


#ifndef WITH_TESTS
private:
#endif
    QTextCharFormat charFormat(OutputFormat format) const;

private:
    void doAppendMessage(const QString &text, OutputFormat format);

    void append(const QString &text, const QTextCharFormat &format);
    void initFormats();
    void flushIncompleteLine();
    void flushTrailingNewline();
    void dumpIncompleteLine(const QString &line, OutputFormat format);
    void clearLastLine();
    QList<FormattedText> parseAnsi(const QString &text, const QTextCharFormat &format);

    class Private;
    Private * const d;
};


} // namespace Utils
