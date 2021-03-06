#ifndef SFTPSERVER_H
#define SFTPSERVER_H

#include <QWidget>
#include <QQueue>
#include <tabpage.h>
#include <QMessageBox>
#include "ssh/sshconnection.h"
#include "ssh/sftpchannel.h"
#include "filetreeview.h"
#include "function.h"
#include "qfiletreeview.h"

namespace Ui {
class SftpServer;
}

typedef struct
{
    QStandardItem *it;
    QString strPath;
}QueueItem;

class SftpServer : public QWidget
{
    Q_OBJECT

public:
    int serverNum;
    QString m_shellPath;

    SftpServer(QWidget *parent = 0);
    explicit SftpServer(const QSsh::SshConnectionParameters &parameters, int serverNumber = 0, QWidget *parent = 0, TabPage *p = 0);
    ~SftpServer();

    void initPage();
    void initTreeView();

public slots:
    void handleConnected();
    void handleError();
    void handleDisconnected();
    void handleChannelInitialized();
    void handleChannelInitializationFailed(const QString &renson);
    void handleJobFinished(QSsh::SftpJobId id, const QString &error);
    void handleFileInfo(QSsh::SftpJobId id, const QList<QSsh::SftpFileInfo> &fileInfoList);
    void handleChannelClosed();
    void getNextLevelList(QString path);

    void handleOpenFileWidgetClicked();
    void handleOpenClicked(const QString &fileName, const QString &fileType);
    void handleUpClicked();
    void handleHomeClicked();


    void handleDownloadClicked(const QString &fileName, const QString &fileType);


private:
    Ui::SftpServer *ui;
    
    QSsh::SshConnection *m_connection;
    QSsh::SftpChannel::Ptr m_channel;
    
    QString m_currentPath;

    enum JobType{
        JobUnknow, JobStatFile, JobListDir, JobCreateDir, JobRemoveDir, JobRemoveFile, JobRename,
        JobCreateFile, JobCreateLink, JobUploadFile, JobDownloadFile, JobUploadDir, JobDownloadDir
    };
    JobType m_jobType;
    QSsh::SftpJobId m_jobListDirId;
    QSsh::SftpJobId m_jobUploadId;
    
//    FileTreeView *fileTree;
    QFileTreeView *m_treeView;
    TabPage *page;

    enum WorkWidget{
        WorkFileWidget, WorkFileTreeView
    };
    WorkWidget m_workWidget;

    QStandardItemModel *m_treeItemModel;
    QStandardItem *m_currentItem;
    QStandardItem *item;
    QQueue<QueueItem *> items;
};

#endif // SFTPSERVER_H
