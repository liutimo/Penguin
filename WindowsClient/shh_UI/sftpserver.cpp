#include "sftpserver.h"
#include "ui_sftpserver.h"

QString homePath = "/root/";

SftpServer::SftpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SftpServer)
{
    ui->setupUi(this);
}

SftpServer::SftpServer(const QSsh::SshConnectionParameters &parameters, int serverNumber, QWidget *parent, TabPage *p)
    : QWidget(parent), ui(new Ui::SftpServer),
      m_connection(new QSsh::SshConnection(parameters)), serverNum(serverNumber), page(p),
      m_currentPath(homePath), m_shellPath(homePath), m_jobType(JobUnknow)
{
    ui->setupUi(this);
    page = p;
    qDebug() << page;

    initTreeView();
    initPage();

    connect(m_connection, SIGNAL(connected()), this, SLOT(handleConnected()));
    connect(m_connection, SIGNAL(error(QSsh::SshError)), this, SLOT(handleError()));
    connect(m_connection, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));

    m_connection->connectToHost();
}

SftpServer::~SftpServer()
{
    delete ui;
}

void SftpServer::handleConnected()
{
    qDebug() << "Create Channel";
    qDebug() << "Connection opened:" << m_connection->connectionParameters().host
             << ":" << m_connection->connectionParameters().port;

    // Create Sftp Channel and initialized
    m_channel = m_connection->createSftpChannel();

    connect(m_channel.data(), SIGNAL(initialized()),
            this, SLOT(handleChannelInitialized()));
    connect(m_channel.data(), SIGNAL(initializationFailed(QString)),
            this, SLOT(handleChannelInitializationFailed(QString)));
    connect(m_channel.data(), SIGNAL(finished(QSsh::SftpJobId, QString)),
            this, SLOT(handleJobFinished(QSsh::SftpJobId, QString)), Qt::DirectConnection);
    connect(m_channel.data(), SIGNAL(fileInfoAvailable(QSsh::SftpJobId,QList<QSsh::SftpFileInfo>)),
            this, SLOT(handleFileInfo(QSsh::SftpJobId, QList<QSsh::SftpFileInfo>)));
    connect(m_channel.data(), SIGNAL(closed()), this, SLOT(handleChannelClosed()));

    m_channel->initialize();
}

void SftpServer::handleError()
{

}

void SftpServer::handleDisconnected()
{

}

void SftpServer::handleChannelInitialized()
{
    qDebug() << "Initialize channel success!";
    
    // list dir
    m_jobType = JobListDir;
    m_workWidget = WorkFileTreeView;
    m_jobListDirId = m_channel->listDirectory(m_currentPath);
}

void SftpServer::handleChannelInitializationFailed(const QString &renson)
{

}

void SftpServer::handleJobFinished(QSsh::SftpJobId id, const QString &error)
{
    qDebug() << "handleJobFinished" << "error" << error;
    qDebug() << "finish : " << m_currentPath;
    if(m_workWidget == WorkFileTreeView)
    {
        if(!items.isEmpty())
        {
            QueueItem *i = items.front();
            m_currentItem = items.front()->it;
            m_currentPath = items.front()->strPath;
            items.pop_front();
            delete i;
            m_jobType = JobListDir;
            qDebug() << "currentItem : " << m_currentItem->text() << "currentPath : " << m_currentPath;
            m_jobListDirId = m_channel->listDirectory(m_currentPath);
        }
    }
    if(m_workWidget == WorkFileWidget)
    {
        return;
    }
}

void SftpServer::handleFileInfo(QSsh::SftpJobId id, const QList<QSsh::SftpFileInfo> &fileInfoList)
{
    qDebug() << "handleFileInfo";


    if(id != m_jobListDirId)
    {
        qDebug() << id;
        qDebug() << "id != m_jobListDirId";
        return;
    }

    if(fileInfoList.isEmpty())
    {
        return;
    }

    /************* FileTreeView ******************/
    if(m_workWidget == WorkFileTreeView)
    {
        QString path = m_currentPath;
        foreach (const QSsh::SftpFileInfo &fi, fileInfoList)
        {
            qDebug()<<fi.name<<" "<<fi.size<<" "<<fi.type<<" "<<fi.permissions;

            if(fi.name == ".." || fi.name == ".")
            {
                continue;
            }

            if(m_currentPath == homePath)
            {
                if(fi.type == QSsh::FileTypeRegular)
                {
                    item = new QStandardItem(getFileIcon(fi.name), fi.name);
                    m_treeItemModel->appendRow(item);
                }
                else if(fi.type == QSsh::FileTypeDirectory)
                {
                    item = new QStandardItem(getFolderIcon(), fi.name);
                    m_treeItemModel->appendRow(item);
                    path = m_currentPath + fi.name + "/";
                    QueueItem *tempQI = new QueueItem;
                    tempQI->it = item;
                    tempQI->strPath = path;
                    items.append(tempQI);
                }
            }// end if
            else
            {
                if(fi.type == QSsh::FileTypeRegular)
                {
                    item = new QStandardItem(getFileIcon(fi.name), fi.name);
                    m_currentItem->appendRow(item);
                }
                else if(fi.type == QSsh::FileTypeDirectory)
                {
                    item = new QStandardItem(getFolderIcon(), fi.name);
                    m_currentItem->appendRow(item);
                    path = m_currentPath + fi.name + "/";
                    QueueItem *tempQI = new QueueItem;
                    tempQI->it = item;
                    tempQI->strPath = path;
                    items.append(tempQI);
                }
            }// end else
        } // end foreash

        m_treeView->sortByColumn(m_treeView->header()->sortIndicatorSection(),
                                  m_treeView->header()->sortIndicatorOrder());
    }// end WorkFileTreeView

    /************* FileWidget ******************/
    if(m_workWidget == WorkFileWidget)
    {
        qDebug() << "refreshDirectory(fileInfoList)";
        page->fileWidget->refreshDirectory(fileInfoList);
    }

}

void SftpServer::handleChannelClosed()
{

}

void SftpServer::getNextLevelList(QString path)
{
    qDebug() << "getNextLevelList : " << path;
    m_jobType = JobListDir;
    m_jobListDirId = m_channel->listDirectory(path);
//    m_currentPath = path;
//    qDebug() << "m_currentPath : " << m_currentPath;
}

void SftpServer::initTreeView()
{
    m_treeView =  new QFileTreeView(this);
    ui->layout->addWidget(m_treeView);

    m_treeView->setSortingEnabled(true);
    m_treeView->setAlternatingRowColors(true);
//    m_treeView->setRootIsDecorated(false);
//    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_treeItemModel = new QStandardItemModel(0, 1,this);
    m_treeItemModel->setHeaderData(0, Qt::Horizontal, tr("Name"));

    m_treeView->setModel(m_treeItemModel);
    m_treeView->header()->setSortIndicator(0, Qt::AscendingOrder);
}

void SftpServer::initPage()
{
    connect(page->fileWidget, SIGNAL(openFileSystemClicked()), this, SLOT(handleOpenFileWidgetClicked()));
    connect(page->fileWidget, SIGNAL(openClicked(QString,QString)), this, SLOT(handleOpenClicked(QString,QString)));
    connect(page->fileWidget, SIGNAL(upClicked()), this, SLOT(handleUpClicked()));
    connect(page->fileWidget, SIGNAL(homeClicked()), this, SLOT(handleHomeClicked()));
    qDebug() << page->fileWidget;
}

void SftpServer::handleOpenFileWidgetClicked()
{
    qDebug() << "handleOpenFileWidgetClicked()";

    //list
    m_jobType = JobListDir;
    m_workWidget = WorkFileWidget;
    m_jobListDirId = m_channel->listDirectory(m_shellPath);
}

void SftpServer::handleOpenClicked(const QString &fileName,const QString &fileType)
{
    qDebug() << "handleOpenClicked";

    if(fileType == getFolderType())
    {
        m_shellPath = m_shellPath + fileName + "/";
        qDebug() << "Current Shell Path : " << m_shellPath;

        // list directory
        m_jobType == JobListDir;
        m_workWidget = WorkFileWidget;
        m_jobListDirId = m_channel->listDirectory(m_shellPath);
    }
    else
    {
        qDebug() << "Not a file, can't open, choose to download!";
        if(QMessageBox::Yes ==
                QMessageBox::question(page->fileWidget, tr("open"),
                                      tr("This file is not a folder, do you want to download?"),
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
        {
            handleDownloadClicked(fileName, fileType);
        }
        return;
    }
}

void SftpServer::handleUpClicked()
{
    qDebug() << "handleUpClicked";

    if(m_shellPath == homePath)
    {
        qDebug() << "m_shellPath == homePath, is home , can't Up!";
        return;
    }

    QString path = m_shellPath;

    // chop the end
    path.chop(1);

    // if the root is "/", after chop the end, the path will be empty
    if(path.isEmpty())
    {
        return;
    }

    QString parentPath = path.left(path.lastIndexOf("/") + 1);

    // update the shellPath
    m_shellPath = parentPath;
    qDebug() << "m_shellPath : " << m_shellPath;

    // list up direcotry
    m_jobType = JobListDir;
    m_workWidget = WorkFileWidget;
    m_jobListDirId = m_channel->listDirectory(m_shellPath);
}

void SftpServer::handleHomeClicked()
{
    qDebug() << "handleHomeClicked";

    // update shellPath to homePath
    m_shellPath = homePath;

    // list up direcotry
    m_jobType = JobListDir;
    m_workWidget = WorkFileWidget;
    m_jobListDirId = m_channel->listDirectory(m_shellPath);
}

void SftpServer::handleDownloadClicked(const QString &fileName,const QString &fileType)
{
    qDebug() << "handleDownloadClicked";
}

