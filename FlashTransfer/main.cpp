#include "mainwindow.h"
#include <QApplication>
using namespace std;
bool inconsole=false;
string programPathLocal="";
extern int transferMode;
extern string flashFileName;
extern void stringReplace(string &strBase, string strSrc, string strDes);
extern vector<string> confuseExcept;
extern int openFile(string path,vector<string> &strArr);
extern string destPostfix;
extern bool overrideFileMode;
extern bool as2Mode;
extern bool cMode;
extern bool objectBasedClassMode;
extern string macrojsPath;
extern bool outputInside;
extern int vectorStringIndexOf(vector<string> &arr,string value);
extern bool transferring;
extern unsigned int asToJsThread(void* param=NULL);


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    programPathLocal=(QCoreApplication::applicationDirPath()+"/").toLocal8Bit().data();
    MainWindow w;
    if(argc<3){
        if(argc==2){
            w.initOpenText(argv[1]);
        }
        w.show();
        return a.exec();
    }else{
        inconsole=true;
        vector<string> args;
        int i=0;
        for(i=0;i<argc;i++){
            args.push_back(argv[i]);
        }
        flashFileName=args[1];
        macrojsPath=args[2];
        stringReplace(flashFileName, "\\", "/");
        stringReplace(macrojsPath, "\\", "/");
        transferMode=0;
        destPostfix="_transferred";
        overrideFileMode=true;
        objectBasedClassMode=vectorStringIndexOf(args,"-obj")>=0;
        as2Mode=vectorStringIndexOf(args,"-as2")>=0;
        cMode=vectorStringIndexOf(args,"-c")>=0;
        outputInside=vectorStringIndexOf(args,"-oi")>=0;
        qDebug("--FlashTransfer--");
        transferring=true;
        asToJsThread();
        return 0;
    }
}
