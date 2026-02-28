#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QStringList>
#include "prjcommon_global.h"

class PRJCOMMON_EXPORT MultiLineInputDialog : public QDialog {
    Q_OBJECT
public:
    explicit MultiLineInputDialog(
        const QString& title,
        const QStringList& labels = {},
        bool noLabelMode = false,
        QWidget* parent = nullptr);

    QStringList texts() const;

private:
    QList<QLineEdit*> m_inputs;
};
