#ifndef TRANSFERTHREADCLASS
#define TRANSFERTHREADCLASS

#endif // TRANSFERTHREADCLASS
#include "mainprogress.h"
#include "qthread.h"

typedef unsigned int (*TransferTheradFunction)(void*);

extern bool transferring;



class TransferTheradClass: public QThread{
    Q_OBJECT
    public:
        TransferTheradFunction transferTheradFunction;
        TransferTheradClass(){
            transferTheradFunction=NULL;
        }
        void run(){
            if(transferring){
                message("转换只能运行一份！");
                exec();
                return;
            }
            setCloseable(false);
            transferring=true;
            if(transferTheradFunction!=NULL){
                transferTheradFunction(this);
            }
            transferring=false;
            setCloseable(true);
            exec();
        }
        void message(std::string s){
            QString qstr=QString::fromStdString(s);
            emit attachLogSignal(qstr);
            QThread::msleep(100);
        }
        void setCloseable(bool closeable){
            emit setCloseableSignal(closeable);
        }
    signals:
        void attachLogSignal(QString);
        void setCloseableSignal(bool);
};
