/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "outputformatter.h"
#include "ansiescapecodehandler.h"
#include <QDir>
#include <QFileInfo>
#include <QPair>
#include <QPlainTextEdit>
#include <QPointer>
#include <QRegularExpressionMatch>
#include <QTextCursor>
#include <optional>
#include <numeric>

namespace Core {

class OutputFormatter::Private
{
public:
    QPlainTextEdit *plainTextEdit = nullptr;
    QTextCharFormat formats[NumberOfFormats];
    QTextCursor cursor;
    AnsiEscapeCodeHandler escapeCodeHandler;
    QPair<QString, OutputFormat> incompleteLine;
    std::optional<QTextCharFormat> formatOverride;
    bool boldFontEnabled = true;
    bool prependCarriageReturn = false;
    bool prependLineFeed = false;
};

OutputFormatter::OutputFormatter() : d(new Private) { }

OutputFormatter::~OutputFormatter()
{
    delete d;
}

QPlainTextEdit *OutputFormatter::plainTextEdit() const
{
    return d->plainTextEdit;
}

void OutputFormatter::setPlainTextEdit(QPlainTextEdit *plainText)
{
    d->plainTextEdit = plainText;
    d->cursor = plainText ? plainText->textCursor() : QTextCursor();
    d->cursor.movePosition(QTextCursor::End);
    initFormats();
}

void OutputFormatter::doAppendMessage(const QString &text, OutputFormat format)
{
    QTextCharFormat charFmt = charFormat(format);

    QList<FormattedText> formattedText = parseAnsi(text, charFmt);
    const QString cleanLine = std::accumulate(formattedText.begin(), formattedText.end(), QString(),
            [](const FormattedText &t1, const FormattedText &t2) -> QString
            { return t1.text + t2.text; });
    
    append(cleanLine, charFmt);
}

QTextCharFormat OutputFormatter::charFormat(OutputFormat format) const
{
    return d->formatOverride ? d->formatOverride.value() : d->formats[format];
}

QList<FormattedText> OutputFormatter::parseAnsi(const QString &text, const QTextCharFormat &format)
{
    return d->escapeCodeHandler.parseText(FormattedText(text, format));
}

void OutputFormatter::append(const QString &text, const QTextCharFormat &format)
{
    if (!plainTextEdit())
        return;
    flushTrailingNewline();
    int startPos = 0;
    int crPos = -1;
    while ((crPos = text.indexOf('\r', startPos)) >= 0)  {
        d->cursor.insertText(text.mid(startPos, crPos - startPos), format);
        d->cursor.clearSelection();
        d->cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
        startPos = crPos + 1;
    }
    if (startPos < text.count())
        d->cursor.insertText(text.mid(startPos), format);
}

void OutputFormatter::clearLastLine()
{
    // Note that this approach will fail if the text edit is not read-only and users
    // have messed with the last line between programmatic inputs.
    // We live with this risk, as all the alternatives are worse.
    if (!d->cursor.atEnd())
        d->cursor.movePosition(QTextCursor::End);
    d->cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    d->cursor.removeSelectedText();
}

void OutputFormatter::initFormats()
{
    if (!plainTextEdit())
        return;

    d->formats[NormalMessageFormat].setForeground(QColor(153));
    d->formats[ErrorMessageFormat].setForeground(QColor(151));
    d->formats[LogMessageFormat].setForeground(QColor(156));
    d->formats[StdOutFormat].setForeground(QColor("gray"));
    d->formats[StdErrFormat].setForeground(QColor("red"));
    d->formats[DebugFormat].setForeground(QColor("blue"));
    d->formats[GeneralMessageFormat].setForeground(QColor("lemonchiffon"));
    setBoldFontEnabled(d->boldFontEnabled);
}

void OutputFormatter::flushIncompleteLine()
{
    clearLastLine();
    doAppendMessage(d->incompleteLine.first, d->incompleteLine.second);
    d->incompleteLine.first.clear();
}

void  OutputFormatter::flushTrailingNewline()
{
    if (d->prependLineFeed) {
        d->cursor.insertText("\n");
        d->prependLineFeed = false;
    }
}

void OutputFormatter::dumpIncompleteLine(const QString &line, OutputFormat format)
{
    if (line.isEmpty())
        return;
    append(line, charFormat(format));
    d->incompleteLine.first.append(line);
    d->incompleteLine.second = format;
}

void OutputFormatter::clear()
{
    if (plainTextEdit())
        plainTextEdit()->clear();
}

void OutputFormatter::reset()
{
    d->prependCarriageReturn = false;
    d->incompleteLine.first.clear();
    d->formatOverride.reset();
    d->escapeCodeHandler = AnsiEscapeCodeHandler();
}

void OutputFormatter::setBoldFontEnabled(bool enabled)
{
    d->boldFontEnabled = enabled;
    const QFont::Weight fontWeight = enabled ? QFont::Bold : QFont::Normal;
    d->formats[NormalMessageFormat].setFontWeight(fontWeight);
    d->formats[ErrorMessageFormat].setFontWeight(fontWeight);
}

void OutputFormatter::flush()
{
    if (!d->incompleteLine.first.isEmpty())
        flushIncompleteLine();
    flushTrailingNewline();
    d->escapeCodeHandler.endFormatScope();
}


void OutputFormatter::appendMessage(const QString &text, OutputFormat format)
{
    if (text.isEmpty())
        return;

    // If we have an existing incomplete line and its format is different from this one,
    // then we consider the two messages unrelated. We re-insert the previous incomplete line,
    // possibly formatted now, and start from scratch with the new input.
    if (!d->incompleteLine.first.isEmpty() && d->incompleteLine.second != format)
        flushIncompleteLine();

    QString out = text;
    if (d->prependCarriageReturn) {
        d->prependCarriageReturn = false;
        out.prepend('\r');
    }
    QString res = out;
    const auto newEnd = std::unique(res.begin(), res.end(), [](const QChar& c1, const QChar& c2) {
        return c1 == '\r' && c2 == '\r'; // QTCREATORBUG-24556
        });
    res.chop(std::distance(newEnd, res.end()));
    res.replace("\r\n", "\n");
    out=res;
    if (out.endsWith('\r')) {
        d->prependCarriageReturn = true;
        out.chop(1);
    }

    // If the input is a single incomplete line, we do not forward it to the specialized
    // formatting code, but simply dump it as-is. Once it becomes complete or it needs to
    // be flushed for other reasons, we remove the unformatted part and re-insert it, this
    // time with proper formatting.
    if (!out.contains('\n')) {
        dumpIncompleteLine(out, format);
        return;
    }

    // We have at least one complete line, so let's remove the previously dumped
    // incomplete line and prepend it to the first line of our new input.
    if (!d->incompleteLine.first.isEmpty()) {
        clearLastLine();
        out.prepend(d->incompleteLine.first);
        d->incompleteLine.first.clear();
    }

    // Forward all complete lines to the specialized formatting code, and handle a
    // potential trailing incomplete line the same way as above.
    for (int startPos = 0; ;) {
        const int eolPos = out.indexOf('\n', startPos);
        if (eolPos == -1) {
            dumpIncompleteLine(out.mid(startPos), format);
            break;
        }
        doAppendMessage(out.mid(startPos, eolPos - startPos), format);
        d->prependLineFeed = true;
        startPos = eolPos + 1;
    }
}

} // namespace Utils
