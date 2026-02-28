#pragma once

#include <QObject>
#include <QSpinBox>
#include <QStyledItemDelegate>
#include <QItemDelegate> 
#include <QKeyEvent>

class CustomSpinboxDelegate : public QStyledItemDelegate
{
  public:
    CustomSpinboxDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent)
    {
    }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setDecimals(DECIMAL);
        editor->setSingleStep(1);
        editor->setMinimum(-1000);
        editor->setMaximum(1000);
        return editor;
    }
    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        float value = index.model()->data(index, Qt::EditRole).toFloat();
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(editor);
        spinBox->setValue(value);
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(editor);
        spinBox->interpretText();
        float value = spinBox->value();
        model->setData(index, QString::number(value, 'f', DECIMAL), Qt::EditRole);
    }
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        editor->setGeometry(option.rect);
    }

    bool eventFilter(QObject *editor, QEvent *event)
    {
        // 监听键盘事件
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            // 按下回车键
            if (keyEvent->key() == Qt::Key_Return)
            {
                //qDebug() << "Ate key press" << keyEvent->key();
                // 提交当前项的数据
                emit commitData((QWidget *)editor);
                // 关闭编辑器并指向下一项
                closeEditor((QWidget *)editor, QAbstractItemDelegate::EditNextItem);
                return true;
            }
        }
        return QStyledItemDelegate::eventFilter(editor, event);
    }

  private:
    const int DECIMAL = 2;
};
