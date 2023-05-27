#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <io.h>

#include "confuseSetting.h"
#include "mainprogress.h"
using namespace std;
extern string programPathLocal;
extern int transferMode;
extern string flashFileName;
extern void stringReplace(string &strBase, string strSrc, string strDes);
extern vector<string> confuseExcept;
extern int openFile(string path,vector<string> &strArr);
extern string destPostfix;
extern bool overrideFileMode;
extern bool as2Mode;
extern bool objectBasedClassMode;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(this->width(), this->height());

    setWindowTitle("Flash Transfer");

    ui->comboBox->addItem("ActionScript 转 JavaScript");
    ui->comboBox->addItem("ActionScript 3.0 混淆");
    destPostfix="_transferred";
    ui->lineEdit_2->setText(QString::fromLocal8Bit(destPostfix.c_str()));


    ui->pushButton->installEventFilter(this);
    ui->pushButton_2->installEventFilter(this);
    ui->OKBtn->installEventFilter(this);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onComboBox()));
    if(openFile(programPathLocal+"confuse_except.txt",confuseExcept)==-1){
        confuseExcept.clear();
    }
}

void MainWindow::initOpenText(const char *pathname){
    string pathnamet=QString::fromLocal8Bit(pathname).toStdString();
    stringReplace(pathnamet, "\\", "/");
    ui->lineEdit->setText(pathnamet.c_str());
}

bool MainWindow::eventFilter(QObject *o, QEvent *e){


    if(o==ui->pushButton && e->type()==QEvent::MouseButtonRelease)
    {
        //进行事件处理
        //QString path = QFileDialog::getOpenFileName(this, tr("选择Flash文件"), "D:/llg/飞车/飞车 - beta5/", tr("Flash Files(*.xfl *.as)"));
        //QString path = QFileDialog::getOpenFileName(this, tr("选择Flash文件"), "D:/applicationProject/模型查看器_test/", tr("Flash Files(*.xfl *.as)"));
        QString path = QFileDialog::getOpenFileName(this, tr("选择Flash文件"), "", tr("Flash Files(*.xfl *.as)"));
        if(path.length() == 0) {
           return false;
       }
       ui->lineEdit->setText(path);
    }
    if(o==ui->pushButton_2 && e->type()==QEvent::MouseButtonRelease)
    {
        //进行事件处理

        std::string pathSetting="";
        if(ui->comboBox->currentIndex()==0){
            pathSetting=programPathLocal+"macrojs.txt";
            if(_access(pathSetting.c_str(),0)==0){
                QDesktopServices::openUrl(QUrl(pathSetting.c_str()));
            }else{
                pathSetting=programPathLocal+"macrojs.txt";
                FILE *fpsetting0=fopen(pathSetting.c_str(),"w");
                if(fpsetting0!=NULL){
                    fputc(0xef,fpsetting0);
                    fputc(0xbb,fpsetting0);
                    fputc(0xbf,fpsetting0);
                    fclose(fpsetting0);
                    fpsetting0=NULL;
                }
                QDesktopServices::openUrl(QUrl(pathSetting.c_str()));
            }
        }else if(ui->comboBox->currentIndex()==1){
            ConfuseSetting csdlg;
            csdlg.exec();
        }
    }
    if(o==ui->OKBtn && e->type()==QEvent::MouseButtonRelease)
    {
        //进行事件处理
        if(ui->lineEdit->text()==""){
            QMessageBox::information(this,"提示","必须输入要转换的文件名。");
            ui->lineEdit->setFocus();
            ui->lineEdit->selectAll();
            return false;
        }
        if(_access(ui->lineEdit->text().toLocal8Bit().data(),0)!=0){
            QMessageBox::information(this,"提示","要转换的文件不存在。");
            ui->lineEdit->setFocus();
            ui->lineEdit->selectAll();
            return false;
        }
        if(ui->comboBox->currentIndex()==1 && ui->lineEdit_2->text()=="" && ui->checkBox_overrideMode->isChecked()){
            QMessageBox::information(this,"提示","混淆模式时的目标后缀名不能为空（可以取消勾选“覆盖模式”项）。");
            return false;
        }
        flashFileName=ui->lineEdit->text().toLocal8Bit().data();
        stringReplace(flashFileName, "\\", "/");
        transferMode=ui->comboBox->currentIndex();
        destPostfix=ui->lineEdit_2->text().toLocal8Bit().data();
        if(destPostfix.length()>0 && (destPostfix[destPostfix.length()-1]=='/' || destPostfix[destPostfix.length()-1]=='\\')){
            destPostfix.resize(destPostfix.length()-1);
        }
        overrideFileMode=ui->checkBox_overrideMode->isChecked();
        objectBasedClassMode=ui->checkBox->isChecked();
        as2Mode=ui->checkBox_2->isChecked();
        MainProgress mpdlg;
        mpdlg.exec();

    }
    return false;
}

void MainWindow::onComboBox(){
   if(ui->comboBox->currentIndex()==0){
        ui->checkBox->setEnabled(true);
        ui->checkBox_2->setEnabled(true);
   }
   if(ui->comboBox->currentIndex()==1){
       ui->checkBox->setEnabled(false);
       ui->checkBox_2->setEnabled(false);
   }
}



MainWindow::~MainWindow()
{
    delete ui;
}
