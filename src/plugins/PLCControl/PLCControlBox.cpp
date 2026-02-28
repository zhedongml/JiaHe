#include "PLCControlBox.h"
#include <Core/icore.h>

StateLabel::StateLabel(int index, QWidget *parent) : QLabel(parent), m_State(false), m_Index(index), m_Enable(true)
{
    setAlignment(Qt::AlignCenter);
    /* setFixedWidth(40);
     setFixedHeight(40);
     setStyleSheet("QLabel{background-color:#a9b2b2;}");*/
    m_Grey = "min-width:20px; min-height:20px;max-width:20px;max-height:20px;border-radius:10px;border:1px solid "
             "black;background:grey";
    m_Red = "min-width:20px; min-height:20px;max-width:20px;max-height:20px;border-radius:10px;border:1px solid "
            "black;background:red";
    m_Green = "min-width:20px; min-height:20px;max-width:20px;max-height:20px;border-radius:10px;border:1px solid "
              "black;background:#32CD32";
    setStyleSheet(m_Grey);
}

void StateLabel::TurnOff()
{
    setStyleSheet(m_Grey);
    m_State = false;
}

void StateLabel::TurnOn()
{
    setStyleSheet(m_Green);
    m_State = true;
}

void StateLabel::ChangeState(int state)
{
    if (state)
    {
        TurnOn();
    }
    else
    {
        TurnOff();
    }
}

void StateLabel::SetEnable(bool enable)
{
    m_Enable = enable;
}

void StateLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        if (m_Enable)
        {
            emit changeState(m_Index, !m_State);
            /*    if (m_State)
                {
                    TurnOff();
                }
                else
                {
                    TurnOn();
                }*/
        }
    }
}

PLCControlBox::PLCControlBox(QString toolboxName, QWidget *parent) : IToolBox(toolboxName, parent)
{
    ui.setupUi(this);
    ui.tableWidget->setColumnCount(3);
    QStringList sHeader;
    sHeader << tr("Name") << tr("ReadOnly") << tr("State");
    ui.tableWidget->setHorizontalHeaderLabels(sHeader);
    // ui.tableWidget->setFixedHeight(130);
    ui.tableWidget->verticalHeader()->setVisible(false);
    QHeaderView *horiHeaders = ui.tableWidget->horizontalHeader();
    // ui.tableWidget->resizeColumnToContents(0);
    // ui.tableWidget->setColumnWidth(0, 150);
    // ui.tableWidget->setColumnWidth(1, 150);
    // ui.tableWidget->setColumnWidth(2, 150);
    // ui.tableWidget->resizeColumnToContents(1);
    // ui.tableWidget->resizeColumnToContents(2);
    /*horiHeaders->setStyleSheet("QHeaderView::section{background:white;color:black;}");*/
    horiHeaders->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    initTableStyle();

    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, this,
        [this]() { m_Model->closeLightBeforeRecipe(); 
        });
}

PLCControlBox::~PLCControlBox()
{
    if(m_Model != nullptr){
        delete m_Model;
        m_Model = nullptr;
    }
}

void PLCControlBox::SetModel(PLCController *model)
{
    m_Model = model;
    QMap<int, PLCCell> cells = m_Model->GetAllPLCCells();
    // QList<int> keyList = cells.keys();

    //int effIndex = 0;
    //QMapIterator<int, PLCCell> iterator(cells);
    //while (iterator.hasNext())
    //{
    //    iterator.next();
    //    PLCCell cell = iterator.value();
    //    if (!cell.visible)
    //        continue;
    //    effIndex++;
    //}
    ui.tableWidget->setRowCount(cells.size());
    //ui.tableWidget->setMinimumHeight(cells.size() * 35);
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui.tableWidget->setFocusPolicy(Qt::NoFocus);
    // ui.tableWidget->setFrameShape(QFrame::NoFrame);


    for (int i = 0; i < cells.size(); i++)
    {
        PLCCell cell = cells.value(i + 1);
        
        QTableWidgetItem *item0 = new QTableWidgetItem();
        item0->setText(cell.name);
        item0->setTextAlignment(Qt::AlignRight);
        item0->setTextAlignment(Qt::AlignVCenter);
        ui.tableWidget->setItem(i, 0, item0);
        

        StateLabel *label = new StateLabel(cell.index);
        QWidget *widget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout;
        layout->setSpacing(0);
        layout->setMargin(0);
        layout->addWidget(label);
        widget->setLayout(layout);
        ui.tableWidget->setCellWidget(i, 2, widget);
        // ui.tableWidget->item(i, 2)->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *item2 = new QTableWidgetItem();
        if (cell.readoniy)
        {
            item2->setText("True");
            label->SetEnable(false);
        }
        else
        {
            item2->setText("False");
            connect(label, SIGNAL(changeState(int, bool)), this, SLOT(changeSatet(int, bool)));
        }
        item2->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setItem(i, 1, item2);

        m_StateLabels.insert(i + 1, label);

        //隐藏暂时用不到的项
        if (!cell.visible)
        {
            ui.tableWidget->hideRow(i);
        }
  
    }

    connect(m_Model, SIGNAL(sendStateToUI(int, int)), this, SLOT(changeStateOnUI(int, int)));
}

void PLCControlBox::initTableStyle()
{
    QString qssTV = "QTableWidget::item:hover{background-color:rgb(92,188,227,200)}"
                    "QTableWidget::item:selected{background-color:#1B89A1}"
                    "QHeaderView::section,QTableCornerButton:section{ \
                     padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
                     border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                     background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                    "QTableWidget{background-color:white;border:none;}";

    ui.tableWidget->horizontalHeader()->setFont(QFont("song", 12));
    QFont font = ui.tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui.tableWidget->horizontalHeader()->setFont(font);

    ui.tableWidget->setStyleSheet(qssTV);
}

void PLCControlBox::changeSatet(int index, bool state)
{
    if (m_Model->IsConnected())
    {
        PLCCell cell = m_Model->m_Cells.value(index);
        if (cell.name == "Projector Down")
        {
            Result res = m_Model->projectorUp(false);
            return;
        }
        else if (cell.name == "Projector Up")
        {
            Result res = m_Model->projectorUp(true);
            return;
        }
        else
        {
            m_Model->SendCommond(index, state);
        }
    }
}

void PLCControlBox::changeStateOnUI(int index, int value)
{
    m_StateLabels.value(index)->ChangeState(value);
}