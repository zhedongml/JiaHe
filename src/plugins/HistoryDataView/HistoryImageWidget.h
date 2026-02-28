#pragma once

#include <QWidget>
#include "ui_HistoryImageWidget.h"
#include "CameraViewer/CameraViewer.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QStandardItem>
#include <HistoryDataView/define.h>
#include <DockManager.h>
#include "DockWidget.h"
#include "tableViewWidget.h"
#include "imageViewWidget.h"
#include "ImageDirectoryWatcher.h"
#include "MLAlgorithm\ml_image_public.h"

using namespace std;
struct OldSenderInfo
{
    QString oldTitle;
    int oldRowInfo;
};
class HistoryImageWidget : public QWidget
{
	Q_OBJECT

public:
	HistoryImageWidget(QWidget *parent = nullptr);
	~HistoryImageWidget();
    void init();
    void initSelectInfo();
    void handleImageData(QString imagePath);
    void startDirectoryWatching(const QString& directory);
    void stopDirectoryWatching();

signals:
   void closeReq();
   void judgeIsRaw(bool isRaw);
   void disableButton(int index);
   void directoryChanged();
 public Q_SLOTS:
    void handleClose();
    void on_btn_load_clicked();
    void on_browseBtn_clicked();

    void imgShowAuto(tableViewWidget* view, const QModelIndex& index);
 private slots: 
    void closeEvent(QCloseEvent *event);
    void imgDirShow(const QModelIndex &index);
    void imgShow(const QModelIndex &index);
    void Men_Slot(QPoint p);
    void showRaw_action();
    void imgClose();
    void setRawFlag(bool isRaw);
    void emitFlagRaw(const QModelIndex &);
    void emitFlagDir(const QModelIndex &);
    void onNewImagesDetected(const QString& directory, const std::vector<std::string>& newImages);

  private:
    void initUi();
    void UpdateTableView(QStringList *headsShowList, std::vector<StringArray> *rowsAllData);
    void updateSecondTableDir(QStringList headsShowList, std::vector<StringArray> *rowsAllData, int row);
    void updateSecondTableDir(QStringList headsShowList, std::vector<StringArray> *rowsAllData);
    QStringList getTxtContent(QString path);
    std::vector<string> getImgDir(QString path, bool isRaw);
    void handleData(std::vector<StringArray> &rowsAllData, QStringList strList, std::vector<string> imglist);
    void dockRemove();
    cv::Mat judgeImgShow(tableViewWidget *view, int index, int row, QString &name,int col);

  private:
	Ui::HistoryImageWidgetClass ui;
    QString m_path;
    QList<ads::CDockWidget *> m_docks;
    ads::CDockManager *m_dockManager; 
    ads::CDockAreaWidget *dockPageManager;
    tableViewWidget *dirView_handle;
    tableViewWidget *dirView_raw;
    QStandardItemModel *standItemModel;
    int m_countFlag = 0;
    bool m_isRaw = false;
    QModelIndex rawIndex;
    QMenu *tableviewMenu; 
    QAction *Action;
    ads::CDockWidget *m_dockTab=nullptr;
    QMap< QString,ads::CDockWidget *> m_imgDocks;
    QMap< ads::CDockWidget *,QString> m_imgSends;
    QMap<int, imageViewWidget*> m_rowImages;
    QMap<imageViewWidget *, OldSenderInfo> m_senderImg;
    QMap<tableViewWidget *, int> m_size;
    QMap<tableViewWidget *, QString> m_paths;
    QVector<QString> m_allImagePaths;
    int m_temRow=0;
    int m_temAdd=0;
    int m_temSub=0;
    imageViewWidget *m_oldSender = nullptr;
    imageViewWidget *m_temAddSender = nullptr;
    imageViewWidget *m_temSubSender = nullptr;
    int m_imgRows;
    int m_raws;
    imageViewWidget *imgWidget;
    QString m_oldName;
    int new_image_size = 0;
    int old_image_size = 0;
    ImageDirectoryWatcher* m_directoryWatcher;
};
