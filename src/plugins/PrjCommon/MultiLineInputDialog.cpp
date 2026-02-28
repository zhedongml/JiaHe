#include "MultiLineInputDialog.h"

MultiLineInputDialog::MultiLineInputDialog(
    const QString& title,
    const QStringList& labels,
    bool noLabelMode,
    QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(title);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    if (!noLabelMode && !labels.isEmpty()) {
        for (const QString& label : labels) {
            QLineEdit* edit = new QLineEdit(this);
            //edit->setPlaceholderText(QString("Please enter %1").arg(label));
            formLayout->addRow(label + ":", edit);
            m_inputs.append(edit);
        }
    }
    else {
        int lineCount = labels.isEmpty() ? 1 : labels.size();
        for (int i = 0; i < lineCount; ++i) {
            QLineEdit* edit = new QLineEdit(this);
            //edit->setPlaceholderText(QString("Please enter content %1").arg(i + 1));
            formLayout->addRow(edit);
            m_inputs.append(edit);
        }
    }

    mainLayout->addLayout(formLayout);

    QDialogButtonBox* buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    setMinimumWidth(400);
}

QStringList MultiLineInputDialog::texts() const
{
    QStringList result;
    for (auto* edit : m_inputs) {
        result << edit->text();
    }
    return result;
}
