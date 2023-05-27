#include "confusesetting.h"
#include "ui_confusesetting.h"
using namespace std;
extern string programPathLocal;
extern vector<string> confuseExcept;
extern void saveFile(string path,vector<string> &strArr,bool hasBom);

ConfuseSetting::ConfuseSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfuseSetting)
{
    ui->setupUi(this);
    setFixedSize(this->width(), this->height());

    connect(ui->listWidget,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelectionChanged()));
    ui->pushButton->installEventFilter(this);
    ui->pushButton_2->installEventFilter(this);
    ui->pushButton_3->installEventFilter(this);
    ui->pushButton_4->installEventFilter(this);
    ui->pushButton_5->installEventFilter(this);
    unsigned int i;
    ui->listWidget->clear();
    QString qstr;
    for(i=0;i<confuseExcept.size();i++){
        if(confuseExcept[i]!=""){
            qstr=QString::fromStdString(confuseExcept[i]);
            ui->listWidget->addItem(qstr);
        }
    }
    ui->listWidget->addItem("...");
}

bool ConfuseSetting::eventFilter(QObject *o, QEvent *e){
    int i;
    if(o==ui->pushButton && e->type()==QEvent::MouseButtonRelease)
    {
        confuseExcept.clear();
        for(i=0;i<ui->listWidget->count()-1;i++){
            confuseExcept.push_back(ui->listWidget->item(i)->text().toStdString());
        }
        saveFile(programPathLocal+"confuse_except.txt",confuseExcept,true);

        accept();
    }
    if(o==ui->pushButton_2 && e->type()==QEvent::MouseButtonRelease)
    {
        reject();
    }
    if(o==ui->pushButton_3 && e->type()==QEvent::MouseButtonRelease && ui->lineEdit->text().length()>0)
    {
        ui->listWidget->insertItem(ui->listWidget->currentIndex().row(),ui->lineEdit->text());
    }
    if(o==ui->pushButton_4 && e->type()==QEvent::MouseButtonRelease && ui->listWidget->currentIndex().row()<ui->listWidget->count()-1)
    {
        ui->listWidget->currentItem()->setText(ui->lineEdit->text());
    }
    if(o==ui->pushButton_5 && e->type()==QEvent::MouseButtonRelease && ui->listWidget->currentIndex().row()<ui->listWidget->count()-1)
    {
        ui->listWidget->takeItem(ui->listWidget->currentIndex().row());
    }
    return false;
}

void ConfuseSetting::onItemSelectionChanged(){
    if(ui->listWidget->currentIndex().row()<ui->listWidget->count()-1){
        ui->lineEdit->setText(ui->listWidget->currentItem()->text());
    }
}

ConfuseSetting::~ConfuseSetting()
{
    delete ui;
}


