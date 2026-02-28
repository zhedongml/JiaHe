#include "HistoryImageWidget.h"
#include <QTextStream>
#include <HistoryDataView/TableViewDataManage.h>
#include <QFile>
#include <io.h>
#include <QtGui/qquaternion.h>
#include <PrjCommon/metricsdata.h>
#include <QtWidgets/qfiledialog.h>
#include <QCalendarWidget>
#include <QThread>

HistoryImageWidget::HistoryImageWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    initUi();
    setWindowIcon(QIcon(":/image/image/mlico.ico"));
    m_directoryWatcher = new ImageDirectoryWatcher(this);
    connect(m_directoryWatcher, &ImageDirectoryWatcher::newImagesDetected, this, &HistoryImageWidget::onNewImagesDetected);
    connect(this, &HistoryImageWidget::directoryChanged, m_directoryWatcher, &ImageDirectoryWatcher::checkForNewImages);

}

HistoryImageWidget::~HistoryImageWidget()
{
    stopDirectoryWatching();
}

void HistoryImageWidget::imgDirShow(const QModelIndex &index)
{
    m_isRaw = false;
    std::vector<StringArray> rowsAllData = {};
    int row = index.row();
    if (m_imgDocks.contains("Image List" + QString::number(row + 1)))
    {
        m_imgDocks.value("Image List" + QString::number(row + 1))->setAsCurrentTab();
    }
    else
    {
    QString path = ui.tableView->model()->index(row, 2).data().toString() ;
    m_path = path+ "\\";
    QStringList buf = getTxtContent(m_path);
    std::vector<string> imglist = getImgDir(m_path, false);
    m_imgRows = imglist.size();
    QStringList headsShowList = {"Name","ND", "Color", "ExposureTime"};
    handleData(rowsAllData,buf,imglist);
    updateSecondTableDir(headsShowList,&rowsAllData,row);
    }
}

void HistoryImageWidget::imgShow(const QModelIndex &index)
{
    QStringList imgInfo = {};
    m_temRow = index.row();
    tableViewWidget* view = qobject_cast<tableViewWidget *>(sender());
    int col=view->getModel()->columnCount();
    for (int i= 0; i < col; i++)
    {
        imgInfo.append(view->getModel()->index(m_temRow, i).data().toString());
    }
    //if (m_imgDocks.contains(imgInfo[0]))
    //{
    //    m_imgDocks.value(imgInfo[0])->setAsCurrentTab();
    //    return;
    //}
    m_oldName = imgInfo[0];
    QString imgName;
    cv::Mat showImg = judgeImgShow(view, 0, m_temRow, imgName, col);
    imageViewWidget *imgWidget =new imageViewWidget;
  
    imgWidget->showImg(showImg);
    m_rowImages.insert(m_temRow, imgWidget);
    ads::CDockWidget *dockTab = new ads::CDockWidget(imgName);
     dockTab->setWidget(imgWidget);
     m_imgDocks.insert(imgName, dockTab);
     m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dockTab);
     m_docks.append(dockTab);
     if (m_rowImages.contains(m_temRow))
     {
         if (m_temRow == 0)
             m_rowImages.value(m_temRow)->setDisenable(0);
         else if (m_temRow == (m_size.value(view) - 1))
                 m_rowImages.value(m_temRow)->setDisenable(1);
     }
     m_senderImg.insert(imgWidget, {m_oldName, m_temRow});

     bool b = connect(dockTab, SIGNAL(closed()), this, SLOT(handleClose()));
     bool d = connect(dockTab, SIGNAL(closed()), this, SLOT(imgClose()));
     bool w = connect(imgWidget, &imageViewWidget::switchNextImg, this, [=]() {
         QString a;
         OldSenderInfo oldInfo;
         imageViewWidget *send = qobject_cast<imageViewWidget *>(sender());
         int row = m_rowImages.key(send);
         cv::Mat img;
         if (m_senderImg.contains(send))
         {
             row = m_senderImg.value(send).oldRowInfo;

             if ((m_oldSender != send) && (!m_senderImg.contains(m_oldSender)))
             {
                 int oldRow = m_rowImages.key(m_oldSender);
                 oldInfo.oldRowInfo = oldRow + m_temAdd - m_temSub;
                 oldInfo.oldTitle = m_oldName;
                 m_senderImg.insert(m_oldSender, oldInfo);
             }
             m_temAdd = 0;
             m_temSub = 0;
         }
         else
         {
             if ((m_oldSender != send) && (m_oldSender != nullptr))
             {
                 int oldRow = m_rowImages.key(m_oldSender);
                 m_senderImg.insert(m_oldSender, {m_oldName, oldRow + m_temAdd - m_temSub});
                 m_temAdd = 0;
                 m_temSub = 0;
             }
             if (m_temAddSender == nullptr)
                 m_temAdd = 0;
         }
         //if (m_imgDocks.contains(m_senderImg.value(send).oldTitle))
         //{
         //    //m_docks.removeOne(m_imgDocks.value(m_senderImg.value(send).oldTitle));
         //    //m_imgDocks.remove(m_senderImg.value(send).oldTitle);
         //}
         int newrow = row + m_temAdd - m_temSub;
         if (newrow >= m_size.value(view) - 2)
         {
             send->setDisenable(1);
             img = judgeImgShow(view, 1, m_size.value(view) - 2, a, col);
         }
         else
         {
             send->setDisenable(-1);
             img = judgeImgShow(view, 1, row + m_temAdd - m_temSub, a, col);
         }
         if (m_senderImg.contains(send))
         {
             m_senderImg.insert(send, {a, row + m_temAdd - m_temSub + 1});
         }
         m_temAdd++;
         m_temAddSender = send;
         m_oldSender = send;
         //m_imgDocks.insert(a, dockTab);
         m_oldName = a;
         imgWidget->showImg(img);
         dockTab->setWindowTitle(a);
         //m_docks.append(dockTab);
     });
     bool w3 = connect(imgWidget, &imageViewWidget::switchLastImg, this, [=]() {
         QString name2;
         imageViewWidget *send = qobject_cast<imageViewWidget *>(sender());
         int row = m_rowImages.key(send);
         cv::Mat img2;
         if (m_senderImg.contains(send))
         {
             row = m_senderImg.value(send).oldRowInfo;

             if ((m_oldSender != send) && (!m_senderImg.contains(m_oldSender)))
             {
                 int oldRow = m_rowImages.key(m_oldSender);
                 m_senderImg.insert(m_oldSender, {m_oldName, oldRow + m_temAdd - m_temSub});
             }
             m_temAdd = 0;
             m_temSub = 0;
         }
         else
         {
             if ((m_oldSender != send) && (m_oldSender != nullptr))
             {
                 int oldRow = m_rowImages.key(m_oldSender);
                 m_senderImg.insert(m_oldSender, {m_oldName, oldRow - m_temSub + m_temAdd});
                 m_temSub = 0;
                 m_temAdd = 0;
             }
             if (m_temSubSender == nullptr)
                 m_temSub = 0;
         }
         //if (m_imgDocks.contains(m_senderImg.value(send).oldTitle))
         //{
         //    m_docks.removeOne(m_imgDocks.value(m_senderImg.value(send).oldTitle));
         //    m_imgDocks.remove(m_senderImg.value(send).oldTitle);
         //}
         int newrow = row - m_temSub + m_temAdd;
         if (newrow <= 1)
         {
             send->setDisenable(0);
             img2 = judgeImgShow(view, -1, 1, name2, col);
         }
         else
         {
             send->setDisenable(-1);
             img2 = judgeImgShow(view, -1, row - m_temSub + m_temAdd, name2, col);
         }
         if (m_senderImg.contains(send))
         {
             m_senderImg.insert(send, {name2, row - m_temSub + m_temAdd - 1});
         }
         m_temSub++;

         m_temSubSender = send;
         m_oldSender = send;
         // m_imgDocks.remove(imgInfo[0]);
         //m_imgDocks.insert(name2, dockTab);
         m_oldName = name2;
         imgWidget->showImg(img2);
         dockTab->setWindowTitle(name2);
         //m_docks.append(dockTab);
     });
}

void HistoryImageWidget::imgShowAuto(tableViewWidget* view, const QModelIndex& index)
{
    QStringList imgInfo = {};
    m_temRow = index.row();
    int col = view->getModel()->columnCount();
    for (int i = 0; i < col; i++)
    {
        imgInfo.append(view->getModel()->index(m_temRow, i).data().toString());
    }

    m_oldName = imgInfo[0];
    QString imgName;
    cv::Mat showImg = judgeImgShow(view, 0, m_temRow, imgName, col);
    
    // 检查是否有打开的图片查看窗口
    bool hasImageViewWindow = false;
    for (auto dock : m_docks) {
        imageViewWidget* widget = qobject_cast<imageViewWidget*>(dock->widget());
        if (widget) {
            hasImageViewWindow = true;
            // 更新当前窗口的图片
            widget->showImg(showImg);
            dock->setWindowTitle(imgName);
            dock->setAsCurrentTab();
            
            // 更新相关映射
            m_rowImages[m_temRow] = widget;
            m_senderImg[widget] = { m_oldName, m_temRow };
            m_imgDocks[imgName] = dock;
            break;
        }
    }
    
    if (!hasImageViewWindow) {
        // 如果没有打开的图片查看窗口，创建新窗口
        imageViewWidget* imgWidget = new imageViewWidget;

        m_rowImages.insert(m_temRow, imgWidget);
        ads::CDockWidget* dockTab = new ads::CDockWidget(imgName);
        dockTab->setWidget(imgWidget);
        m_imgDocks.insert(imgName, dockTab);
        m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dockTab);
        m_docks.append(dockTab);
        dockTab->show();
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        imgWidget->showImg(showImg);
        dockTab->setWindowTitle(imgName);
        if (m_rowImages.contains(m_temRow))
        {
            if (m_temRow == 0)
                m_rowImages.value(m_temRow)->setDisenable(0);
            else if (m_temRow == (m_size.value(view) - 1))
                m_rowImages.value(m_temRow)->setDisenable(1);
        }
        m_senderImg.insert(imgWidget, { m_oldName, m_temRow });
        
        // 连接关闭信号
        connect(dockTab, SIGNAL(closed()), this, SLOT(handleClose()));
        connect(dockTab, SIGNAL(closed()), this, SLOT(imgClose()));
        connect(imgWidget, &imageViewWidget::switchNextImg, this, [=]() {
            QString a;
            OldSenderInfo oldInfo;
            imageViewWidget* send = qobject_cast<imageViewWidget*>(sender());
            int row = m_rowImages.key(send);
            cv::Mat img;
            if (m_senderImg.contains(send))
            {
                row = m_senderImg.value(send).oldRowInfo;

                if ((m_oldSender != send) && (!m_senderImg.contains(m_oldSender)))
                {
                    int oldRow = m_rowImages.key(m_oldSender);
                    oldInfo.oldRowInfo = oldRow + m_temAdd - m_temSub;
                    oldInfo.oldTitle = m_oldName;
                    m_senderImg.insert(m_oldSender, oldInfo);
                }
                m_temAdd = 0;
                m_temSub = 0;
            }
            else
            {
                if ((m_oldSender != send) && (m_oldSender != nullptr))
                {
                    int oldRow = m_rowImages.key(m_oldSender);
                    m_senderImg.insert(m_oldSender, { m_oldName, oldRow + m_temAdd - m_temSub });
                    m_temAdd = 0;
                    m_temSub = 0;
                }
                if (m_temAddSender == nullptr)
                    m_temAdd = 0;
            }
            int newrow = row + m_temAdd - m_temSub;
            if (newrow >= m_size.value(view) - 2)
            {
                send->setDisenable(1);
                img = judgeImgShow(view, 1, m_size.value(view) - 2, a, col);
            }
            else
            {
                send->setDisenable(-1);
                img = judgeImgShow(view, 1, row + m_temAdd - m_temSub, a, col);
            }
            if (m_senderImg.contains(send))
            {
                m_senderImg.insert(send, { a, row + m_temAdd - m_temSub + 1 });
            }
            m_temAdd++;
            m_temAddSender = send;
            m_oldSender = send;
            m_oldName = a;
            imgWidget->showImg(img);
            dockTab->setWindowTitle(a);
            //m_docks.append(dockTab);
        });
        connect(imgWidget, &imageViewWidget::switchLastImg, this, [=]() {
            QString name2;
            imageViewWidget* send = qobject_cast<imageViewWidget*>(sender());
            int row = m_rowImages.key(send);
            cv::Mat img2;
            if (m_senderImg.contains(send))
            {
                row = m_senderImg.value(send).oldRowInfo;

                if ((m_oldSender != send) && (!m_senderImg.contains(m_oldSender)))
                {
                    int oldRow = m_rowImages.key(m_oldSender);
                    m_senderImg.insert(m_oldSender, { m_oldName, oldRow + m_temAdd - m_temSub });
                }
                m_temAdd = 0;
                m_temSub = 0;
            }
            else
            {
                if ((m_oldSender != send) && (m_oldSender != nullptr))
                {
                    int oldRow = m_rowImages.key(m_oldSender);
                    m_senderImg.insert(m_oldSender, { m_oldName, oldRow - m_temSub + m_temAdd });
                    m_temSub = 0;
                    m_temAdd = 0;
                }
                if (m_temSubSender == nullptr)
                    m_temSub = 0;
            }
            int newrow = row - m_temSub + m_temAdd;
            if (newrow <= 1)
            {
                send->setDisenable(0);
                img2 = judgeImgShow(view, -1, 1, name2, col);
            }
            else
            {
                send->setDisenable(-1);
                img2 = judgeImgShow(view, -1, row - m_temSub + m_temAdd, name2, col);
            }
            if (m_senderImg.contains(send))
            {
                m_senderImg.insert(send, { name2, row - m_temSub + m_temAdd - 1 });
            }
            m_temSub++;

            m_temSubSender = send;
            m_oldSender = send;
            m_oldName = name2;
            imgWidget->showImg(img2);
            dockTab->setWindowTitle(name2);
            //m_docks.append(dockTab);
        });


    }
    // 处理事件，确保图片显示
    QApplication::processEvents();
}

void HistoryImageWidget::Men_Slot(QPoint p)
{

    rawIndex = ui.tableView->indexAt(p); 
    int row = rawIndex.row();
    QString path = ui.tableView->model()->index(row, 2).data().toString() + "\\";
    m_path = path;
    if (rawIndex.isValid())                       
    {
        QItemSelectionModel *selections = ui.tableView->selectionModel(); 
        QModelIndexList selected = selections->selectedIndexes(); 
        if (selected.count() == 1)                                
        {
            Action->setVisible(true);
        }
        tableviewMenu->exec(QCursor::pos()); 
    }
}

void HistoryImageWidget::showRaw_action()
{
    m_isRaw = true;
    std::vector<StringArray> rowsAllData = {};
    int row = rawIndex.row();
    if (m_imgDocks.contains("Raw Image List"+QString::number(row+1)))
    {
        m_imgDocks.value("Raw Image List" + QString::number(row + 1))->setAsCurrentTab();
    }
    else
    {
        QString path = ui.tableView->model()->index(row, 2).data().toString() + "\\";
        std::vector<string> rawData = getImgDir(path,true);
        QStringList headsShowList = {"Name", "ND", "Color", "ExposureTime"};
        QStringList buf = getTxtContent(path);//get txt
        handleData(rowsAllData, buf, rawData);
        m_raws = rowsAllData.size();
        dirView_raw = new tableViewWidget;
        dirView_raw->updateSecondTableDir(headsShowList, &rowsAllData, headsShowList.size());
        ads::CDockWidget *dockTab = new ads::CDockWidget("Raw Image List" + QString::number(row + 1));
        m_imgDocks.insert("Raw Image List" + QString::number(row + 1), dockTab);
        m_size.insert(dirView_raw, m_raws);
        m_paths.insert(dirView_raw, m_path);
        dockTab->setWidget(dirView_raw);
        m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dockTab);
        m_docks.append(dockTab);
        m_imgDocks.insert("Raw Image List" + QString::number(row + 1), dockTab);
        bool b = connect(dockTab, SIGNAL(close()), this, SLOT(handleClose()));
        bool d = connect(dockTab, SIGNAL(closed()), this, SLOT(imgClose()));
        bool v2 = connect(dirView_raw, SIGNAL(doubleSecond(const QModelIndex &)), SLOT(emitFlagRaw(const QModelIndex &)));
        bool v = connect(dirView_raw, SIGNAL(doubleSecond(const QModelIndex &)), SLOT(imgShow(const QModelIndex &)));

    }

}
void HistoryImageWidget::handleClose()
{
    ads::CDockWidget *dockTab = qobject_cast<ads::CDockWidget *>(sender());
   int index= m_docks.indexOf(dockTab);

    m_docks.removeOne(dockTab);
   m_dockManager->removeDockWidget(dockTab);
   int i = 0;
   if (index!=0)
        m_docks[index-1-i]->setAsCurrentTab();
}

void HistoryImageWidget::init()
{
    //m_raws.clear();
    //m_dockManager->show();
    QDateTime timer = QDateTime::currentDateTime();
    QDateTime timerStart = timer.addDays(-1);
    QDateTime timerBegin = timer.addDays(-366);
    QDateTime timerEnd = timer.addDays(366);
    ui.dateTimeStart->setDateTime(timerStart);
    ui.dateTimeEnd->setDateTime(timer);
    ui.dateTimeStart->setMinimumDateTime(timerBegin);
    ui.dateTimeEnd->setMaximumDateTime(timerEnd);
    ui.dateTimeStart->setMaximumDateTime(timer);
    on_btn_load_clicked();
    ui.tableView->hide();
    ui.widget_query->hide();
    
    
}

void HistoryImageWidget::initUi()
{
    ui.label_7->hide();
    ui.comboEyeBox->hide();
    QCalendarWidget *CalendarStart = new QCalendarWidget();
    QCalendarWidget *CalendarEnd = new QCalendarWidget();

    ui.dateTimeStart->setCalendarPopup(true); 
    CalendarStart->setFixedSize(350, 300);
    ui.dateTimeStart->setCalendarWidget(CalendarStart);
    ui.dateTimeStart->setDate(QDate::currentDate());
    ui.dateTimeStart->setDisplayFormat("yyyy-MM-dd HH:mm:ss");

    ui.dateTimeEnd->setCalendarPopup(true); 
    CalendarEnd->setFixedSize(350, 300);
    ui.dateTimeEnd->setCalendarWidget(CalendarEnd);
    ui.dateTimeEnd->setDate(QDate::currentDate());
    ui.dateTimeEnd->setDisplayFormat("yyyy-MM-dd HH:mm:ss");

    m_dockManager = new ads::CDockManager(ui.widget_3);
    m_dockManager->setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    m_dockManager->setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    m_dockManager->setMinimumWidth(820);
    m_dockManager->setMinimumHeight(ui.tableView->height());
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(imgDirShow(const QModelIndex &)));
    ui.verticalLayout_3->addWidget(m_dockManager, 1);
    //m_dockManager->hide();
    ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableviewMenu = new QMenu(ui.tableView);
    Action = new QAction("Raw Image Load", ui.tableView);
    tableviewMenu->addAction(Action);
    bool t = connect(Action, SIGNAL(triggered()), this, SLOT(showRaw_action()));
    connect(ui.tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(Men_Slot(QPoint)));

    QIcon icon;
    icon.addFile(QString::fromUtf8(":/image/image/files.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui.browseBtn->setIconSize(QSize(30, 30));
    ui.browseBtn->setIcon(icon);
    ui.browseBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui.browseBtn->setAutoRaise(true);
    ui.browseBtn->setEnabled(true);
    ui.browseBtn->setToolTip("Browse Files");
}

void HistoryImageWidget::UpdateTableView(QStringList *headsShowList, std::vector<StringArray> *rowsAllData)
{
    standItemModel = new QStandardItemModel();
    standItemModel->setColumnCount(headsShowList->size());
    int p = headsShowList->size();
    for (int i = 0; i < headsShowList->size(); i++)
    {
        QString headStr = headsShowList->at(i);
        standItemModel->setHeaderData(i, Qt::Horizontal, headStr);

        for (int j = 0; j < rowsAllData->size(); j++)
        {
            QStandardItem *standItem1 = new QStandardItem(QString::fromStdString(rowsAllData->at(j).at(i)));
            standItemModel->setItem(j, i, standItem1);
        }
    }
    ui.tableView->setModel(standItemModel);
    m_dockManager->show();
    ui.tableView->show();
    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); 
    ui.tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); 
    if (!m_dockTab)
    {
        m_dockTab= new ads::CDockWidget("Information");
        m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, m_dockTab);
    }
    m_dockTab->setWidget(ui.tableView);
    m_imgDocks.insert(m_dockTab->windowTitle(), m_dockTab);
    m_docks.append(m_dockTab);
    ui.tableView->horizontalHeader()->show();
    m_dockTab->setAsCurrentTab();
    m_countFlag++;
}

void HistoryImageWidget::updateSecondTableDir(QStringList headsShowList, std::vector<StringArray> *rowsAllData,int row)
{
    m_dockManager->show();
    ads::CDockWidget *dockTab = new ads::CDockWidget("Image List"+QString::number(row+1));
    dirView_handle = new tableViewWidget;
    dirView_handle->updateSecondTableDir(headsShowList, rowsAllData,4);
    dockTab->setWidget(dirView_handle);
    m_imgDocks.insert(dockTab->windowTitle(), dockTab);
    m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dockTab);
    m_size.insert(dirView_handle, m_imgRows);
    m_paths.insert(dirView_handle, m_path);
    bool v = connect(dirView_handle, SIGNAL(doubleSecond(const QModelIndex &)), SLOT(emitFlagDir(const QModelIndex &)));
    bool qv = connect(dirView_handle, SIGNAL(doubleSecond(const QModelIndex &)), SLOT(imgShow(const QModelIndex &)));

    m_docks.append(dockTab);
    bool b = connect(dockTab, SIGNAL(closed()), this, SLOT(handleClose()));
    bool d = connect(dockTab, SIGNAL(closed()), this, SLOT(imgClose()));
}

void HistoryImageWidget::updateSecondTableDir(QStringList headsShowList, std::vector<StringArray> *rowsAllData)
{
   m_dockManager->show();
    ads::CDockWidget *dockTab = new ads::CDockWidget("Image List");//////local brouse
    dirView_handle = new tableViewWidget;
    dirView_handle->updateSecondTableDir(headsShowList, rowsAllData, 4);
    dockTab->setWidget(dirView_handle);
    //m_switchTab = dockTab;
    m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dockTab);
    m_imgDocks.insert( "Image List",dockTab);
    m_size.insert(dirView_handle, m_imgRows);
    m_paths.insert(dirView_handle, m_path);
    bool v = connect(dirView_handle, SIGNAL(doubleSecond(const QModelIndex &)), SLOT(emitFlagDir(const QModelIndex &)));
    bool qv = connect(dirView_handle, SIGNAL(doubleSecond(const QModelIndex &)), SLOT(imgShow(const QModelIndex &)));
    m_docks.append(dockTab);
    bool b = connect(dockTab, SIGNAL(closed()), this, SLOT(handleClose()));
    bool d = connect(dockTab, SIGNAL(closed()), this, SLOT(imgClose()));
}

QStringList HistoryImageWidget::getTxtContent(QString path)
{
    QString strAll;
    QStringList strList; 
    QString txtPath = path + "info.txt";
    QFile file(txtPath); 
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        strAll = stream.readAll();
    }
    file.close();
    strList=strAll.split("\n");
    QStringList list;
    strList.removeLast();
    for (auto str : strList)
    {
        if (str.contains("_"))
            list.append(str);
    }
    return list;
}

std::vector<string> HistoryImageWidget::getImgDir(QString path,bool isRaw)
{
    QString dir = path;
    path.replace("\\", "/");
    if (isRaw)
        dir = path + "/raw";
    std::string temp_imgname;
    intptr_t hFile = 0;   
    _finddata_t fileinfo; 
    std::vector<string> imglist;
    string search = dir.toStdString();
    const char *to_search = dir.toStdString().c_str();
    string p;
    if ((hFile = _findfirst(p.assign(search).append("/*.tif").c_str(), &fileinfo)) != -1) 
    {
        do
        {
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                continue; 
            }
            else
            {
                imglist.push_back(fileinfo.name);
            }
        } while (_findnext(hFile, &fileinfo) == 0);

        _findclose(hFile);
    }
    return imglist;
}

void HistoryImageWidget::handleData(std::vector<StringArray> &rowsAllData, QStringList strList,std::vector<string> imglist)
{
    StringArray rowValue;
    StringArray rows;
    QString infoName;
    QString imgName;
    QStringList showValues;
    QMap<string, StringArray> imgs;

    for (auto str : strList)
    {
        QStringList lineStrs = str.split(";");
        string key = lineStrs[0].split(":")[1].toStdString();
        for (auto item : lineStrs)
        {
            QStringList items = item.split(":");
            rowValue.push_back(items[1].toStdString());
        }
        auto iter=rowValue.begin();
        iter=rowValue.erase(iter);
        imgs.insert(key, rowValue);
        rowValue.clear();
    }
    for (auto img : imglist)
    {
        infoName = img.c_str();
        if (infoName.contains("-"))
        {
            int index = infoName.indexOf("-");
            imgName = infoName.mid(0, index);
        }
        else
        {
            int index = infoName.lastIndexOf("_");
            imgName = infoName.mid(0, index);
        }
        if (imgs.contains(imgName.toStdString()))
        {
            StringArray key_value = imgs.value(imgName.toStdString());
            rows.push_back(infoName.toStdString());
            for (auto value : key_value)
            {
                  rows.push_back(value);
            }
        }
        else
        {
            rows.push_back(infoName.toStdString());
            rows.push_back("");
            rows.push_back("");
            rows.push_back("");
        }
        rowsAllData.push_back(rows);
        rows.clear();
    }
}

void HistoryImageWidget::dockRemove()
{
    //m_dataDocks.clear();
    for (int i = m_docks.size()-1; i >=0 ; i--)
    {
        QString a=m_docks[i]->windowTitle();
        m_dockManager->removeDockWidget(m_docks[i]);
        imageViewWidget *imgWdg = (imageViewWidget *)(m_docks[i]->widget());
        m_senderImg.remove(imgWdg);
        m_docks.removeOne(m_imgDocks.value(a));  
        m_imgDocks.remove(a);
    }
    m_dockTab = nullptr;
}

cv::Mat HistoryImageWidget::judgeImgShow(tableViewWidget *view, int index, int row, QString &imgName,int col)
{
    int newRow = row + index;
    QStringList imgInfo = {};
    for (int i = 0; i < col; i++)
    {
        imgInfo.append(view->getModel()->index(newRow, i).data().toString());
    }
    //QString imgName;
    if (!m_isRaw)
        imgName = imgInfo[0];
    else
        imgName = "raw\\" + imgInfo[0] ;
    QString imgPath = m_paths.value(view) + imgName;
    imgPath.replace("\\", "//");
    cv::Mat showImg = cv::imread(imgPath.toStdString().c_str(), cv::IMREAD_UNCHANGED);
    return showImg;
}

void HistoryImageWidget::initSelectInfo()
{
    ui.comboDutSN->clear();
    ui.comboEyeBox->clear();
    DutInfoShowArray m_dutInfoShowArray;
    m_dutInfoShowArray.clear();
    TableViewDataManage::GetImgDutShowData(m_dutInfoShowArray);
    for (int m = 0; m < m_dutInfoShowArray.size(); m++)
    {
        QString a = m_dutInfoShowArray.at(m).dutSN.c_str();
        ui.comboDutSN->addItem(m_dutInfoShowArray.at(m).dutSN.c_str());
        for (int n = 0; n < m_dutInfoShowArray.at(m).eyeboxs.size(); n++)
        {
            ui.comboEyeBox->addItem(m_dutInfoShowArray.at(m).eyeboxs[n].c_str());
        }
    }
}



void HistoryImageWidget::closeEvent(QCloseEvent *event)
{
    dockRemove();
    m_imgDocks.clear();
}

void HistoryImageWidget::imgClose()
{
    ads::CDockWidget *dockTab = qobject_cast<ads::CDockWidget *>(sender());
    QString index = m_imgDocks.key(dockTab);
    m_imgDocks.remove(index);
    imageViewWidget *imgWdg = (imageViewWidget*)(dockTab->widget());
    m_senderImg.remove(imgWdg);
}

void HistoryImageWidget::setRawFlag(bool isRaw)
{
    m_isRaw=isRaw;
}

void HistoryImageWidget::emitFlagRaw(const QModelIndex &index)
{
    m_isRaw = true;
    int row = index.row();
}

void HistoryImageWidget::emitFlagDir(const QModelIndex &index)
{
    m_isRaw = false;
    int row = index.row();
}

void HistoryImageWidget::on_btn_load_clicked()
{
    //initSelectInfo();
    //init();
    dockRemove();

    QString dutSn = ui.comboDutSN->currentText();
    QString eyebox = ui.comboEyeBox->currentText();
    QString startTimeStr;
    QString endTimeStr;
    if (dutSn == "" )
    {
        return;
    }
    QDateTime startTime = ui.dateTimeStart->dateTime();
    QDateTime endTime = ui.dateTimeEnd->dateTime();
    startTimeStr = startTime.toString("yyyy-MM-dd hh:mm:ss");
    endTimeStr = endTime.toString("yyyy-MM-dd hh:mm:ss");
    //if (m_dataDocks.contains(-1))
    //{
    //    m_dataDocks.value(-1)->setAsCurrentTab();
    //}
    //else
    //{
        QStringList headsShowList;
        std::vector<StringArray> rowsAllData;
        headsShowList.clear();
        rowsAllData.clear();
	    TableViewDataManage::GetMetricTableImgShowDir(dutSn, eyebox, headsShowList, rowsAllData, startTimeStr, endTimeStr);
        //m_dataDocks.insert(-1, m_dockTab);
        UpdateTableView(&headsShowList, &rowsAllData);
}

void HistoryImageWidget::on_browseBtn_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    if (selectDir.isEmpty())
        return;
    handleImageData(selectDir);
}
void HistoryImageWidget::handleImageData(QString imagePath)
{
    dockRemove();
    QStringList headsShowList = { "Name", "ND", "Color", "ExposureTime" };
    std::vector<StringArray> rowsAllData = {};
    if (imagePath != "")
    {
        ui.imageShowPath->setText(imagePath);
        std::vector<string> imgs = getImgDir(imagePath, false);
        m_path = imagePath + "/";
        m_imgRows = imgs.size();
        QStringList buf = getTxtContent(imagePath + "\\");
        handleData(rowsAllData, buf, imgs);
        startDirectoryWatching(imagePath);
    }
    updateSecondTableDir(headsShowList, &rowsAllData);
}

void HistoryImageWidget::startDirectoryWatching(const QString& directory)
{
    if (m_directoryWatcher) {
        m_directoryWatcher->startWatching(directory);
    }
}

void HistoryImageWidget::stopDirectoryWatching()
{
    if (m_directoryWatcher) {
        m_directoryWatcher->stopWatching();
    }
}

void HistoryImageWidget::onNewImagesDetected(const QString& directory, const std::vector<std::string>& newImages)
{
    std::vector<string> allImages = m_directoryWatcher->getCurrentImages();
    
    // 使用新图像更新界面
    std::vector<StringArray> rowsAllData = {};
    QStringList buf = getTxtContent(directory + "\\");
    QStringList headsShowList = { "Name", "ND", "Color", "ExposureTime" };
    handleData(rowsAllData, buf, allImages);
    //dirView_handle = m_paths.key(directory + "/");

    if (dirView_handle) {
        dirView_handle->updateSecondTableDir(headsShowList, &rowsAllData, 4);
        
        // 更新图像计数
        m_imgRows = allImages.size();
        if (m_size.contains(dirView_handle)) {
            m_size[dirView_handle] = m_imgRows;
        }

        if (!newImages.empty()) {
            for (int row = 0; row < rowsAllData.size(); ++row) {
                for (const auto& newImg : newImages) {
                    if (rowsAllData[row][0] == newImg) {
                        // 创建并显示新图片
                        QModelIndex index = dirView_handle->getModel()->index(row, 0);
                        imgShowAuto(dirView_handle, index);
                        QApplication::processEvents();
                        QThread::msleep(1000);
                        break;
                    }
                }
            }
        }
        else
        {
            for (auto dock : m_docks) {
                imageViewWidget* widget = qobject_cast<imageViewWidget*>(dock->widget());
                if (widget) {
                    m_senderImg.remove(widget);
                    m_dockManager->removeDockWidget(dock);
                    m_docks.removeOne(dock);
                }
            }

        }
    }
}



