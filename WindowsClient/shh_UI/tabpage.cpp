#include "tabpage.h"
#include "ui_tabpage.h"

TabPage::TabPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabPage)
{
    ui->setupUi(this);
    initStyle();
}

TabPage::~TabPage()
{
    delete ui;
}

void TabPage::initStyle()
{
    /****** 用public指针指向私有成员，外部访问可通使用指针 *******/
    ui->fileSystemWidget->setVisible(false);    //设置初始用于显示文件资源管理系统的窗口为不可见
    ui->filePathLineText->setVisible(false);
    textEdit = new ShellTextEdit(ui->textWidget);   //新增TextEdit
    ui->textWidgetLayout->addWidget(textEdit);
    filePathLineEdit = ui->filePathLineText;
    fileWidget = new FileWidget(ui->fileSystemWidget);  //新增FileWidget
    ui->fileSystemWidgetLayout->addWidget(fileWidget);
}

//打开文件管理系统
void TabPage::openFileSystem()
{
    //如果文件管理系统已经打开
    if(isOpenFileSystem)
    {
        showFileSystem();
        return;
    }

    ui->filePathLineText->setText("d:\\");
    ui->filePathLineText->setAlignment(Qt::AlignLeft);
    ui->filePathLineText->setEnabled(false);
    ui->filePathLineText->setFocusPolicy(Qt::NoFocus);
    ui->filePathLineText->setVisible(true);

    fileWidget->openFileSystem();                                              //TODO 用于测试是否能显示图标
    ui->fileSystemWidget->setFrameShape(QFrame::NoFrame);
    showFileSystem();
    isOpenFileSystem = true;
}

void TabPage::closeFileSystem()
{
    //此处为方便下一次打开做隐藏处理
    concealFileSystem();
}

void TabPage::showFileSystem()
{
    if(isShowFileSystem == false)
    {
       ui->fileSystemWidget->setVisible(true);
       //设置显示文件管理系统的值为真
       isShowFileSystem = true;
       return;
    }
    //如果已经文件管理系统打开
/*
    if(isShowFileSystem == true)
    {
        //提示对话框
        QMessageBox::information(this, tr("提示"), tr("文件管理系统已经显示！"), QMessageBox::Ok);
    }*/
}

void TabPage::concealFileSystem()
{
    if(isShowFileSystem == false)
        return;

    if(isShowFileSystem == true)
    {
        ui->fileSystemWidget->setVisible(false);
        isShowFileSystem = false;
        return;
    }
}
