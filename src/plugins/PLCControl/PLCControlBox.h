#pragma once

#include <QWidget>
#include "Core/itoolbox.h"
#include "plccontrol_global.h"
#include "PLCController.h"
#include "ui_PLCControlBox.h"
#include <QLabel>
#include <QMouseEvent>

class StateLabel : public QLabel
{
    Q_OBJECT

  signals:
    void changeState(int,bool);

  public:
    StateLabel(int index, QWidget *parent = Q_NULLPTR);

  public:
    void TurnOff();
    void TurnOn();
    void ChangeState(int state);
    void SetEnable(bool enable);

  protected:
    virtual void mousePressEvent(QMouseEvent *ev) override;

 private:
    bool m_State;
    int m_Index;
    QString m_Red;
    QString m_Green;
    QString m_Grey;
    bool m_Enable;
};

class PLCCONTROL_EXPORT PLCControlBox : public Core::IToolBox
{
    Q_OBJECT

public slots:
    void changeSatet(int, bool);
    void changeStateOnUI(int, int);

public:
    PLCControlBox(QString toolboxName = "", QWidget *parent = nullptr);
    ~PLCControlBox();

public:
    void SetModel(PLCController *model);

private:
    void initTableStyle();

private:
    Ui::PLCControlBox ui;
    PLCController *m_Model;
    QMap<int, StateLabel *> m_StateLabels;
};
