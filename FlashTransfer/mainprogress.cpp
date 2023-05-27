#include "mainprogress.h"
#include "ui_mainprogress.h"
#include "transferthreadclass.h"
#include <QKeyEvent>
#include <regex.h>
#include <QFileDialog>
#include <QMessageBox>
#include <sstream>
#include <QTextCodec>
#include <time.h>
#include <io.h>
#include <direct.h>
#include <QThread>
//direct.h _mkdir
using namespace std;

extern bool inconsole;
extern string programPathLocal;

typedef struct constTable{
    vector<string> commentTable;
    vector<int> commentPosTable;
    vector<string> stringTable;
    vector<string> regexpTable;
    vector<string> numberTable;
    vector<string> intTable;
    vector<string> objectTable;
    vector<string> xmlTable;
}ConstTable;

typedef struct varibleTable{
    vector<string> propertyTable;
    vector<string> propertyTypeTable;
    vector<string> methodTable;
    vector<string> methodTypeTable;
    vector<string> propertyConfusedTable;
    vector<string> methodConfusedTable;
}VaribleTable;



int transferMode=0;
string destPostfix="_transferred";
bool overrideFileMode=false;
bool as2Mode=false;
bool objectBasedClassMode=false;
bool cMode=false;
bool outputInside=false;
vector<string> confuseExcept;
bool transferring=false;
string flashFileName="";
string macrojsPath="";
QTextEdit *ui_log=NULL;
QProgressBar *ui_progress=NULL;
TransferTheradClass *transferTheradClass=NULL;

string substring(string &str0,int begin=0,int end=0x7fffffff){
    string strt="";
    int leng0=str0.length();

    if(leng0<0){
        strt=str0;
        return strt;
    }

    if(end>leng0){
        end=leng0;
    }
    if(begin<0){
        begin=0;
    }
    if(begin>=end){
        strt="";
        return strt;
    }

    if(begin==0 && end==leng0){
        strt=str0;
        return strt;
    }

    strt=str0.substr(begin,end-begin);
    return strt;
}

void stringReplace(string &strBase, string strSrc, string strDes){
    string::size_type pos = 0;
    string::size_type srcLen = strSrc.size();
    string::size_type desLen = strDes.size();
    pos=strBase.find(strSrc, pos);
    while (pos != string::npos){
        strBase.replace(pos, srcLen, strDes);
        pos=strBase.find(strSrc, (pos+desLen));
    }
}

int vectorStringIndexOf(vector<string> &arr,string value){
    int i=0,leng0=arr.size();
    if(leng0<=0){
        return -1;
    }
    for(i=0;i<leng0;i++){
        if(arr[i]==value){
            return i;
        }
    }
    return -1;
}

void concatVectorStringWith(vector<string> &arrConcated,vector<string> &arrConcatWith){
    unsigned int a=arrConcated.size(),b=arrConcatWith.size();
    unsigned int c=a+b;
    unsigned int i;
    arrConcated.resize(c);
    for(i=a;i<c;i++){
        arrConcated[i]=arrConcatWith[i-a];
    }
}

void copyVectorString(vector<string> &arr0,vector<string> &arrt){
    unsigned int i=0;
    unsigned int length=arr0.size();
    arrt.resize(length);
    for(i=0;i<length;i++){
        arrt[i]=arr0[i];
    }
}

QString displayString(const string &str0,bool ansimode=false){
    if(ansimode){
        QTextCodec *gbk = QTextCodec::codecForName("GB18030");
        QString strt = gbk->toUnicode(str0.c_str());
        return strt;
    }else{
        QString str = QString::fromStdString(str0);
        return str;
    }
}

string gbToUtf8(const string &str0){
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
    QString strt = gbk->toUnicode(str0.c_str());
    return strt.toStdString();
}

string utf8ToGb(const string &str0){
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
    QString str = QString::fromStdString(str0);
    string strt = gbk->fromUnicode(str).data();
    return strt;
}

string _t(const string &str0){
    string strt = QString::fromLocal8Bit(str0.c_str()).toStdString();
    return strt;
}


int getFoundPos(int foundData){
    return (foundData<<16)>>16;
}

int getFoundLen(int foundData){
    return foundData>>16;
}

string getReplacedStr(string str0,regmatch_t* pmatch=NULL,int maxCount=10,const char *matchSrc=NULL,int matchSrcLength=0){
    int i=0,j=0;
    int l0=str0.length();
    char strinext=0;
    string strt="";
    int n=0;
    int matchStrLenMax=128;
    char *matchStr=new char[matchStrLenMax];
    int matchStrLen=0;
    int matchStrPos=0;
    char *matchStrt=NULL;
    if(matchSrc!=NULL && matchSrcLength<=0){
        matchSrcLength=strlen(matchSrc);
    }
    for(i=0;i<l0;i++){
        if(i<j){
            continue;
        }
        if(str0[i]=='\\' && i+1<l0){
            strinext=str0[i+1];
            strt+=str0.substr(j,i-j);
            j=i+2;
            if(strinext=='r'){
                strt+="\r";
            }
            if(strinext=='n'){
                strt+="\n";
            }
            if(strinext=='t'){
                strt+="\t";
            }
            if(strinext=='\\'){
                strt+="\\";
            }
            if(strinext=='$'){
                strt+="$";
            }
            if(strinext=='/'){
                strt+="/";
            }
            if(strinext=='*'){
                strt+="*";
            }
            if(strinext=='['){
                strt+="[";
            }
            if(strinext==']'){
                strt+="]";
            }
            if(strinext=='|'){
                strt+="|";
            }            
            if(strinext==';'){
                strt+=";";
            }
            if(strinext=='\"'){
                strt+="\"";
            }
            if(strinext=='\''){
                strt+="\'";
            }
        }
        if(str0[i]=='$' && i+1<l0){
            strinext=str0[i+1];
            strt+=str0.substr(j,i-j);
            j=i+2;
            if(strinext=='$'){
                strt+="$";
            }
        }
        if((str0[i]=='\\' || str0[i]=='$') && i+1<l0 && ((strinext>='0' && strinext<='9') || strinext=='&') && matchSrc!=NULL && pmatch!=NULL){
            n=strinext!='&'?strinext-'0':0;
            if(n>=0 && n<maxCount){
                //strt+=matchSrc.substr(pmatch[n].rm_so,pmatch[n].rm_eo-pmatch[n].rm_so);
                matchStrPos=pmatch[n].rm_so;
                matchStrLen=pmatch[n].rm_eo-matchStrPos;

                if(matchStrPos>=0 && matchStrLen>0 && matchStrPos+matchStrLen<=matchSrcLength){
                    if(matchStrLen<matchStrLenMax){
                        memcpy(matchStr,matchSrc+matchStrPos,matchStrLen);
                        matchStr[matchStrLen]=0;
                        strt+=matchStr;
                    }else{
                        matchStrt=new char[matchStrLen+1];
                        memcpy(matchStrt,matchSrc+matchStrPos,matchStrLen);
                        matchStrt[matchStrLen]=0;
                        strt+=matchStrt;
                        delete []matchStrt;
                    }
                }//else{
                 //   char ostr[1001];
                //    snprintf(ostr,1000,"匹配项的开头为%d,结尾为%d，长度为%d,字符串长度为%d。",(int)(pmatch[n].rm_so),(int)(pmatch[n].rm_eo),matchStrLen,matchSrcLength);
                //    QMessageBox::information(0,"匹配项不存在",ostr);
                //}
            }
        }
    }
    if(i>j){
        strt+=str0.substr(j,i-j);
    }
    delete []matchStr;
    return strt;
}

string getReplacedStr(string str0,regmatch_t* pmatch,int maxCount,string &matchSrc){
    return getReplacedStr(str0,pmatch,maxCount,matchSrc.c_str(),matchSrc.length());
}

int findNext(string &str,string pat,int stringpos0=0,int varNum=0){
    if(getFoundPos(stringpos0)<0 || getFoundLen(stringpos0)<0){
        return -1;
    }
    stringpos0=getFoundPos(stringpos0)+getFoundLen(stringpos0);
    int strlength=str.length();
    if(stringpos0>strlength){
        stringpos0=strlength;
    }

    if(pat==""){
        return -1;
    }
    regex_t reg;
    memset(&reg,0,sizeof(regex_t));

    int errcode=regcomp(&reg,pat.c_str(),REG_EXTENDED);
    if(errcode!=0){
        return -1;
    }


    const size_t nmatch = 10;
    regmatch_t pmatch[nmatch];
    errcode=regexec(&reg,str.c_str()+stringpos0,nmatch,pmatch,0);
    if(errcode!=0){
        regfree(&reg);
        return -1;
    }

    regfree(&reg);
    int strfrom=pmatch[varNum].rm_so+stringpos0;
    int strto=pmatch[varNum].rm_eo+stringpos0;

    if(strfrom>=0 && strfrom<=0x7fff && strto>=0 && strto<=0x7fff && strfrom<strto){
        return (strto-strfrom)<<16|strfrom;
    }else{
        return -1;
    }
}



int findAll(string &str,string pat,const char *replaceStr=NULL,vector<int> *posArr=NULL,int varNum=0){
    int stringpos0=0;
    int strlength=str.length();

    if(pat==""){
        return 0;
    }
    string patt=pat;
    bool findOnce=false;
    if(pat.length()>1 && pat[0]=='^'){
        patt=pat;
        findOnce=true;
    }else if(pat.length()>3 && pat[0]=='(' && substring(pat,0,3)=="(^|"){
        patt="("+substring(pat,3);
    }

    regex_t reg,regt;
    memset(&reg,0,sizeof(regex_t));
    memset(&regt,0,sizeof(regex_t));

    int errcode=regcomp(&reg,pat.c_str(),REG_EXTENDED);
    if(errcode!=0){
        return 0;
    }

    errcode=regcomp(&regt,patt.c_str(),REG_EXTENDED);
    if(errcode!=0){
        return 0;
    }

    const size_t nmatch = 10;
    regmatch_t pmatch[nmatch];
    int strfrom=stringpos0;
    int strto=stringpos0;
    int strfromt=-1;
    int strtot=-1;
    string strmt="";
    string strt="";
    int count=0;
    if(replaceStr==NULL && posArr!=NULL){
        posArr->clear();
    }
    while(regexec(stringpos0==0?&reg:&regt,str.c_str()+stringpos0,nmatch,pmatch,0)==0){
        strfrom=pmatch[0].rm_so+stringpos0;
        strto=pmatch[0].rm_eo+stringpos0;
        //char ostr0[1001];
        //snprintf(ostr0,1000,"%d %d %d %d %d %d %d %d",pmatch[0].rm_so,pmatch[0].rm_eo,pmatch[1].rm_so,pmatch[1].rm_eo,pmatch[2].rm_so,pmatch[2].rm_eo,pmatch[3].rm_so,pmatch[3].rm_eo);
        //QMessageBox::information(0,"",ostr0);
        if(replaceStr!=NULL){
            strmt=getReplacedStr(replaceStr,pmatch,nmatch,str.c_str()+stringpos0,strlength-stringpos0);
            //textStatus->setText(strmt.c_str());
            strt=str.substr(0,strfrom)+strmt+str.substr(strto);
            str.assign(strt);
            strlength=str.length();
            strto=strfrom+strmt.length();
        }else if(posArr!=NULL){
            if(varNum==0){
                posArr->push_back(strfrom);
                posArr->push_back(strto);
            }else{
                strfromt=pmatch[varNum].rm_so+stringpos0;
                strtot=pmatch[varNum].rm_eo+stringpos0;
                posArr->push_back(strfromt);
                posArr->push_back(strtot);
            }
        }
        stringpos0=strto;
        count++;
        if(stringpos0>strlength){
            stringpos0=strlength;
            break;
        }
        if(findOnce){
            break;
        }
    }

    regfree(&regt);
    regfree(&reg);
    //memset(&reg,0,sizeof(regex_t));

    return count;
}

int getFindArray(string &str,string pat,vector<int> &posArr,int varNum=0){
    return findAll(str,pat,NULL,&posArr,varNum);
}

int replaceAll(string &str,string pat,string replaceStr){
    return findAll(str,pat,replaceStr.c_str(),NULL);
}

int mkdirs(const char *pathName0){
    //创建多个目录（文件或目录路径），返回值：0不需要创建，1创建成功，-1创建失败。
    int i=-1;
    string pathName=pathName0;
    stringReplace(pathName,"\\","/");
    int j=pathName.rfind('/');
    int k=pathName.rfind('.');
    if(j<k && j>=0){
        pathName=substring(pathName,0,j+1);
    }else if(j+1!=(int)(pathName.length()) && (pathName[0]=='/' || (int)(pathName.find(':'))>=0)){
        pathName+="/";
    }
    int l=pathName.length();
    string strt="";
    int returnValue=0;
    for(i=0;i<l;i++){
        if(pathName[i]=='/'){
            strt=substring(pathName,0,i+1);
            if(_access(strt.c_str(), 0)==-1){
                if(returnValue==0){
                    returnValue=1;
                }
                if(_mkdir(strt.c_str())!=0){//if(mkdir(strt.c_str())!=0){
                    returnValue=-1;
                    break;
                }
            }
        }
    }
    return returnValue;
}

FILE* fopen_ex(const char *pathName0,const char *type){
    if(mkdirs(pathName0)==-1){
        return NULL;
    }
    return fopen(pathName0,type);
}


int openFile(string path,vector<string> &strArr){
    strArr.clear();
    if(path.length()<=0){
        return -1;
    }
    FILE *fp=fopen(path.c_str(),"r");
    if(fp==NULL){
        return -1;
    }
    char strl[10001];
    string str="";
    //string strt="";
    int i=0;
    bool hasBom=false;
    while(fgets(strl,10000,fp)!=NULL){
        i++;
        str=strl;
        if(i==1 && str.length()>=3 && str[0]==(char)0xef && str[1]==(char)0xbb && str[2]==(char)0xbf){
            str=str.substr(3);
            hasBom=true;
        }
        stringReplace(str,"\r","");
        stringReplace(str,"\n","");
        //strt+=str+"\n";
        strArr.push_back(str);
    }
    fclose(fp);

    return hasBom?1:0;
}

void saveFile(string path,vector<string> &strArr,bool hasBom=true){
    FILE *fp=fopen_ex(path.c_str(),"w");
    if(fp==NULL){
        return;
    }
    if(hasBom){
        fputc(0xef,fp);
        fputc(0xbb,fp);
        fputc(0xbf,fp);
    }
    int i=0;
    int strCount=strArr.size();
    for(i=0;i<strCount;i++){
        //if(hasBom){
            fputs(strArr[i].c_str(),fp);
        //}else{
         //   fputs(utf8ToGb(strArr[i]).c_str(),fp);
        //}
        fputc('\n',fp);
    }
    fclose(fp);
}

void message(string str){
    if(inconsole){
        qDebug(str.c_str());
    }else if(transferTheradClass!=NULL){
        transferTheradClass->message(str);
    }else if(ui_log!=NULL){
        QString strt=ui_log->toPlainText()+str.c_str();
        strt+="\n";
        ui_log->setText(strt);
        ui_log->moveCursor(QTextCursor::End);
    }
}




bool writeInfoTableAndReplace(vector<string> &str0,ConstTable &table){
    //写入预处理信息表并将读取到的文件内容替换成相应的信息表数据编号（防止意外替换），返回是否存在错误
    unsigned int i,j,k;
    int l,m,n;
    unsigned int a,b,c,d;
    unsigned int str0Count=str0.size();
    vector<int> fac0,fac1,fac2,fac3,fas0,far0,fap0;
    string strt;
    string numStr;
    ostringstream ostr;
    bool willAdd=true;

    l=0,m=0,n=0;
    bool incomment=false;
    bool willIncomment=false;
    bool willContinue=false;
    bool hasError=false;
    int preMax=1;
    int preCount=0;
    //ostr<<str0Count;
    //message(ostr.str());
    for(i=0;i<str0Count;i++){
        willContinue=false;

        getFindArray(str0[i],"\\/\\/.*",fac0);
        getFindArray(str0[i],"\\/\\*.*?\\*\\/",fac1);
        getFindArray(str0[i],"\\/\\*.*",fac2);
        getFindArray(str0[i],".*\\*\\/",fac3);
        getFindArray(str0[i],"\"([^\"\\\\]|\\\\.)*\"|\'([^\'\\\\]|\\\\.)*\'",fas0);
        getFindArray(str0[i],"([=,(]\\s*|\\[\\s*)(\\/([^*/\\\\]|\\\\.)([^/\\\\]|\\\\.)*\\/[A-Za-z]*)",far0,2);
        //getFindArray(str0[i],"\'([^\'\\\\]|\\\\.)*\'",fas1);
        j=0;
        while(j+1<fac2.size()){
            willAdd=true;
            for(k=0;k+1<fac1.size();k+=2){
                if(fac2[j]==fac1[k]){
                    willAdd=false;
                    break;
                }
            }
            if(!willAdd){
                fac2.erase(fac2.begin()+j,fac2.begin()+j+2);
            }else{
                j+=2;
            }
        }
        j=0;
        while(j+1<fac3.size()){
            willAdd=true;
            for(k=0;k+1<fac1.size();k+=2){
                if(fac3[j+1]==fac1[k+1]){
                    willAdd=false;
                    break;
                }
            }
            if(!willAdd){
                fac3.erase(fac3.begin()+j,fac3.begin()+j+2);
            }else{
                j+=2;
            }
        }

        willIncomment=false;
        if(!incomment && fac2.size()>0 && fac3.size()==0){
            willIncomment=true;
        }
        if(fac3.size()>0){
            incomment=false;
        }
        if(incomment){
            table.commentTable.push_back(str0[i]);
            table.commentPosTable.push_back(i);
            str0[i]="";
            continue;
        }

        a=fac0.size();
        b=fac1.size();
        c=fac2.size();
        d=fac3.size();
        fac0.resize(a+b+c+d);
        j=a;
        while(j<a+b){
            fac0[j]=fac1[j-a];
            j++;
        }
        while(j<a+b+c){
            fac0[j]=fac2[j-a-b];
            j++;
        }
        while(j<a+b+c+d){
            fac0[j]=fac3[j-a-b-c];
            j++;
        }

        /*a=fas0.size();
        b=fas1.size();
        j=a;
        fas0.resize(a+b);
        while(j<a+b){
            fas0[j]=fas1[j-a];
            j++;
        }*/

        j=0;
        while(j+1<fac0.size()){
            willAdd=true;
            for(k=0;k+1<fas0.size();k+=2){
                if(fac0[j]>=fas0[k] && fac0[j]<fas0[k+1]){
                    willAdd=false;
                    break;
                }
            }
            if(!willAdd){
                fac0.erase(fac0.begin()+j,fac0.begin()+j+2);
            }else{
                j+=2;
            }
        }

        j=0;
        while(j+1<fas0.size()){
            willAdd=true;
            for(k=0;k+1<fac0.size();k+=2){
                if(fas0[j]>=fac0[k] && fas0[j]<fac0[k+1]){
                    willAdd=false;
                    break;
                }
            }
            if(!willAdd){
                fas0.erase(fas0.begin()+j,fas0.begin()+j+2);
            }else{
                j+=2;
            }
        }

        j=0;
        while(j+1<far0.size()){
            willAdd=true;
            for(k=0;k+1<fas0.size();k+=2){
                if(far0[j]>=fas0[k] && far0[j]<fas0[k+1]){
                    willAdd=false;
                    break;
                }
            }
            for(k=0;k+1<fac0.size();k+=2){
                if(far0[j]>=fac0[k] && far0[j]<fac0[k+1]){
                    willAdd=false;
                    break;
                }
            }
            if(!willAdd){
                far0.erase(far0.begin()+j,far0.begin()+j+2);
            }else{
                j+=2;
            }
        }


        strt=str0[i];
        for(j=0;j+1<fac0.size();j+=2){
            table.commentTable.push_back(substring(strt,fac0[j],fac0[j+1]));
            table.commentPosTable.push_back(i);
            l=fac0[j+1]-fac0[j];
            m=fac0[j];
            n=fac0[j+1];
            strt=substring(strt,0,fac0[j])+substring(strt,fac0[j+1]);
            for(k=0;k<far0.size();k++){
                if(far0[k]>n){
                    far0[k]-=l;
                }else if(far0[k]>m){
                    far0[k]=m;
                }
            }
            for(k=0;k<fas0.size();k++){
                if(fas0[k]>n){
                    fas0[k]-=l;
                }else if(fas0[k]>m){
                    fas0[k]=m;
                }
            }
            for(k=j+2;k<fac0.size();k++){
                if(fac0[k]>n){
                    fac0[k]-=l;
                }else if(fac0[k]>m){
                    fac0[k]=m;
                }
            }
        }
        //str0[i]=strt;

        getFindArray(strt,"@[a-z][0-9]+#",fap0);
        for(j=0;j+1<fap0.size();j+=2){
            for(k=0;k+1<fas0.size();k+=2){
                if(fap0[j]>=fas0[k] && fap0[j]<fas0[k+1]){
                    break;
                }
            }
            if(k+1>=fas0.size()){
                for(k=0;k+1<far0.size();k+=2){
                    if(fap0[j]>=far0[k] && fap0[j]<far0[k+1]){
                        break;
                    }
                }

                if(k+1>=far0.size()){
                    if(preCount<preMax){
                        ostr.str("");
                        ostr<<"Error:在第"<<(i+1)<<"行处发现注释和字符串外的中转字符/@[a-z][0-9]+#/。("<<strt<<")";
                        message(ostr.str());
                    }else if(preCount==preMax){
                        message("...");
                    }
                    preCount++;
                    strt="//"+strt;
                    table.commentTable.push_back(strt);
                    table.commentPosTable.push_back(i);
                    str0[i]="";

                    if(willIncomment){
                        incomment=true;
                    }
                    hasError=true;
                    willContinue=true;
                    break;
                }

            }
        }
        if(willContinue){
            continue;
        }

        //strt=str0[i];
        for(j=0;j+1<fas0.size();j+=2){
            ostr.str("");
            ostr<<"@s"<<table.stringTable.size()<<"#";
            numStr=ostr.str();
            l=fas0[j+1]-fas0[j]-numStr.length();
            m=fas0[j];
            n=fas0[j+1];
            table.stringTable.push_back(substring(strt,fas0[j],fas0[j+1]));
            strt=substring(strt,0,fas0[j])+numStr+substring(strt,fas0[j+1]);
            for(k=0;k<far0.size();k++){
                if(far0[k]>n){
                    far0[k]-=l;
                }else if(far0[k]>m){
                    far0[k]=m+numStr.length();
                }
            }
            for(k=j+2;k<fas0.size();k++){
                if(fas0[k]>n){
                    fas0[k]-=l;
                }else if(fas0[k]>m){
                    fas0[k]=m+numStr.length();
                }
            }
        }

        for(j=0;j+1<far0.size();j+=2){
            ostr.str("");
            ostr<<"@r"<<table.regexpTable.size()<<"#";
            numStr=ostr.str();
            l=far0[j+1]-far0[j]-numStr.length();
            m=far0[j];
            n=far0[j+1];
            table.regexpTable.push_back(substring(strt,far0[j],far0[j+1]));
            strt=substring(strt,0,far0[j])+numStr+substring(strt,far0[j+1]);
            for(k=j+2;k<far0.size();k++){
                if(far0[k]>n){
                    far0[k]-=l;
                }else if(far0[k]>m){
                    far0[k]=m+numStr.length();
                }
            }
        }

        str0[i]=strt;


        if(str0[i].length()>0x7fff){
            ostr.str("");
            ostr<<"Error:行"<<(i+1)<<"下的文本过长。";
            message(ostr.str());
            hasError=true;
            continue;
        }

        strt=str0[i];
        l=findNext(strt,"^[0-9]*\\.[0-9]+([Ee][+\\-]?[0-9]+)?",0,0);
        m=getFoundPos(l);
        if(m==0){
            n=getFoundLen(l);
            ostr.str("");
            ostr<<"@n"<<table.numberTable.size()<<"#";
            numStr=ostr.str();
            table.numberTable.push_back(substring(strt,0,n));
            strt=numStr+substring(strt,n);
        }
        l=findNext(strt,"\\W([0-9]*\\.[0-9]+([Ee][+\\-]?[0-9]+)?)",0,1);
        m=getFoundPos(l);
        n=getFoundLen(l);
        while(m>=0 && n>=0){
            m=getFoundPos(l);
            n=getFoundLen(l);
            ostr.str("");
            ostr<<"@n"<<table.numberTable.size()<<"#";
            numStr=ostr.str();
            table.numberTable.push_back(substring(strt,m,m+n));
            strt=substring(strt,0,m)+numStr+substring(strt,m+n);
            l=findNext(strt,"\\W([0-9]*\\.[0-9]+([Ee][+\\-]?[0-9]+)?)",0,1);
            m=getFoundPos(l);
            n=getFoundLen(l);
        }
        str0[i]=strt;

        strt=str0[i];
        l=findNext(strt,"^0x[A-Fa-f0-9]+|[0-9]+",0,0);
        m=getFoundPos(l);
        if(m==0){
            n=getFoundLen(l);
            ostr.str("");
            ostr<<"@i"<<table.intTable.size()<<"#";
            numStr=ostr.str();
            table.intTable.push_back(substring(strt,0,n));
            strt=numStr+substring(strt,n);
        }
        l=findNext(strt,"\\W(0x[A-Fa-f0-9]+|[0-9]+)",0,1);
        m=getFoundPos(l);
        n=getFoundLen(l);
        while(m>=0 && n>=0){
            ostr.str("");
            ostr<<"@i"<<table.intTable.size()<<"#";
            numStr=ostr.str();
            table.intTable.push_back(substring(strt,m,m+n));
            strt=substring(strt,0,m)+numStr+substring(strt,m+n);
            l=findNext(strt,"\\W(0x[A-Fa-f0-9]+|[0-9]+)",0,1);
            m=getFoundPos(l);
            n=getFoundLen(l);
        }
        str0[i]=strt;

        strt=str0[i];
        l=findNext(strt,"[\\=\\(][\\s]*(\\{[^}]*\\})",0,1);//findNext(strt,"[\\=\\(][\\s]*(\\{[^\\}]*\\})",0,1);
        m=getFoundPos(l);
        n=getFoundLen(l);
        while(m>=0 && n>=0){
            ostr.str("");
            ostr<<"@o"<<table.objectTable.size()<<"#";
            numStr=ostr.str();
            table.objectTable.push_back(substring(strt,m,m+n));
            strt=substring(strt,0,m)+numStr+substring(strt,m+n);
            l=findNext(strt,"[\\=\\(][\\s]*(\\{[^}]*\\})",0,1);//findNext(strt,"[\\=\\(][\\s]*(\\{[^\\}]*\\})",0,1);
            m=getFoundPos(l);
            n=getFoundLen(l);
        }
        str0[i]=strt;

        if(willIncomment){
            incomment=true;
        }
    }
    return !hasError;
}

void moveComment(constTable &table,int rowAt,int commentMovement,bool rowAtIsEmpty=false){
    int i,lengt;
    bool isCommentEnd;
    if((commentMovement!=1 && commentMovement!=-1) ||
       table.commentTable.size()==0 || table.commentTable.size()!=table.commentPosTable.size()){
        return;
    }
    for(i=(int)(table.commentPosTable.size())-1;i>=0;i--){
        if(table.commentPosTable[i]>rowAt){
            table.commentPosTable[i]+=commentMovement;
        }else if(table.commentPosTable[i]==rowAt){
            if(commentMovement==-1){
                if(!rowAtIsEmpty || rowAt==0){
                    break;
                }
                table.commentPosTable[i]+=commentMovement;
            }else if(commentMovement==1){
                lengt=table.commentTable[i].length();
                isCommentEnd=(substring(table.commentTable[i],lengt-2)=="*/" && substring(table.commentTable[i],0,2)!="/*");
                if(isCommentEnd){
                    break;
                }
                table.commentPosTable[i]+=commentMovement;
            }
        }else{
            break;
        }
    }
}

bool recombineStatements(vector<string> &str0,ConstTable &table){
    //语句重组并自动套用格式
    unsigned int i,j;
    unsigned int str0Count=str0.size();
    vector<int> fas;
    string strt;
    string numStr;
    ostringstream ostr;
    bool willAdd=true;


    bool hasError=false;
    int bracketLevel=0;
    unsigned int lengt;
    int commentMovement=0;
    //ostr<<str0Count;
    //message(ostr.str());
    //清除不必要的空格
    for(i=0;i<str0Count;i++){
        if(str0[i].length()>0x7fff){
            ostr.str("");
            ostr<<"Error:行"<<(i+1)<<"下的文本过长。";
            message(ostr.str());
            hasError=true;
            continue;
        }

        getFindArray(str0[i],"\\s+",fas);
        if(fas.size()>=2 && fas[fas.size()-1]==(int)(str0[i].length())){
           str0[i]=substring(str0[i],0,fas[fas.size()-2]);
        }
        if(fas.size()>=2 && fas[0]==0){
           str0[i]=substring(str0[i],fas[1]);
        }
        replaceAll(str0[i],"\\s{2,}"," ");
        replaceAll(str0[i],"\\s+([^A-Za-z0-9_@])","$1");
        replaceAll(str0[i],"([^A-Za-z0-9_#])\\s+","$1");
    }

    //自动加分号
    bracketLevel=0;
    for(i=0;i<str0Count;i++){
        lengt=str0[i].length();
        for(j=0;j<lengt;j++){
            if(str0[i][j]=='(' || str0[i][j]=='['){
                bracketLevel++;
            }
            if(str0[i][j]==')' || str0[i][j]==']'){
                bracketLevel--;
            }
        }
        if(findNext(str0[i],"^(if|else|for|while|try|catch|finally)(\\W|$)",0,0)>=0){
            continue;
        }
        if(bracketLevel==0 && lengt>0 &&
           str0[i][lengt-1]!=';' && str0[i][lengt-1]!='{'  && str0[i][lengt-1]!='}'  &&
           str0[i][lengt-1]!='=' && str0[i][lengt-1]!=',' && str0[i][lengt-1]!=':' &&
           (i+1>=str0Count || str0[i+1].length()==0 ||
            (str0[i+1][0]!='=' && str0[i+1][0]!=',' && str0[i+1][0]!='{'))){
            str0[i]+=";";
            /*if(i+1<str0Count && str0[i+1].length()>0){
                static char cstr[1001];
                snprintf(cstr,1000,"%c %c",str0[i+1][0],str0[i][lengt-1]);
                message(cstr);
            }*/
        }
    }

    //按语句进行重组
    bracketLevel=0;
    i=0;
    while(i<str0.size()){
        willAdd=true;
        lengt=str0[i].length();
        j=0;
        while(j<lengt){
            if(str0[i][j]==';' || str0[i][j]=='{' || str0[i][j]=='}'){
                break;
            }
            j++;
        }
        commentMovement=0;
        if(j>=lengt){
            if(i+1<str0.size()
            && findNext(str0[i],"^(else|try|finally)(\\W|$)",0,0)<0
            && substring(str0[i+1],0,2)!="@_"){
                str0[i]=str0[i]+str0[i+1];
                str0.erase(str0.begin()+(i+1));
                willAdd=false;
                commentMovement=-1;
            }else if(i+1==str0.size() && lengt==0){
                str0.erase(str0.begin()+i);
            }
        }else if(lengt>0 && j+1<lengt){
            strt=substring(str0[i],j+1);
            str0.insert(str0.begin()+(i+1),strt);
            str0[i]=substring(str0[i],0,j+1);
            commentMovement=1;
        }
        if(commentMovement==1 || commentMovement==-1){
             moveComment(table,i,commentMovement,(lengt==0));
        }
        if(willAdd){
            i++;
        }
    }

    return !hasError;
}

bool getVaribleTable(vector<string> &str,varibleTable &table,int braceLevel0=-2){
    //语句重组并自动套用格式
    unsigned int i,j;
    int k,l,m,n,p;
    unsigned int strCount=str.size();
    string strt,strt2,strt3;
    ostringstream ostr;


    bool hasError=false;
    unsigned int lengt;
    bool isOverride=false;
    int bracketLevel=0;
    bool inFunction=false;

    //ostr<<str0Count;
    //message(ostr.str());
    k=braceLevel0;
    for(i=0;i<strCount;i++){
        if(str[i].length()>0x7fff){
            ostr.str("");
            ostr<<"Error:行"<<(i+1)<<"下的文本过长。";
            message(ostr.str());
            hasError=true;
            continue;
        }
        lengt=str[i].length();

        if(findNext(str[i],"(^|\\s)(function) ",0,2)>=0){
            if(k>0){
                //ostr.str("");
                //ostr<<"Info:在第"<<(i+1)<<"行处发现嵌套定义的函数。("<<str[i]<<")";
                //message(ostr.str());
            }else if(k<0){
                ostr.str("");
                ostr<<"Warning:在第"<<(i+1)<<"行处发现可能是包外类的函数（暂不支持）。("<<str[i]<<")";
                message(ostr.str());
            }else{
                inFunction=true;
                if(findNext(str[i],"(^|\\s)(override) ",0,2)>=0){
                    //ostr.str("");
                    //ostr<<"Info:在第"<<(i+1)<<"行处发现override的函数。("<<str[i]<<")";
                    //message(ostr.str());
                    isOverride=true;
                }else{
                    isOverride=false;
                }
                strt=str[i];
                replaceAll(strt,"function (\\w+)[(].*[)](\\:[A-Za-z0-9_.<>*]+)?[{;]","|$1|$2");
                m=strt.find('|',0);
                n=strt.find('|',m+1);
                if(m>=0 && n>=0){
                    strt2=substring(strt,m+1,n);
                    strt3=substring(strt,n+2);
                    if(isOverride){
                        strt2+=":";
                    }
                    if(strt3=="void" || strt3=="*"){
                        strt3="";
                    }
                    if(vectorStringIndexOf(table.methodTable,strt2)<0){
                        table.methodTable.push_back(strt2);
                        table.methodTypeTable.push_back(strt3);
                    }else{
                        ostr.str("");
                        ostr<<"Warning:函数定义重复，在第"<<(i+1)<<"行。("<<str[i]<<")";
                        message(ostr.str());
                    }
                }

                strt=str[i];
                replaceAll(strt,"function (get|set) (\\w+)[(].*(\\:[A-Za-z0-9_.<>*]+)","|$2|$3");
                m=strt.find('|',0);
                n=strt.find('|',m+1);
                if(m>=0 && n>=0){
                    strt2=substring(strt,m+1,n);
                    strt3=substring(strt,n+2);
                    if(isOverride){
                        strt2+=":";
                    }
                    if(strt3.length()>0 && strt3[strt3.length()-1]=='{'){
                        strt3=substring(strt3,0,strt3.length()-1);
                    }
                    if(strt3=="void" || strt3=="*"){
                        strt3="";
                    }
                    p=vectorStringIndexOf(table.propertyTable,strt2);
                    if(p<0){
                        table.propertyTable.push_back(strt2);
                        table.propertyTypeTable.push_back(strt3);
                    }else if(strt3!="" && table.propertyTypeTable[p]==""){
                        table.propertyTypeTable[p]=strt3;
                    }
                }

            }
        }

        if(!inFunction && k>=0){//if(k==0){//(是否识别条件和循环内定义的变量)
            l=findNext(str[i],"(^|\\s)(var) ",0,2);
            if(l>=0){
                l=findNext(str[i]," (\\w+)[:=,;]",l,1);
                while(l>=0){
                    m=getFoundPos(l);
                    n=getFoundLen(l);
                    strt=substring(str[i],m,m+n);

                    bracketLevel=0;
                    for(j=0;j<lengt;j++){
                        if(str[i][j]=='(' || str[i][j]=='['){
                            bracketLevel++;
                        }
                        if(str[i][j]==')' || str[i][j]==']'){
                            bracketLevel--;
                        }
                        if(m==(int)j){
                            break;
                        }
                    }
                    if(j<lengt && bracketLevel!=0){
                        l=findNext(str[i],",(\\w+)[:=,;]",l,1);
                        continue;
                    }

                    if(vectorStringIndexOf(table.propertyTable,strt)>=0){
                        ostr.str("");
                        ostr<<"Warning:变量定义重复，在第"<<(i+1)<<"行。("<<str[i]<<")";
                        message(ostr.str());
                        break;
                    }
                    table.propertyTable.push_back(strt);

                    p=l;
                    l=findNext(str[i],"^\\:([A-Za-z0-9_.<>*]+)[=,;]",l,1);
                    if(l>=0){
                        m=getFoundPos(l);
                        n=getFoundLen(l);
                        strt=substring(str[i],m,m+n);
                        table.propertyTypeTable.push_back(strt!="*"?strt:"");
                    }else{
                        table.propertyTypeTable.push_back("");
                    }
                    l=p;

                    l=findNext(str[i],",(\\w+)[:=,;]",l,1);
                }
            }
        }else if(k<0 && findNext(str[i],"(^|\\s)(var) ",0,2)>=0){
            ostr.str("");
            ostr<<"Warning:在第"<<(i+1)<<"行处发现可能是包外类定义的变量（暂不支持）。("<<str[i]<<")";
            message(ostr.str());
        }

        for(j=0;j<lengt;j++){
            if(str[i][j]=='{'){
                k++;
            }
            if(str[i][j]=='}'){
                k--;
            }
        }

        if(inFunction && k==0){
            inFunction=false;
        }

    }

    return !hasError;
}

void getConfusedVaribles(varibleTable &table,string varStrHead,string funcStrHead,int varNumberBegin=0,int funcNumberBegin=0){
    int i,j,temp;
    vector<int> numArr;
    string strt="";
    int strtLen=0;
    char ostr[128];

    int leng0=table.propertyTable.size();
    int lengt=0;
    if(leng0>0){
        for(i=0;i<leng0;i++){
            strt=table.propertyTable[i];
            strtLen=strt.length();
            if(strtLen>0 && strt[strtLen-1]!=':' && vectorStringIndexOf(confuseExcept,strt)<0){
                lengt++;
            }
        }
        table.propertyConfusedTable.resize(leng0);
        numArr.resize(lengt);
        for(i=0;i<lengt;i++){
            numArr[i]=varNumberBegin+i;
        }
        for(i=0;i+1<lengt;i++){
            j=i+rand()%(lengt-i);
            if(j!=i){
                temp=numArr[j];
                numArr[j]=numArr[i];
                numArr[i]=temp;
            }
        }
        j=0;
        for(i=0;i<leng0;i++){
            strt=table.propertyTable[i];
            strtLen=strt.length();
            if(strtLen>0 && strt[strtLen-1]!=':' && j<lengt && vectorStringIndexOf(confuseExcept,strt)<0){
                snprintf(ostr,127,"%s%d",varStrHead.c_str(),numArr[j]);
                table.propertyConfusedTable[i]=ostr;
                j++;
            }else{
                table.propertyConfusedTable[i]=table.propertyTable[i];
            }
        }
    }



    leng0=table.methodTable.size();
    lengt=0;
    if(leng0>0){
        for(i=0;i<leng0;i++){
            strt=table.methodTable[i];
            strtLen=strt.length();
            if(strtLen>0 && strt[strtLen-1]!=':' && vectorStringIndexOf(confuseExcept,strt)<0){
                lengt++;
            }
        }
        table.methodConfusedTable.resize(leng0);
        numArr.resize(lengt);
        for(i=0;i<lengt;i++){
            numArr[i]=funcNumberBegin+i;
        }
        for(i=0;i+1<lengt;i++){
            j=i+rand()%(lengt-i);
            if(j!=i){
                temp=numArr[j];
                numArr[j]=numArr[i];
                numArr[i]=temp;
            }
        }
        j=0;
        for(i=0;i<leng0;i++){
            strt=table.methodTable[i];
            strtLen=strt.length();
            if(strtLen>0 && strt[strtLen-1]!=':' && j<lengt && vectorStringIndexOf(confuseExcept,strt)<0){
                snprintf(ostr,127,"%s%d",funcStrHead.c_str(),numArr[j]);
                table.methodConfusedTable[i]=ostr;
                j++;
            }else{
                table.methodConfusedTable[i]=table.methodTable[i];
            }
        }
    }
}

string getClassElement(vector<string> &str){
    int i=0;
    int l,m,n;
    int strCount=str.size();
    ostringstream ostr;
    string className="";
    string extendName="";
    for(i=0;i<strCount;i++){
        if(str[i].length()>0x7fff){
            ostr.str("");
            ostr<<"Error:行"<<(i+1)<<"下的文本过长。";
            message(ostr.str());
            //hasError=true;
            continue;
        }
        l=findNext(str[i],"(^|\\s)class ",0,0);
        if(l<0){
            continue;
        }
        l=findNext(str[i],"^\\w+",l,0);
        if(l<0){
            continue;
        }
        m=getFoundPos(l);
        n=getFoundLen(l);
        className.assign(substring(str[i],m,m+n));
        l=findNext(str[i]," extends (\\w+)",l,1);
        if(l>=0){
            m=getFoundPos(l);
            n=getFoundLen(l);
            extendName.assign(substring(str[i],m,m+n));
        }
        break;
    }
    if(extendName!=""){
        return className+":"+extendName;
    }else{
        return className;
    }
}

void getConfusedClasses(vector<string> &classTable0,vector<string> &classTableConfused,string classStrHead){
    int i,j,temp;
    vector<int> numArr;
    int leng0=classTable0.size();
    if(leng0<=0){
        return;
    }
    classTableConfused.resize(leng0);
    int lengt=0;
    string className;
    for(i=0;i<leng0;i++){
        temp=classTable0[i].find(':');
        if(temp<0){
            temp=classTable0[i].length();
        }
        className=substring(classTable0[i],0,temp);
        if(classTable0[i].length()>0 && classTable0[i][0]!='[' && vectorStringIndexOf(confuseExcept,className)<0){
            lengt++;
        }
    }
    numArr.resize(lengt);
    for(i=0;i<lengt;i++){
        numArr[i]=i;
    }
    for(i=0;i+1<lengt;i++){
        j=i+rand()%(lengt-i);
        if(j!=i){
            temp=numArr[j];
            numArr[j]=numArr[i];
            numArr[i]=temp;
        }
    }
    char ostr[128];
    j=0;
    for(i=0;i<leng0;i++){
        temp=classTable0[i].find(':');
        if(temp<0){
            temp=classTable0[i].length();
        }
        className=substring(classTable0[i],0,temp);
        if(classTable0[i].length()>0 && classTable0[i][0]!='[' && j<lengt && vectorStringIndexOf(confuseExcept,className)<0){
            snprintf(ostr,127,"%s%d",classStrHead.c_str(),numArr[j]);
            classTableConfused[i]=ostr;
            j++;
        }else{
            classTableConfused[i]=classTable0[i];
        }
    }
}

void concatVaribleTableWith(VaribleTable &concated,VaribleTable &concatWith){
    concatVectorStringWith(concated.propertyTable,concatWith.propertyTable);
    concatVectorStringWith(concated.propertyTypeTable,concatWith.propertyTypeTable);
    concatVectorStringWith(concated.methodTable,concatWith.methodTable);
    concatVectorStringWith(concated.methodTypeTable,concatWith.methodTypeTable);
    concatVectorStringWith(concated.propertyConfusedTable,concatWith.propertyConfusedTable);
    concatVectorStringWith(concated.methodConfusedTable,concatWith.methodConfusedTable);
}

void confuse(vector<string> &str,varibleTable &table,vector<varibleTable> &tableFrom,vector<string> &classTable,vector<string> &classConfusedTable,string localVaribleHead,string innerFunctionHead,int braceLevel0=-2,int varNumberBegin=0,int funcNumberBegin=0){
    int i,j,k;
    int l,m,n,p;
    int strCount=str.size();
    int lengt=0;
    ostringstream ostr;
    string className="";
    string extendName="";
    string classNamet="";
    string str0="";
    string strt="";
    //vector<string> foundArr;
    int braceLevel=braceLevel0;
    int bracketLevel=0;
    bool methodPrev=false;
    int infoMax=10;
    int infoCount=0;

    string functionName="";
    int functionBegin=-1;
    vector<string> fstr;
    string classHead="{";


    for(i=0;i<strCount;i++){
        if(str[i].length()>0x7fff){
            ostr.str("");
            ostr<<"Error:行"<<(i+1)<<"下的文本过长。";
            message(ostr.str());
            //hasError=true;
            continue;
        }
        str0=str[i];
        l=findNext(str[i],"(^|\\s)class ",0,0);
        if(l>=0){
            l=findNext(str[i],"^(\\w+|\\[\\w*\\])",l,0);
            if(l>=0){
                m=getFoundPos(l);
                n=getFoundLen(l);
                className.assign(substring(str[i],m,m+n));
                p=vectorStringIndexOf(classTable,className);
                if(p>=0 && p<(int)(classConfusedTable.size())){
                    str[i]=substring(str[i],0,m)+classConfusedTable[p]+substring(str[i],m+n);
                }

                l=findNext(str[i]," extends (\\w+)",0,1);
                classHead=str0;
            }

            if(l>=0){
                m=getFoundPos(l);
                n=getFoundLen(l);
                extendName.assign(substring(str[i],m,m+n));
                p=vectorStringIndexOf(classTable,extendName);
                if(p>=0 && p<(int)(classConfusedTable.size())){
                    str[i]=substring(str[i],0,m)+classConfusedTable[p]+substring(str[i],m+n);
                }
            }
        }
        if(braceLevel==0){
            l=findNext(str[i],"(^|\\s)(function) (\\w+)[(]",0,3);
            if(l<0){
                l=findNext(str[i],"(^|\\s)(function) (set|get) (\\w+)[(]",0,4);
            }
            if(l>=0){
                m=getFoundPos(l);
                n=getFoundLen(l);
                functionName=substring(str[i],m,m+n);
                functionBegin=i;
            }
        }
        if(functionBegin<0 && braceLevel>=0 && substring(str[i],0,7)!="import "){
            l=findNext(str[i],"(^|[^@.A-Za-z0-9_])(\\w+)[^#{]",0,2);
            while(l>=0){
                m=getFoundPos(l);
                n=getFoundLen(l);
                strt=substring(str[i],m,m+n);
                if(m+n<(int)(str[i].length()) && str[i][m+n]=='('){
                    methodPrev=true;
                }else{
                    methodPrev=false;
                }


                bracketLevel=0;
                classNamet="";
                p=-1;
                if(strt=="this"){
                    classNamet=className;
                }else if(strt=="super"){
                    classNamet=extendName;
                }else if(methodPrev && m>=5 && (substring(str[i],m-4,m)=="new ") &&
                        (m-5<(int)(str[i].length()) && !(str[i][m-5]>='A' && str[i][m-5]<='Z')
                       && !(str[i][m-5]>='a' && str[i][m-5]<='z') && !(str[i][m-5]>='0' && str[i][m-5]<='9') &&
                        str[i][m-5]!='_')){
                    k=vectorStringIndexOf(classTable,strt);
                    if(k>=0 && k<(int)(classConfusedTable.size())){
                        str[i]=substring(str[i],0,m)+classConfusedTable[k]+substring(str[i],m+n);
                        n=classConfusedTable[k].length();
                        l=n<<16|m;
                    }
                    if(str[i][m-5]=='('){
                        bracketLevel=1;
                        classNamet=strt;
                    }else{
                        p=-1;
                    }
                    methodPrev=false;
                }else if(vectorStringIndexOf(classTable,strt)>=0){
                      k=vectorStringIndexOf(classTable,strt);
                      if(k>=0 && k<(int)(classConfusedTable.size())){
                          str[i]=substring(str[i],0,m)+classConfusedTable[k]+substring(str[i],m+n);
                          n=classConfusedTable[k].length();
                          l=n<<16|m;
                          classNamet=strt;
                      }
                }else if(!methodPrev){
                    p=vectorStringIndexOf(table.propertyTable,strt);
                    if(p>=0 && p<(int)(table.propertyConfusedTable.size())){
                        str[i]=substring(str[i],0,m)+table.propertyConfusedTable[p]+substring(str[i],m+n);
                        n=table.propertyConfusedTable[p].length();
                        l=n<<16|m;
                    }else{
                        p=vectorStringIndexOf(table.methodTable,strt);
                        if(p>=0 && p<(int)(table.methodConfusedTable.size())){
                            str[i]=substring(str[i],0,m)+table.methodConfusedTable[p]+substring(str[i],m+n);
                            n=table.methodConfusedTable[p].length();
                            l=n<<16|m;
                            p=-1;
                        }
                    }
                }else{
                    p=vectorStringIndexOf(table.methodTable,strt);
                    if(p>=0 && p<(int)(table.methodConfusedTable.size())){
                        str[i]=substring(str[i],0,m)+table.methodConfusedTable[p]+substring(str[i],m+n);
                        n=table.methodConfusedTable[p].length();
                        l=n<<16|m;
                    }else{
                        p=vectorStringIndexOf(table.propertyTable,strt);
                        if(p>=0 && p<(int)(table.propertyConfusedTable.size())){
                            str[i]=substring(str[i],0,m)+table.propertyConfusedTable[p]+substring(str[i],m+n);
                            n=table.propertyConfusedTable[p].length();
                            l=n<<16|m;
                            p=-1;
                        }
                    }

                }

                //bracketLevel=0;
                while(m+n<(int)(str[i].length())){
                    if(str[i][m+n]==']'){
                        if(classNamet=="" && !methodPrev && p>=0 && p<(int)(table.propertyTypeTable.size())){
                            classNamet=table.propertyTypeTable[p];
                        }else if(classNamet=="" && methodPrev && p>=0 && p<(int)(table.methodTypeTable.size())){
                            classNamet=table.methodTypeTable[p];
                        }
                        if(classNamet!=""
                           && substring(classNamet,0,8)=="Vector.<"
                           && substring(classNamet,(int)(classNamet.length())-1)==">"){
                            classNamet=substring(classNamet,8,(int)(classNamet.length())-1);
                        }else{
                            classNamet="";
                        }
                        p=-1;
                    }
                    if(str[i][m+n]=='(' || str[i][m+n]=='['){
                        bracketLevel++;
                    }
                    if(str[i][m+n]==')' || str[i][m+n]==']'){
                        bracketLevel--;
                        if(bracketLevel==0 && !(m+n+1<(int)(str[i].length()) && str[i][m+n+1]=='[')){
                            n++;
                            break;
                        }
                    }else if(bracketLevel==0){
                        break;
                    }
                    n++;
                }

                if(substring(str[i],m+n,m+n+4)==" as " || substring(str[i],m+n,m+n+3)=="as "){
                    j=n<<16|m;
                    j=findNext(str[i],"as (\\w+)\\)",j,1);
                    if(j>=0){
                        strt=substring(str[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
                        n=getFoundPos(j)+getFoundLen(j)+1-m;
                        j=vectorStringIndexOf(classTable,strt);
                        if(j>=0 && j<(int)(classConfusedTable.size())){
                            classNamet=strt;
                        }
                    }
                }
                j=n<<16|m;
                k=-1;
                while((p>=0 || classNamet!="") && m+n<(int)(str[i].length()) && str[i][m+n]=='.'){
                    j=findNext(str[i],"^\\.(\\w+)",j,1);
                    if(j<0){
                        break;
                    }
                    m=getFoundPos(j);
                    n=getFoundLen(j);
                    if(classNamet!=""){
                        if(substring(classNamet,0,8)=="Vector.<" &&
                           substring(classNamet,(int)(classNamet.length())-1)==">"){
                            classNamet="Vector";
                        }
                        strt=classNamet;
                        classNamet="";
                    }else if(!methodPrev){
                        if(k<0 && p<(int)(table.propertyTypeTable.size())){
                            strt=table.propertyTypeTable[p];
                        }else if(k>=0 && k<(int)(tableFrom.size()) && p<(int)(tableFrom[k].propertyTypeTable.size())){
                            strt=tableFrom[k].propertyTypeTable[p];
                        }else{
                            strt="";
                        }
                    }else{
                        if(k<0 && p<(int)(table.methodTypeTable.size())){
                            strt=table.methodTypeTable[p];
                        }else if(k>=0 && k<(int)(tableFrom.size()) && p<(int)(tableFrom[k].methodTypeTable.size())){
                            strt=tableFrom[k].methodTypeTable[p];
                        }else{
                            strt="";
                        }
                    }

                    if(strt!=""){
                        k=vectorStringIndexOf(classTable,strt);
                    }else{
                        k=-1;
                        if(infoCount<infoMax){
                            ostr.str("");
                            ostr<<"Info:存在无数据类型变量访问的属性或方法，在第"<<(i+1)<<"行。("<<str0<<")";
                            message(ostr.str());
                        }else if(infoCount==infoMax){
                            message("...");
                        }
                        infoCount++;
                    }
                    if(k<0 || k>=(int)(tableFrom.size())){
                        break;
                    }

                    if(m+n<0 || m+n>=(int)(str[i].length())){
                        break;
                    }
                    strt=substring(str[i],m,m+n);
                    if(str[i][m+n]!='('){
                        p=vectorStringIndexOf(tableFrom[k].propertyTable,strt);
                        if(p>=0 && p<(int)(tableFrom[k].propertyConfusedTable.size())){
                            str[i]=substring(str[i],0,m)+tableFrom[k].propertyConfusedTable[p]+substring(str[i],m+n);
                            n=tableFrom[k].propertyConfusedTable[p].length();
                            methodPrev=false;
                        }else{
                            p=vectorStringIndexOf(tableFrom[k].methodTable,strt);
                            if(p>=0 && p<(int)(tableFrom[k].methodConfusedTable.size())){
                                str[i]=substring(str[i],0,m)+tableFrom[k].methodConfusedTable[p]+substring(str[i],m+n);
                                n=tableFrom[k].methodConfusedTable[p].length();
                                methodPrev=false;
                                p=-1;
                            }
                        }
                    }else{
                        p=vectorStringIndexOf(tableFrom[k].methodTable,strt);
                        if(p>=0 && p<(int)(tableFrom[k].methodConfusedTable.size())){
                            str[i]=substring(str[i],0,m)+tableFrom[k].methodConfusedTable[p]+substring(str[i],m+n);
                            n=tableFrom[k].methodConfusedTable[p].length();
                            methodPrev=true;
                        }else{
                            p=vectorStringIndexOf(tableFrom[k].propertyTable,strt);
                            if(p>=0 && p<(int)(tableFrom[k].propertyConfusedTable.size())){
                                str[i]=substring(str[i],0,m)+tableFrom[k].propertyConfusedTable[p]+substring(str[i],m+n);
                                n=tableFrom[k].propertyConfusedTable[p].length();
                                methodPrev=true;
                                p=-1;
                            }
                        }
                    }
                    bracketLevel=0;
                    while(m+n<(int)(str[i].length())){
                        if(str[i][m+n]==']'){
                            if(classNamet=="" && !methodPrev && p>=0 && p<(int)(table.propertyTypeTable.size())){
                                classNamet=table.propertyTypeTable[p];
                            }else if(classNamet=="" && methodPrev && p>=0 && p<(int)(table.methodTypeTable.size())){
                                classNamet=table.methodTypeTable[p];
                            }
                            if(classNamet!=""
                               && substring(classNamet,0,8)=="Vector.<"
                               && substring(classNamet,(int)(classNamet.length())-1)==">"){
                                classNamet=substring(classNamet,8,(int)(classNamet.length())-1);
                            }else{
                                classNamet="";
                            }
                            p=-1;
                        }
                        if(str[i][m+n]=='(' || str[i][m+n]=='['){
                            bracketLevel++;
                        }
                        if(str[i][m+n]==')' || str[i][m+n]==']'){
                            bracketLevel--;
                            if(bracketLevel==0 && !(m+n+1<(int)(str[i].length()) && str[i][m+n+1]=='[')){
                                n++;
                                break;
                            }
                        }else if(bracketLevel==0){
                            break;
                        }
                        n++;
                    }
                    if(substring(str[i],m+n,m+n+4)==" as " || substring(str[i],m+n,m+n+3)=="as "){
                        j=n<<16|m;
                        j=findNext(str[i],"as (\\w+)\\)",j,1);
                        if(j>=0){
                            strt=substring(str[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
                            n=getFoundPos(j)+getFoundLen(j)+1-m;
                            j=vectorStringIndexOf(classTable,strt);
                            if(j>=0 && j<(int)(classConfusedTable.size())){
                                classNamet=strt;
                            }
                        }
                    }
                    j=n<<16|m;
                }

                l=findNext(str[i],"[^@.A-Za-z0-9_](\\w+)[^#{]",l,1);
            }
        }


        lengt=str[i].length();
        for(j=0;j<lengt;j++){
            if(str[i][j]=='{'){
                braceLevel++;
            }
            if(str[i][j]=='}'){
                braceLevel--;
            }
        }

        if(functionBegin>=0 && braceLevel==0){

            fstr.clear();
            j=functionBegin;

            k=str[j].find('(');
            if(k>=0){
                p=str[j].rfind(' ',k);
                strt=substring(str[j],p+1,k);
                if(strt==className){
                    l=vectorStringIndexOf(classTable,strt);
                    if(l>=0 && l<(int)(classConfusedTable.size())){
                        str[j]=substring(str[j],0,p+1)+classConfusedTable[l]+substring(str[j],k);
                        k=str[j].find('(');
                    }
                }else if(substring(str[j],p-4,p+1)==" get " || substring(str[j],p-4,p+1)==" set "){
                    l=vectorStringIndexOf(table.propertyTable,strt);
                    if(l>=0 && l<(int)(table.propertyConfusedTable.size())){
                        str[j]=substring(str[j],0,p+1)+table.propertyConfusedTable[l]+substring(str[j],k);
                        k=str[j].find('(');
                    }
                }else{
                    l=vectorStringIndexOf(table.methodTable,strt);
                    if(l>=0 && l<(int)(table.methodConfusedTable.size())){
                        str[j]=substring(str[j],0,p+1)+table.methodConfusedTable[l]+substring(str[j],k);
                        k=str[j].find('(');
                    }
                }
            }


            l=str[j].rfind(')');

            if(l>=0 && l+1<(int)(str[j].length()) && str[j][l+1]==':'){
                m=l+2;
                n=(int)(str[j].length())-1-m;
                strt=substring(str[j],m,m+n);
                p=vectorStringIndexOf(classTable,strt);
                if(p>=0 && p<(int)(classConfusedTable.size())){
                    str[j]=substring(str[j],0,m)+classConfusedTable[p]+substring(str[j],m+n);
                }
            }

            fstr.push_back(classHead);
            if(k>=0 && l>=0 && k+1<l){
                strt="var "+substring(str[j],k+1,l)+";";
                fstr.push_back(strt);
            }else{
                fstr.push_back("");
            }
            for(j=functionBegin+1;j<i;j++){
                fstr.push_back(str[j]);
            }
            p=str[i].rfind('}');
            if(p>0){
                strt=substring(str[i],0,p);
                fstr.push_back(strt);
                fstr.push_back("}");
            }else{
                fstr.push_back("");
                fstr.push_back("}");
            }


            VaribleTable fvtable;
            getVaribleTable(fstr,fvtable,-1);
            m=fvtable.propertyTable.size();
            n=fvtable.methodTable.size();
            if(localVaribleHead!="" && innerFunctionHead!=""){
                getConfusedVaribles(fvtable,localVaribleHead,innerFunctionHead,varNumberBegin,funcNumberBegin);
            }else{
                copyVectorString(fvtable.propertyTable,fvtable.propertyConfusedTable);
                copyVectorString(fvtable.methodTable,fvtable.methodConfusedTable);
            }
            concatVaribleTableWith(fvtable,table);
            confuse(fstr,fvtable,tableFrom,classTable,classConfusedTable,localVaribleHead,innerFunctionHead,-1,varNumberBegin+m,funcNumberBegin+n);

            j=functionBegin;
            strt=substring(fstr[1],4,(int)(fstr[1].length())-1);
            //k=str[j].find('(');
            //l=str[j].rfind(')');
            str[j]=substring(str[j],0,k+1)+strt+substring(str[j],l);
            for(j=functionBegin+1;j<=i;j++){
                if(j-functionBegin+1>=1 && j-functionBegin+1<(int)(fstr.size()-1)){
                    str[j]=fstr[j-functionBegin+1];
                }
            }
            if(i>functionBegin){
                str[i]+="}";
            }

            functionBegin=-1;

        }
    }
}

bool attachConstTable(vector<string> &str,ConstTable &table,bool withAlign=true,bool withComment=false,bool leftBraceWrap=false,int braceLevel0=0){
    unsigned int i,j;
    int k,l,m,n;
    unsigned int lengt=0;
    string strt="";
    char ctype=0;
    string numStr="";
    int num=0;
    ostringstream ostr;
    //bool willAdd=true;
    string alignStr="";
    //bool isCommentEnd=false;
    bool hasError=false;
    bool leftCommentMode=false;
    bool rowCommentMode=false;

    i=0;
    k=braceLevel0;
    while(i<str.size()){
        if(str[i].length()>0x7fff){
            ostr.str("");
            ostr<<"Error:行"<<(i+1)<<"下的文本过长。";
            message(ostr.str());
            hasError=true;
            i++;
            continue;
        }

        if(i+1<str.size()){
            if(str[i]=="}" && (
                  (!leftBraceWrap && substring(str[i+1],0,4)=="else")
               || (!leftBraceWrap && substring(str[i+1],0,5)=="catch")
               || (!leftBraceWrap && substring(str[i+1],0,7)=="finally")
               || (substring(str[i+1],0,5)=="while" && substring(str[i+1],(int)(str[i+1].length())-1)!="{"))){
                str[i]+=str[i+1];
                str.erase(str.begin()+(i+1));
                moveComment(table,i,-1);
                continue;
            }else if(substring(str[i],0,4)=="for(" && substring(str[i],(int)(str[i].length())-1)==";"){
                str[i]+=str[i+1];
                str.erase(str.begin()+(i+1));
                moveComment(table,i,-1);
                continue;
            }
        }

        l=findNext(str[i],"@o[0-9]+#",0,0);
        while(l>=0){
            m=getFoundPos(l);
            n=getFoundLen(l);
            strt=substring(str[i],m,m+n);
            if((int)(strt.length())<4){
                ostr.str("");
                ostr<<"Error:解析失败。("<<str[i]<<")";
                message(ostr.str());
                hasError=true;
                break;
            }
            ctype=strt[1];
            numStr=substring(strt,2,(int)(strt.length())-1);
            num=atoi(numStr.c_str());
            if(num>=0 && num<(int)(table.objectTable.size())){
                str[i]=substring(str[i],0,m)+table.objectTable[num]+substring(str[i],m+n);
                n=table.objectTable[num].length();
                l=n<<16|m;
            }else{
                ostr.str("");
                ostr<<"Error:解析失败。("<<str[i]<<")";
                message(ostr.str());
                hasError=true;
                break;
            }
            l=findNext(str[i],"@o[0-9]+#",l,0);
        }

        l=findNext(str[i],"@[inrs][0-9]+#",0,0);
        while(l>=0){
            m=getFoundPos(l);
            n=getFoundLen(l);
            strt=substring(str[i],m,m+n);
            if((int)(strt.length())<4){
                ostr.str("");
                ostr<<"Error:解析失败。("<<str[i]<<")";
                message(ostr.str());
                break;
            }
            ctype=strt[1];
            numStr=substring(strt,2,(int)(strt.length())-1);
            num=atoi(numStr.c_str());
            if(ctype=='i' && num>=0 && num<(int)(table.intTable.size())){
                str[i]=substring(str[i],0,m)+table.intTable[num]+substring(str[i],m+n);
                n=table.intTable[num].length();
                l=n<<16|m;
            }else if(ctype=='n' && num>=0 && num<(int)(table.numberTable.size())){
                str[i]=substring(str[i],0,m)+table.numberTable[num]+substring(str[i],m+n);
                n=table.numberTable[num].length();
                l=n<<16|m;
            }else if(ctype=='r' && num>=0 && num<(int)(table.regexpTable.size())){
                str[i]=substring(str[i],0,m)+table.regexpTable[num]+substring(str[i],m+n);
                n=table.regexpTable[num].length();
                l=n<<16|m;
            }else if(ctype=='s' && num>=0 && num<(int)(table.stringTable.size())){
                str[i]=substring(str[i],0,m)+table.stringTable[num]+substring(str[i],m+n);
                n=table.stringTable[num].length();
                l=n<<16|m;
            }else{
                ostr.str("");
                ostr<<"Error:解析失败。("<<str[i]<<")";
                message(ostr.str());
                break;
            }
            l=findNext(str[i],"@[inrs][0-9]+#",l,0);
        }

        alignStr="";

        if(withAlign && k>0){
            for(j=0;(int)j<k;j++){
                if((int)j==k-1 && substring(str[i],0,1)=="}"){
                    break;
                }
                alignStr+="\t";
            }
            str[i]=alignStr+str[i];
        }
        if(leftBraceWrap){
            stringReplace(str[i],"{","\n"+alignStr+"{");
        }

        lengt=str[i].length();
        for(j=0;j<lengt;j++){
            if(str[i][j]=='{'){
                k++;
            }
            if(str[i][j]=='}'){
                k--;                
            }
        }

        if(withComment && str[i].length()>0 && str[i][str[i].length()-1]=='{'){
            alignStr+="\t";
        }
        if(withComment){
            leftCommentMode=true;
            rowCommentMode=false;
            for(j=0;j<table.commentTable.size() && j<table.commentPosTable.size();j++){
                if(table.commentPosTable[j]<(int)i){
                    continue;
                }
                if(table.commentPosTable[j]>(int)i){
                    break;
                }
                strt=table.commentTable[j];
                if(leftCommentMode && strt.length()>=2 && strt[0]=='/' && strt[1]=='/'){
                    leftCommentMode=false;
                    rowCommentMode=true;
                }else if(strt.length()>=2 && strt[0]=='/' && strt[1]=='*'){
                    leftCommentMode=false;
                    rowCommentMode=false;
                }
                lengt=str[i].length();
                if(leftCommentMode){
                    str[i]=table.commentTable[j]+"\n"+str[i];
                }else if(rowCommentMode && lengt>=1 && str[i][lengt-1]!='{'){
                    str[i]+=table.commentTable[j];
                }else if(strt.length()>=2 && strt[0]=='/' && (strt[1]=='/' || strt[1]=='*')){
                    str[i]+="\n"+alignStr+table.commentTable[j];
                }else{
                    str[i]+="\n"+table.commentTable[j];
                }
                if(rowCommentMode){
                    rowCommentMode=false;
                }
            }
        }

        i++;
    }

    i=0;
    while(i<str.size()){
        l=str[i].find('\n');
        if(l>=0){
            strt=substring(str[i],l+1);
            str.insert(str.begin()+(i+1),strt);
            str[i]=substring(str[i],0,l);
        }
        i++;
    }
    return !hasError;
}

string getFrameCodes(vector<string> &str0,vector<int> &posArray,vector<string> &codeArray,constTable &ctable,varibleTable &vtable){
    posArray.clear();
    codeArray.clear();
    unsigned int i,j;
    int k,l,m,n;
    string strt;
    string numStr;
    ostringstream ostr;
    unsigned int strCount=str0.size();
    bool inScript=false;
    string className="[]",extendName="";
    for(i=0;i<strCount;i++){
        if(str0[i].length()>0x7fff){
            ostr.str("");
            ostr<<"Error:xml文档中，行"<<(i+1)<<"下的文本过长。("<<str0[i]<<")";
            message(ostr.str());
            continue;
        }
        if(i==0 && substring(str0[i],0,15)=="<DOMSymbolItem "){
            l=findNext(str0[i]," linkageClassName=\\\"(\\w+)\\\"",0,1);
            if(l>=0){
                m=getFoundPos(l);
                n=getFoundLen(l);
                className=substring(str0[i],m,m+n);
            }else{
                l=findNext(str0[i]," name=\\\"(\\w+)\\\"",0,1);
                if(l>=0){
                    m=getFoundPos(l);
                    n=getFoundLen(l);
                    className="["+substring(str0[i],m,m+n)+"]";
                }
            }
            l=findNext(str0[i]," linkageBaseClass=\\\"(\\w+)\\\"",0,1);
            if(l>=0){
                m=getFoundPos(l);
                n=getFoundLen(l);
                strt=substring(str0[i],m,m+n);
                //if(strt.find('.')==string::npos){
                    extendName=strt;
                //}
            }
        }
        if(!inScript){
            l=str0[i].find("<Actionscript>");
            if(l>=0 && i+1<strCount){
                l=findNext(str0[i+1],"<script>\\s*<\\!\\[CDATA\\[");
                if(l<0){
                    l=findNext(str0[i],"<script>\\s*<\\!\\[CDATA\\[");
                    k=i;
                }else{
                    k=i+1;
                }
                if(l>=0){
                    m=getFoundPos(l);
                    n=m+getFoundLen(l);
                    posArray.push_back(k);
                    posArray.push_back(n);
                    inScript=true;
                }
            }
        }

        if(inScript){
            l=str0[i].find("</Actionscript>");
            if(l>=0 && i>=1){
                l=findNext(str0[i-1],"\\]\\]>\\s*<\\/script>");
                if(l<0){
                    l=findNext(str0[i],"\\]\\]>\\s*<\\/script>");
                    k=i;
                }else{
                    k=i-1;
                }
                if(l>=0){
                    m=getFoundPos(l);
                    n=m+getFoundLen(l);
                    posArray.push_back(k);
                    posArray.push_back(m);
                    inScript=false;
                }
            }
        }
    }
    if(posArray.size()==0){
        return extendName==""?(className):(className+":"+extendName);
    }
    string classHead="public class "+className;
    if(extendName!=""){
        classHead+=" extends "+extendName;
    }
    classHead+="{";
    codeArray.push_back(classHead);
    for(i=0;i+3<posArray.size();i+=4){
        m=posArray[i];
        n=posArray[i+2];
        for(j=(unsigned int)m;j<=(unsigned int)n;j++){
            strt=str0[j];
            if(j==(unsigned int)n){
                strt=substring(strt,0,posArray[i+3]);
            }
            if(j==(unsigned int)m){
                strt=substring(strt,posArray[i+1]);
            }
            codeArray.push_back(strt);
        }
        ostr.str("");
        ostr<<"@_"<<(i>>2)<<"#;";
        numStr=ostr.str();
        codeArray.push_back(numStr);
    }
    codeArray.push_back("}");
    writeInfoTableAndReplace(codeArray,ctable);
    recombineStatements(codeArray,ctable);
    if(codeArray.size()>0){
        codeArray[0]=classHead;
    }
    getVaribleTable(codeArray,vtable,-1);


    return extendName==""?(className):(className+":"+extendName);

}

void attachFrameScript(vector<string> &str0,vector<string> &codeArray,vector<int> &posArray,constTable &ctable,bool withAlign=true,bool withComment=false,bool leftBraceWrap=false){

    int i,j,k,l,m,n;
    int iprev=1;
    int movement=0;
    string strHead="",strEnd="";
    string className="[]",extendName="";
    if(str0.size()>0 && codeArray.size()>0 && substring(str0[0],0,15)=="<DOMSymbolItem "){

        l=findNext(codeArray[0],"class (\\w+|\\[\\w*\\])",0,1);
        if(l>=0){
            m=getFoundPos(l);
            n=getFoundLen(l);
            className=substring(codeArray[0],m,m+n);
            l=findNext(codeArray[0]," extends (\\w+)",l,1);

            if(l>=0){
                m=getFoundPos(l);
                n=getFoundLen(l);
                extendName=substring(codeArray[0],m,m+n);
            }
            //message(className);
            //message(extendName);
            if(className.length()>0 && className[0]!='['){
                replaceAll(str0[0]," linkageClassName=\\\"\\w+\\\""," linkageClassName=\""+className+"\"");
            }
            if(extendName!=""){
                replaceAll(str0[0]," linkageBaseClass=\\\"\\w+\\\""," linkageBaseClass=\""+extendName+"\"");
            }
        }
    }
    attachConstTable(codeArray,ctable,withAlign,withComment,leftBraceWrap,-1);
    iprev=1;
    j=0;



    for(i=1;i+1<(int)(codeArray.size());i++){
        if(substring(codeArray[i],0,2)=="@_"){

            if(j+3>=(int)(posArray.size())){
                break;
            }
            movement=(i-iprev)-(posArray[j+2]-posArray[j]+1);

            for(k=j+4;k+3<(int)(posArray.size());k+=4){
                posArray[k]+=movement;
                posArray[k+2]+=movement;
            }

            strHead=substring(str0[posArray[j]],0,posArray[j+1]);
            strEnd=substring(str0[posArray[j+2]],posArray[j+3]);

            if(movement>0){
                str0.insert(str0.begin()+posArray[j+2],movement,"");
            }else if(movement<0){
                str0.erase(str0.begin()+posArray[j+2]+movement,str0.begin()+posArray[j+2]);
            }



            posArray[j+2]+=movement;
            l=posArray[j];
            for(k=iprev;k<i;k++){
                if(l<(int)(str0.size())){
                    str0[l]=codeArray[k];
                    if(k==iprev){
                        str0[l]=strHead+str0[l];
                    }
                    if(k+1==i){
                        str0[l]+=strEnd;
                    }
                    l++;
                }
            }
            j+=4;
            iprev=i+1;
        }
    }
}

void getMovieVaribles(vector<string> &str,VaribleTable &vtable){
    unsigned int i=0;
    int j=0;
    unsigned int strCount=str.size();
    string namet="";
    string typet="";
    for(i=0;i<strCount;i++){
        j=findNext(str[i],"<DOMSymbolInstance( .* | )name\\=\\\"(\\w+)\\\"",0,2);
        if(j>=0){
            namet=substring(str[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
            if(vectorStringIndexOf(vtable.propertyTable,namet)>=0){
                continue;
            }
            vtable.propertyTable.push_back(namet);
            if(findNext(str[i]," symbolType\\=\\\"button\\\"",0,0)>=0){
                vtable.propertyTypeTable.push_back("SimpleButton");
            }else{
                vtable.propertyTypeTable.push_back("MovieClip");
            }
            continue;
        }

        j=findNext(str[i],"<DOMDynamicText( .* | )name\\=\\\"(\\w+)\\\"",0,2);
        if(j>=0){
            namet=substring(str[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
            if(vectorStringIndexOf(vtable.propertyTable,namet)<0){
                vtable.propertyTable.push_back(namet);
                vtable.propertyTypeTable.push_back("TextField");
            }
            continue;
        }

        j=findNext(str[i],"<DOMInputText( .* | )name\\=\\\"(\\w+)\\\"",0,2);
        if(j>=0){
            namet=substring(str[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
            if(vectorStringIndexOf(vtable.propertyTable,namet)<0){
                vtable.propertyTable.push_back(namet);
                vtable.propertyTypeTable.push_back("TextField");
            }
            continue;
        }

        j=findNext(str[i],"<DOMComponentInstance( .* | )name\\=\\\"(\\w+)\\\"",0,2);
        if(j>=0){
            namet=substring(str[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
            if(vectorStringIndexOf(vtable.propertyTable,namet)>=0){
                continue;
            }
            j=findNext(str[i]," libraryItemName=\"(\\w+)\"",0,1);
            //message(namet);
            if(j<0){
                continue;
            }
            typet=substring(str[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
            vtable.propertyTable.push_back(namet);
            vtable.propertyTypeTable.push_back(typet);
            continue;
        }
    }
}


int readFileInfo(string filePathName,vector<string> &fileStringArray,ConstTable &ctable,VaribleTable &vtable,int braceLevel0=-2){
    int openRet=openFile(filePathName,fileStringArray);

    ostringstream ostr;
    if(openRet==-1){
        ostr.str("");
        ostr<<"打开文件失败。("<<filePathName<<")";
        message(ostr.str());
        transferring=false;
        return openRet;
    }
    writeInfoTableAndReplace(fileStringArray,ctable);
    recombineStatements(fileStringArray,ctable);
    getVaribleTable(fileStringArray,vtable,braceLevel0);

    return openRet;
}



void getFilePathArrayAtSameDir(vector<string> &pathNameArr,string pathName0,const char *filterStr=NULL){
    pathNameArr.clear();
    int i=pathName0.rfind('/');
    if(i<0){
        return;
    }
    string path0=substring(pathName0,0,i+1);
    string path=_t(path0);
    QDir *dir=new QDir(path.c_str());
    QStringList filter;
    bool dirMode=(filterStr!=NULL && strcmp(filterStr,"<dir>")==0);
    if(filterStr!=NULL && !dirMode){
        filter<<filterStr;
        dir->setNameFilters(filter);
    }
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    int count=fileInfo->count();
    string strt;
    for(i=0;i<count;i++){
        strt=fileInfo->at(i).absoluteFilePath().toLocal8Bit().data();
        if(fileInfo->at(i).isDir()){
            strt+="/";
        }
        if(strt.length()>path0.length() && (!dirMode || fileInfo->at(i).isDir())){
            pathNameArr.push_back(strt);
            //message(_t(strt));
        }
    }
    delete fileInfo;
}

void getFilePathArrayInDir(vector<string> &pathNameArr,string pathName0,const char *filterStr=NULL){
    //pathNameArr.clear();
    int i=pathName0.rfind('/');
    if(i<0){
        return;
    }
    string path0=substring(pathName0,0,i+1);
    string path=_t(path0);
    QDir *dir=new QDir(path.c_str());
    QStringList filter;
    bool dirMode=(filterStr!=NULL && strcmp(filterStr,"<dir>")==0);
    if(filterStr!=NULL && !dirMode){
        filter<<filterStr;
        dir->setNameFilters(filter);
    }
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    int count=fileInfo->count();
    string strt;
    for(i=0;i<count;i++){
        strt=fileInfo->at(i).absoluteFilePath().toLocal8Bit().data();
        if(fileInfo->at(i).isDir()){
            strt+="/";
            if(strt.length()>path0.length() && dirMode){
                pathNameArr.push_back(strt);
                //message(_t(strt));
            }
            if(strt.length()>path0.length()){
                getFilePathArrayInDir(pathNameArr,strt,filterStr);
            }
        }else if(strt.length()>path0.length() && !dirMode){
            pathNameArr.push_back(strt);
            //message(_t(strt));
        }
    }
    delete fileInfo;
}

void copyFile(string fromDir ,string toDir,bool coverFileIfExist=true){

    if (fromDir==toDir){
        return;
    }
    mkdirs(toDir.c_str());
    QString qfromDir=_t(fromDir).c_str();
    QString qtoDir=_t(toDir).c_str();

    if (!QFile::exists(qfromDir)){
        return;
    }

    QDir *dir=new QDir;
    bool exist=dir->exists(qtoDir);
    if(exist && coverFileIfExist){
        dir->remove(qtoDir);
    }

    QFile::copy(qfromDir, qtoDir);
    delete dir;
}

string getFileNameNotExist(string path0,bool dirMode=false){
    int i,j,n;
    string patht=path0;

    i=patht.rfind('/');
    string strt="";
    if(i<0){
        return "";
    }
    string destRoot="";
    string destAddition="";
    if(dirMode){
        j=patht.rfind('/',i-1);
        if(j<0){
            return "";
        }
        strt=substring(patht,j+1,i);
        destRoot=substring(patht,0,j+1);
        destAddition=substring(patht,i);
    }else{
        strt=substring(patht,i+1);
        destRoot=substring(patht,0,i+1);
        destAddition="";
    }
    string strn="";
    patht=destRoot+strt;
    ostringstream ostr;
    while(_access(patht.c_str(),0)==0){
        i=0;
        j=dirMode?(int)(strt.length()):-1;
        n=0;
        if(dirMode && j<=0){
            message(strt);
            return "";
        }

        for(i=(int)(strt.length())-1;i>=0;i--){
            if(strt[i]=='/' || strt[i]=='\\'){
                break;
            }
            if(j<0 && strt[i]=='.'){
                j=i;
                continue;
            }
            if(j>=0 && (strt[i]<'0' || strt[i]>'9')){
                strn=substring(strt,i+1,j);
                n=atoi(strn.c_str())+1;
                ostr.str("");
                ostr<<n;
                strn=ostr.str();
                strt=substring(strt,0,i+1)+strn+substring(strt,j);
                break;
            }
        }
        if(i<0 && j>=0){
            strn=substring(strt,0,j);
            n=atoi(strn.c_str())+1;
            ostr.str("");
            ostr<<n;
            strn=ostr.str();
            strt=strn+substring(strt,j);
        }else if(j<0){
            if(transferMode==0){
                strt+=".js";
            }else if(transferMode==1){
                strt+=".as";
            }else{
                return "";
            }
        }
        patht=destRoot+strt;
    }
    patht=destRoot+strt+destAddition;
    return patht;
}

int readFileInfoAndSave(string filePathName,int braceLevel0=-2){
    vector<string> fileStringArray;
    ConstTable ctable;
    VaribleTable vtable;
    int openRet=openFile(filePathName,fileStringArray);

    ostringstream ostr;
    if(openRet==-1){
        ostr.str("");
        ostr<<"打开文件失败。("<<filePathName<<")";
        message(ostr.str());
        transferring=false;
        return openRet;
    }
    writeInfoTableAndReplace(fileStringArray,ctable);
    recombineStatements(fileStringArray,ctable);
    getVaribleTable(fileStringArray,vtable,braceLevel0);

    bool hasBom=(openRet==1);
    getConfusedVaribles(vtable,"v","f");
    int i;
    ConstTable &table=ctable;
    string filePathNamet=filePathName.substr(0,filePathName.rfind('.'))+"_transferred_.as";
    i=filePathNamet.rfind('/');
    string fileNamet=substring(filePathNamet,i+1);
    filePathNamet=substring(filePathNamet,0,i)+destPostfix+"/test/"+fileNamet;
    if(!overrideFileMode){
        filePathNamet=getFileNameNotExist(filePathNamet,false);
        if(filePathNamet==""){
            message("\n无法获取新的目标路径。");
            return openRet;
        }
    }

    saveFile(filePathNamet,fileStringArray,hasBom);
    FILE *fp=fopen(filePathNamet.c_str(),"a");
    for(i=0;i<(int)(table.commentTable.size());i++){
        ostr.str("");
        ostr<<"@c"<<i<<"#("<<(table.commentPosTable[i]+1)<<")";
        fputs(ostr.str().c_str(),fp);
        fputc('\t',fp);
        fputs(table.commentTable[i].c_str(),fp);
        fputc('\n',fp);
    }
    for(i=0;i<(int)(table.stringTable.size());i++){
        ostr.str("");
        ostr<<"@s"<<i<<"#";
        fputs(ostr.str().c_str(),fp);
        fputc('\t',fp);
        fputs(table.stringTable[i].c_str(),fp);
        fputc('\n',fp);
    }
    for(i=0;i<(int)(table.numberTable.size());i++){
        ostr.str("");
        ostr<<"@n"<<i<<"#";
        fputs(ostr.str().c_str(),fp);
        fputc('\t',fp);
        fputs(table.numberTable[i].c_str(),fp);
        fputc('\n',fp);
    }
    for(i=0;i<(int)(table.intTable.size());i++){
        ostr.str("");
        ostr<<"@i"<<i<<"#";
        fputs(ostr.str().c_str(),fp);
        fputc('\t',fp);
        fputs(table.intTable[i].c_str(),fp);
        fputc('\n',fp);
    }
    for(i=0;i<(int)(table.regexpTable.size());i++){
        ostr.str("");
        ostr<<"@r"<<i<<"#=";
        fputs(ostr.str().c_str(),fp);
        fputc('\t',fp);
        fputs(table.regexpTable[i].c_str(),fp);
        fputc('\n',fp);
    }
    for(i=0;i<(int)(table.objectTable.size());i++){
        ostr.str("");
        ostr<<"@o"<<i<<"#";
        fputs(ostr.str().c_str(),fp);
        fputc('\t',fp);
        fputs(table.objectTable[i].c_str(),fp);
        fputc('\n',fp);
    }
    for(i=0;i<(int)(vtable.propertyTable.size()) && i<(int)(vtable.propertyTypeTable.size())
        && i<(int)(vtable.propertyConfusedTable.size());i++){
        fputs("var ",fp);
        fputs(vtable.propertyTable[i].c_str(),fp);
        fputc(' ',fp);
        fputs(vtable.propertyTypeTable[i].c_str(),fp);
        fputc(' ',fp);
        fputs(vtable.propertyConfusedTable[i].c_str(),fp);
        fputc('\n',fp);
    }
    for(i=0;i<(int)(vtable.methodTable.size()) && i<(int)(vtable.methodTypeTable.size())
        && i<(int)(vtable.methodConfusedTable.size());i++){
        fputs("function ",fp);
        fputs(vtable.methodTable[i].c_str(),fp);
        fputc(' ',fp);
        fputs(vtable.methodTypeTable[i].c_str(),fp);
        fputc(' ',fp);
        fputs(vtable.methodConfusedTable[i].c_str(),fp);
        fputc('\n',fp);
    }
    fputs(getClassElement(fileStringArray).c_str(),fp);
    fputc('\n',fp);
    fclose(fp);

    return openRet;
}


unsigned int as3ConfuseTherad(void* param=NULL){
    if(!transferring){
        return 0;
    }
    srand((unsigned)time(NULL));
    int i=0,j=0,k=0,l=0;
    ostringstream ostr;
    //ostr.str("");
    //ostr<<sizeof(string::size_type);
    //message(ostr.str());
    ostr.str("");
    ostr<<"准备转换：("<<_t(flashFileName)<<")。";
    message(ostr.str());

    string extName=substring(flashFileName,(int)(flashFileName.rfind('.')));

    string path0="";
    string parentPath="";
    i=flashFileName.rfind('/');
    path0=substring(flashFileName,0,i);
    i=path0.rfind('/');
    parentPath=substring(path0,0,i);
    if(path0!=""){
        path0+="/";
    }
    if(parentPath!="" && path0!=""){
        parentPath+="/";
    }else if(path0==""){
        ostr.str("");
        ostr<<"输入的路径不支持。("<<_t(flashFileName)<<")";
        message(ostr.str());
        return 0;
    }else if(extName==".xfl"){
        ostr.str("");
        ostr<<"无法定位在根目录下xfl文档的输出路径。("<<_t(flashFileName)<<")";
        message(ostr.str());
        return 0;
    }


    vector<string> pathNameSrc;
    string path;
    string xflPath;
    string patht;

    if(extName==".as"){
        path=path0;
        xflPath="";
        patht=substring(path,0,path.length()-1);
        patht+=destPostfix+"/";
        if(!overrideFileMode){
            patht=getFileNameNotExist(patht,true);
            if(patht==""){
                message("\n无法获取新的目标路径。("+_t(path0)+")");
                return 0;
            }
        }
        getFilePathArrayAtSameDir(pathNameSrc,path,"*.as");
    }else if(extName==".xfl"){
        path=parentPath;
        xflPath=substring(path0,parentPath.length());
        patht=substring(path,0,path.length()-1);
        patht+=destPostfix+"/";
        if(!overrideFileMode){
            patht=getFileNameNotExist(patht,true);
            if(patht==""){
                message("\n无法获取新的目标路径。("+_t(path0)+")");
                return 0;
            }
        }

        getFilePathArrayAtSameDir(pathNameSrc,path,"*.as");
    }
    int fileCount0=pathNameSrc.size();

    vector< vector<string> > file;
    vector< vector<int> > posArray;
    vector<ConstTable> constTable;
    vector<VaribleTable> varibleTable;
    vector<string> classTable;
    vector<string> classNameTable;
    vector<string> classConfusedTable;
    vector<bool> hasBom;
    vector<string> pathNameDest;
    vector<string> xmlPathNameSrc;
    vector< vector<string> > *xmlFile=new vector< vector<string> >;
    string fileNamet;
    string pathNamet;
    string classElement;
    int openRet;
    bool hasBomt;
    string strt;

    for(i=0;i<fileCount0;i++){
        vector<string> str0;
        ConstTable ctable;
        VaribleTable vtable;
        //VaribleTable vtableEmpty;

        j=path.length();
        fileNamet=substring(pathNameSrc[i],j);
        pathNamet=patht+fileNamet;
        ostr.str("");
        ostr<<"\n读取"<<_t(pathNameSrc[i])<<"文件...";
        message(ostr.str());


        openRet=readFileInfo(pathNameSrc[i],str0,ctable,vtable);
        if(openRet==-1){
            continue;
        }
        hasBomt=(openRet==1);


        if(str0.size()==0 || str0[0]!="package{"){
            continue;
        }

        classElement=getClassElement(str0);


        j=classElement.find(":");
        if(j>=0){
            strt=substring(classElement,0,j);
        }else{
            strt=classElement;
        }

        if(substring(pathNamet,patht.length(),pathNamet.length()-3)!=strt){
            continue;
        }

        classNameTable.push_back(strt);
        classTable.push_back(classElement);
        file.push_back(str0);
        constTable.push_back(ctable);
        if(vectorStringIndexOf(confuseExcept,strt)<0){
            getConfusedVaribles(vtable,"v","f");
            varibleTable.push_back(vtable);
        }else{
            //varibleTable.push_back(vtableEmpty);
            copyVectorString(vtable.propertyTable,vtable.propertyConfusedTable);
            copyVectorString(vtable.methodTable,vtable.methodConfusedTable);
            varibleTable.push_back(vtable);
        }
        //classConfusedTable.push_back(constTable);
        pathNameDest.push_back(pathNamet);
        hasBom.push_back(hasBomt);
    }

    int asCount=pathNameDest.size();
    vector<string> xflFileList;
    getFilePathArrayInDir(xflFileList,path0,NULL);
    if(extName==".xfl"){
        for(i=0;i<(int)(xflFileList.size());i++){
            strt=substring(xflFileList[i],(int)(xflFileList[i].rfind('.')));
            pathNamet=patht+xflPath+substring(xflFileList[i],path0.length());
            if(strt!=".xml"){
                copyFile(xflFileList[i],pathNamet);
                continue;
            }

            vector<string> xmlstr;
            vector<string> str1;
            ConstTable ctable1;
            VaribleTable vtable1;
            //VaribleTable vtableEmpty1;
            vector<int> posArray1;
            openRet=openFile(xflFileList[i],xmlstr);
            if(openRet==-1){
                continue;
            }

            ostr.str("");
            ostr<<"\n读取"<<_t(xflFileList[i])<<"文件";
            message(ostr.str());

            classElement=getFrameCodes(xmlstr,posArray1,str1,ctable1,vtable1);
            if(posArray1.size()==0){
                copyFile(xflFileList[i],pathNamet);
                continue;
            }
            hasBomt=(openRet==1);
            //xmlPathNameSrc.push_back(xflFileList[i]);
            xmlFile->push_back(xmlstr);
            posArray.push_back(posArray1);
            file.push_back(str1);


            constTable.push_back(ctable1);

            classTable.push_back(classElement);

            j=classElement.find(":");
            if(j>=0){
                strt=substring(classElement,0,j);
            }else{
                strt=classElement;
            }
            classNameTable.push_back(strt);

            if(vectorStringIndexOf(confuseExcept,strt)<0){
                getConfusedVaribles(vtable1,"v","f");
                varibleTable.push_back(vtable1);
            }else{
                //varibleTable.push_back(vtableEmpty1);
                copyVectorString(vtable1.propertyTable,vtable1.propertyConfusedTable);
                copyVectorString(vtable1.methodTable,vtable1.methodConfusedTable);
                varibleTable.push_back(vtable1);
            }

            //classConfusedTable.push_back(constTable);
            pathNameDest.push_back(pathNamet);
            hasBom.push_back(hasBomt);
        }

    }

    int fileCount=pathNameDest.size();

    getConfusedClasses(classTable,classConfusedTable,"c");
    for(i=0;i<fileCount;i++){
        classElement=classTable[i];
        j=classElement.find(":");
        if(j>=0){
            strt=substring(classElement,j+1);
            j=vectorStringIndexOf(classNameTable,strt);
        }
        if(j>=0){
            if(vectorStringIndexOf(confuseExcept,classNameTable[i])<0){
                getConfusedVaribles(varibleTable[i],"v","f",varibleTable[j].propertyTable.size(),varibleTable[j].methodTable.size());
            }
            for(k=0;k<(int)(varibleTable[i].propertyTable.size());k++){
                strt=varibleTable[i].propertyTable[k];
                l=strt.rfind(':');
                if(l>=0){
                    strt=substring(strt,l);
                    l=vectorStringIndexOf(varibleTable[j].propertyTable,strt);
                }
                if(l>=0){
                    varibleTable[i].propertyConfusedTable[k]=varibleTable[j].propertyConfusedTable[l];
                }
            }
            for(k=0;k<(int)(varibleTable[i].methodTable.size());k++){
                strt=varibleTable[i].methodTable[k];
                l=strt.rfind(':');
                if(l>=0){
                    strt=substring(strt,l);
                    l=vectorStringIndexOf(varibleTable[j].methodTable,strt);
                }
                if(l>=0){
                    varibleTable[i].methodConfusedTable[k]=varibleTable[j].methodConfusedTable[l];
                }
            }
            concatVaribleTableWith(varibleTable[i],varibleTable[j]);
        }
    }

    for(i=0;i<asCount;i++){
        if(extName==".xfl"){
            pathNamet=patht+classConfusedTable[i]+".as";
            ostr.str("");
            ostr<<"\n转换"<<_t(pathNamet)<<"文件("<<_t(substring(pathNameDest[i],patht.length()))<<")...";
            message(ostr.str());
        }else{
            pathNamet=pathNameDest[i];
            ostr.str("");
            ostr<<"\n转换"<<_t(pathNamet)<<"文件...";
            message(ostr.str());
        }
        confuse(file[i],varibleTable[i],varibleTable,classNameTable,classConfusedTable,"l","lf",-2);
        attachConstTable(file[i],constTable[i],true,false,false);
        saveFile(pathNamet,file[i],hasBom[i]);
    }

    if(extName==".xfl"){
        for(i=asCount;i<fileCount;i++){
            ostr.str("");
            ostr<<"\n转换"<<_t(pathNameDest[i])<<"文件...";
            message(ostr.str());
            confuse(file[i],varibleTable[i],varibleTable,classNameTable,classConfusedTable,"l","lf",-1);
            /*vector<string> xmlFilet;
            openFile(xmlPathNameSrc[i-asCount],xmlFilet);
            attachFrameScript(xmlFilet,file[i],posArray[i-asCount],constTable[i],true,false,false);
            saveFile(pathNameDest[i],xmlFilet,hasBom[i]);*/
            attachFrameScript(xmlFile->at(i-asCount),file[i],posArray[i-asCount],constTable[i],true,false,false);
            saveFile(pathNameDest[i],xmlFile->at(i-asCount),hasBom[i]);
            //saveFile(pathNameDest[i],file[i],hasBom[i]);
        }
    }

    delete xmlFile;
    message("\n转换完成");
    return 0;
}

unsigned int asToJsThread(void* param=NULL){
    if(!transferring){
        return 0;
    }
    srand((unsigned)time(NULL));
    ostringstream ostr;
    //ostr.str("");
    //ostr<<sizeof(string::size_type);
    //message(ostr.str());
    int i,j,k;

    string extName=substring(flashFileName,(int)(flashFileName.rfind('.')));

    string path0="";
    string parentPath="";
    i=flashFileName.rfind('/');
    path0=substring(flashFileName,0,i);
    i=path0.rfind('/');
    parentPath=substring(path0,0,i);
    if(path0!=""){
        path0+="/";
    }
    if(parentPath!="" && path0!=""){
        parentPath+="/";
    }else if(path0==""){
        ostr.str("");
        ostr<<"输入的路径不支持。("<<_t(flashFileName)<<")";
        message(ostr.str());
        return 0;
    }else if(extName==".xfl"){
        ostr.str("");
        ostr<<"无法定位在根目录下xfl文档的输出路径。("<<_t(flashFileName)<<")";
        message(ostr.str());
        return 0;
    }

    string path;
    string xflPath;
    string patht;

    if(extName==".as"){
        path=path0;
        xflPath="";
        //patht=substring(path,0,path.length()-1);
        //patht+="_transferred/";
        patht=path;

        //getFilePathArrayAtSameDir(pathNameSrc,path,"*.as");
    }else if(extName==".xfl"){
        path=parentPath;
        xflPath=substring(path0,parentPath.length());
        //patht=substring(path,0,path.length()-1);
        //patht+="_transferred/";
        patht=outputInside?path0:path;

        //getFilePathArrayAtSameDir(pathNameSrc,path,"*.as");
    }


    string flashName=substring(flashFileName,path0.length(),flashFileName.length()-extName.length());
    string pathNamet=patht+flashName+destPostfix+".js";
    if(!overrideFileMode){
        pathNamet=getFileNameNotExist(pathNamet,false);
        if(pathNamet==""){
            message("\n无法获取新的目标路径。("+flashFileName+")\n无法转换");
            return 0;
        }
    }

    ostr.str("");
    ostr<<"读取：("<<_t(flashFileName)<<")...";
    message(ostr.str());

    vector<string> str0;
    vector<string> xmlstr;
    ConstTable ctable;
    VaribleTable vtable;
    VaribleTable mvt0;
    vector<int> posArray;
    string classElement="";
    int openRet=-1;
    string strt="";
    if(extName!=".xfl"){
        openRet=openFile(flashFileName,str0);
        if(openRet==-1){
            message("打开文件出错。");
        }
        writeInfoTableAndReplace(str0,ctable);
        recombineStatements(str0,ctable);
        classElement=getClassElement(str0);
        getVaribleTable(str0,vtable,classElement!=""?-2:0);
    }else{
        openRet=openFile(path0+"DOMDocument.xml",xmlstr);
        if(openRet==-1){
            message("打开文件出错。");
            return 0;
        }
        classElement=getFrameCodes(xmlstr,posArray,str0,ctable,vtable);
        if(str0.size()>=2){
            str0.pop_back();
            str0.erase(str0.begin());
        }

        getMovieVaribles(xmlstr,mvt0);
        j=mvt0.propertyTable.size();
        if(j>(int)(mvt0.propertyTypeTable.size())){
            j=mvt0.propertyTypeTable.size();
        }
        if(j>0){
            str0.insert(str0.begin(),1,"@_#");
            for(i=j-1;i>=0;i--){
                strt="var "+mvt0.propertyTable[i]+":"+mvt0.propertyTypeTable[i]+";";
                str0.insert(str0.begin(),1,strt);
            }
            for(i=0;i<(int)(ctable.commentPosTable.size());i++){
                ctable.commentPosTable[i]+=j;
            }
            vtable.propertyTable.clear();
            vtable.propertyTypeTable.clear();
            vtable.methodTable.clear();
            vtable.methodTypeTable.clear();
            getVaribleTable(str0,vtable,0);
        }
    }

    vector<string> macrojs;
    if(!macrojsPath.empty() && openFile(macrojsPath,macrojs)==-1){
        message("\n指定宏定义文件“"+macrojsPath+"”失败");
        message("无法转换");
    }else if(!cMode && openFile(programPathLocal+"macrojs.txt",macrojs)==-1){
        message("\n程序目录下缺少macrojs.txt文件");
        message("无法转换");
        return 0;
    }else if(cMode && openFile(programPathLocal+"macroc.txt",macrojs)==-1){
        message("\n程序目录下缺少macroc.txt文件");
        message("无法转换");
        return 0;
    }
    macrojs.push_back("");
    bool inFunction=false;
    int lengt=0;
    if(as2Mode){
        k=0;
        for(i=0;i<(int)str0.size();i++){
            j=findNext(str0[i],"(^|\\s)function ",0,0);
            if(j>=0){
                inFunction=true;
            }
            if(!inFunction){
                j=findNext(str0[i],"^(\\w+)=[^=]",0,1);
                if(j>=0){
                    strt=substring(str0[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
                    if(vectorStringIndexOf(vtable.propertyTable,strt)<0){
                        str0[i]="var "+str0[i];
                        vtable.propertyTable.push_back(strt);
                        vtable.propertyTypeTable.push_back("");
                    }
                }
            }
            lengt=str0[i].length();
            for(j=0;j<lengt;j++){
                if(str0[i][j]=='{'){
                    k++;
                }
                if(str0[i][j]=='}'){
                    k--;
                }
            }
            if(inFunction && k<=0){
                inFunction=false;
            }
        }
    }
    const int CLASS_ONLY=1;
    const int AS2_ONLY=2;
    const int AFTER_DEFINE=4;
    const int DEFINE=8;
    const int FUNCTION=16;
    const int MEMBER=32;

    vector<string> findArray;
    vector<string> replaceArray;
    vector<string> conditionArray;

    bool hasDefine=false;
    vector<VaribleTable> vtf;
    vector<string> classNameTable;
    vector<string> classReplacedTable;
    string varNameSrc;
    string varNameDest;
    string classNameSrc;
    string classNameDest;

    copyVectorString(vtable.propertyTable,vtable.propertyConfusedTable);
    copyVectorString(vtable.methodTable,vtable.methodConfusedTable);
    vtf.push_back(vtable);
    j=classElement.find(":");
    if(j>=0){
        strt=substring(classElement,0,j);
    }else{
        strt=classElement;
    }
    string className=strt;
    string extendName=j>=0?substring(classElement,j+1):"";
    if(className==""){
        classElement="[]";
        className="[]";
        extendName="";
    }
    bool classMode=!(classElement.length()>0 && classElement[0]=='[');
    classNameTable.push_back(className);
    classReplacedTable.push_back(className);

    string constructorArg="";
    string superConstructorUsedArg="";
    if(classMode){
        strt="(^|\\W)function "+className+"\\((.*)\\)";
        for(i=0;i<(int)(str0.size());i++){
            j=findNext(str0[i],strt,0,2);
            if(j>=0){
                constructorArg=substring(str0[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
                break;
            }
        }

    }
    if(classMode && extendName!=""){
        strt="(^|\\W)super\\((.*)\\)";
        for(i=0;i<(int)(str0.size());i++){
            j=findNext(str0[i],strt,0,2);
            if(j>=0){
                superConstructorUsedArg=substring(str0[i],getFoundPos(j),getFoundPos(j)+getFoundLen(j));
                break;
            }
        }
    }

    vector<string> findArrayAfterDefine;
    vector<string> replaceArrayAfterDefine;
    vector<string> conditionArrayAfterDefine;


    bool isBlank=false;
    int flag=0;
    bool incomment=false;
    j=0;
    for(i=0;i<(int)macrojs.size();i++){
        if(!incomment && macrojs[i].length()>=2 && macrojs[i][0]=='/' && macrojs[i][1]=='*'){
            incomment=true;
        }
        if(incomment){
            if(findNext(macrojs[i],"\\*\\/\\s*$")>=0){
                incomment=false;
            }
            isBlank=true;
            j=i+1;
            continue;
        }
        isBlank=false;
        if(macrojs[i].length()==0 || substring(macrojs[i],0,2)=="//"){
            isBlank=true;
        }else if(macrojs[i].length()>2 && macrojs[i][0]=='[' && macrojs[i][(int)(macrojs[i].length())-1]==']'){
            isBlank=true;
        }else{
            k=findNext(macrojs[i],"\\s+",0,0);
            if(getFoundPos(k)==0 && getFoundLen(k)==(int)(macrojs[i].length())){
                isBlank=true;
            }
        }
        if((flag&DEFINE)==0 && (isBlank || j+2<i)){
            if(j+1<i && ((flag&CLASS_ONLY)==0 || classMode)
                     && ((flag&AS2_ONLY)==0 || as2Mode)
                     && ((flag&FUNCTION)==0 || !objectBasedClassMode)
                     && ((flag&MEMBER)==0 || objectBasedClassMode)){
                strt=macrojs[j+1];
                if(strt.length()>1 && strt[0]=='\\'){
                    k=findNext(macrojs[i],"\\s+",0,0);
                    if(getFoundPos(k)==1 && getFoundLen(k)==(int)(macrojs[i].length())-1){
                        strt=substring(strt,1);
                    }
                }else if(strt=="\\"){
                    strt="";
                }
                if((flag&CLASS_ONLY)!=0){
                    if(strt.find('\\')!=string::npos){
                        replaceAll(strt,"(^|[^\\\\])\\\\c","$1"+className);
                        replaceAll(strt,"(^|[^\\\\])\\\\e","$1"+extendName);
                        replaceAll(strt,"(^|[^\\\\])\\\\C","$1"+constructorArg);
                        replaceAll(strt,"(^|[^\\\\])\\\\E","$1"+superConstructorUsedArg);
                    }
                    if((flag&AFTER_DEFINE)==0){
                        replaceArray.push_back(strt);
                    }else{
                        replaceArrayAfterDefine.push_back(strt);
                    }

                    strt=macrojs[j];
                    if(strt.find('\\')!=string::npos){
                        replaceAll(strt,"(^|[^\\\\])\\\\c","$1"+className);
                        replaceAll(strt,"(^|[^\\\\])\\\\e","$1"+extendName);
                        replaceAll(strt,"(^|[^\\\\])\\\\C","$1"+constructorArg);
                        replaceAll(strt,"(^|[^\\\\])\\\\E","$1"+superConstructorUsedArg);
                    }
                    if((flag&AFTER_DEFINE)==0){
                        findArray.push_back(strt);
                    }else{
                        findArrayAfterDefine.push_back(strt);
                    }

                    if(j+2<i){
                        strt=macrojs[j+2];
                        if(strt.find('\\')!=string::npos){
                            replaceAll(strt,"(^|[^\\\\])\\\\c","$1"+className);
                            replaceAll(strt,"(^|[^\\\\])\\\\e","$1"+extendName);
                            replaceAll(strt,"(^|[^\\\\])\\\\C","$1"+constructorArg);
                            replaceAll(strt,"(^|[^\\\\])\\\\E","$1"+superConstructorUsedArg);
                        }
                    }else{
                        strt="";
                    }
                    if((flag&AFTER_DEFINE)==0){
                        conditionArray.push_back(strt);
                    }else{
                        conditionArrayAfterDefine.push_back(strt);
                    }

                }else if((flag&AFTER_DEFINE)==0){
                    findArray.push_back(macrojs[j]);
                    replaceArray.push_back(strt);
                    conditionArray.push_back(j+2<i?macrojs[j+2]:"");
                }else{
                    findArrayAfterDefine.push_back(macrojs[j]);
                    replaceArrayAfterDefine.push_back(strt);
                    conditionArrayAfterDefine.push_back(j+2<i?macrojs[j+2]:"");
                }
            }
            j=isBlank?i+1:i;
        }else if((flag&DEFINE)!=0){
            if(!isBlank){
                k=macrojs[i].find(' ');
            }
            if(!isBlank && k>0){
                hasDefine=true;
                varNameSrc=substring(macrojs[i],0,k);
                varNameDest=substring(macrojs[i],k+1);
                if((flag&MEMBER)==0 && (flag&FUNCTION)==0
                    && vectorStringIndexOf(vtable.propertyTable,varNameSrc)<0){
                    vtable.propertyTable.push_back(varNameSrc);
                    vtable.propertyTypeTable.push_back("");
                    vtable.propertyConfusedTable.push_back(varNameDest);
                }else if((flag&MEMBER)==0 && (flag&FUNCTION)!=0
                         && vectorStringIndexOf(vtable.methodTable,varNameSrc)<0){
                    vtable.methodTable.push_back(varNameSrc);
                    vtable.methodTypeTable.push_back("");
                    vtable.methodConfusedTable.push_back(varNameDest);
                }else{
                    k=varNameSrc.find('.');
                    if(k>0){
                        classNameSrc=substring(varNameSrc,0,k);
                        varNameSrc=substring(varNameSrc,k+1);
                    }else{
                        classNameSrc=className;
                        varNameSrc=substring(varNameSrc,k+1);
                    }
                    k=varNameDest.find('.');
                    if(k>0){
                        classNameDest=substring(varNameDest,0,k);
                        varNameDest=substring(varNameDest,k+1);
                    }else{
                        classNameDest=className;
                        varNameDest=substring(varNameDest,k+1);
                    }
                    k=vectorStringIndexOf(classNameTable,classNameSrc);
                    if(k<0){
                        VaribleTable vt1;
                        vtf.push_back(vt1);
                        classNameTable.push_back(classNameSrc);
                        classReplacedTable.push_back(classNameDest);
                        k=(int)(vtf.size())-1;
                    }
                    if((flag&FUNCTION)==0 && vectorStringIndexOf(vtf[k].propertyTable,varNameSrc)<0){
                        vtf[k].propertyTable.push_back(varNameSrc);
                        vtf[k].propertyTypeTable.push_back("");
                        vtf[k].propertyConfusedTable.push_back(varNameDest);
                    }else if((flag&FUNCTION)!=0 && vectorStringIndexOf(vtf[k].methodTable,varNameSrc)<0){
                        vtf[k].methodTable.push_back(varNameSrc);
                        vtf[k].methodTypeTable.push_back("");
                        vtf[k].methodConfusedTable.push_back(varNameDest);
                    }
                }
            }
            j=isBlank?i+1:i;
        }
        if(macrojs[i].length()>2 && macrojs[i][0]=='[' && macrojs[i][(int)(macrojs[i].length())-1]==']'){
            if(macrojs[i]=="[general]"){
                flag=0;
            }else if(macrojs[i]=="[class]"){
                flag=CLASS_ONLY;
            }else if(macrojs[i]=="[function based class]"){
                flag=FUNCTION|CLASS_ONLY;
            }else if(macrojs[i]=="[object based class]"){
                flag=MEMBER|CLASS_ONLY;
            }else if(macrojs[i]=="[as2]"){
                flag=AS2_ONLY;
            }else if(macrojs[i]=="[after defines]"){
                flag=AFTER_DEFINE;
            }else if(macrojs[i]=="[class after defines]"){
                flag=AFTER_DEFINE|CLASS_ONLY;
            }else if(macrojs[i]=="[function based class after defines]"){
                flag=FUNCTION|AFTER_DEFINE|CLASS_ONLY;
            }else if(macrojs[i]=="[object based class after defines]"){
                flag=MEMBER|AFTER_DEFINE|CLASS_ONLY;
            }else if(macrojs[i]=="[as2 after defines]"){
                flag=AFTER_DEFINE|AS2_ONLY;
            }else if(macrojs[i]=="[varibles]"){
                flag=DEFINE;
            }else if(macrojs[i]=="[functions]"){
                flag=FUNCTION|DEFINE;
            }else if(macrojs[i]=="[member varibles]"){
                flag=MEMBER|DEFINE;
            }else if(macrojs[i]=="[member functions]"){
                flag=MEMBER|FUNCTION|DEFINE;
            }
        }
    }

    bool findNo=false;
    if(findArray.size()>0){
        VaribleTable vta;
        vector<string> publicVectorTable;
        vector<string> staticVectorTable;
        vector<string> strac;
        strac.push_back("");
        lengt=findArray.size();
        if(lengt>(int)(replaceArray.size())){
            lengt=replaceArray.size();
        }
        if(lengt>(int)(conditionArray.size())){
            lengt=conditionArray.size();
        }
        for(i=0;i<(int)str0.size();i++){
            findNo=false;
            for(j=0;j<lengt;j++){
                if(conditionArray[j].length()!=0 && findNo && j>0 && conditionArray[j]==conditionArray[j-1]){
                    continue;
                }
                if(replaceArray[j].length()>=2 && replaceArray[j][0]=='\\'){
                    strt=replaceArray[j];
                    if(replaceArray[j][1]=='a' || replaceArray[j][1]=='u' ||
                       replaceArray[j][1]=='v' || replaceArray[j][1]=='V'){
                        strt=substring(replaceArray[j],2);
                    }
                    if(conditionArray[j].length()==0 || findNext(str0[i],conditionArray[j])>=0){
                        if(findNo){
                            findNo=false;
                        }
                        if(replaceAll(str0[i],findArray[j],strt)==0){
                            continue;
                        }
                    }else{
                        findNo=true;
                        continue;
                    }
                    if(replaceArray[j][1]=='a'){
                        strac[0]=str0[i];
                        attachConstTable(strac,ctable,false,false,false);
                        str0[i]=strac[0];
                    }else if(replaceArray[j][1]=='v' || replaceArray[j][1]=='u'){
                        strt=str0[i];
                        k=findNext(strt,"[=;]",0,0);
                        if(k>=0){
                            k=getFoundPos(k);
                            strt=substring(strt,0,k);
                            k=strt.find(':');
                            if(k>=0){
                                varNameDest=substring(strt,0,k);
                                classNameDest=substring(strt,k+1);
                            }else{
                                varNameDest=strt;
                                classNameDest="";
                            }
                            if(str0[i].length()>strt.length() && str0[i][strt.length()]==';'){
                                str0[i]=substring(str0[i],strt.length()+1);
                            }
                            if(replaceArray[j][1]=='u'){
                                k=1;
                                ostr.str("");
                                ostr<<k;
                                varNameDest+=ostr.str();
                                while(vectorStringIndexOf(vta.propertyTable,varNameDest)>=0
                                   || vectorStringIndexOf(vtable.propertyTable,varNameDest)>=0){
                                    varNameDest=substring(varNameDest,0,
                                                (int)(varNameDest.length())-(int)(ostr.str().length()));
                                    k++;
                                    ostr.str("");
                                    ostr<<k;
                                    varNameDest+=ostr.str();
                                }
                                vta.propertyTable.push_back(varNameDest);
                                vta.propertyTypeTable.push_back(classNameDest);
                                vta.propertyConfusedTable.push_back(varNameDest);
                                if(classMode && classNameDest==""){
                                    publicVectorTable.push_back(varNameDest);
                                }else if(classMode){
                                    strt=varNameDest+":"+classNameDest;
                                    publicVectorTable.push_back(strt);
                                }
                                strt=substring(varNameDest,0,
                                               (int)(varNameDest.length())-(int)(ostr.str().length()));
                                replaceAll(str0[i],"(^|\\W)"+strt+"(\\W|$)","$1"+varNameDest+"$2");
                            }else if(vectorStringIndexOf(vtable.propertyTable,varNameDest)<0){
                                if(vectorStringIndexOf(vta.propertyTable,varNameDest)<0){
                                    vta.propertyTable.push_back(varNameDest);
                                    vta.propertyTypeTable.push_back(classNameDest);
                                    vta.propertyConfusedTable.push_back(varNameDest);
                                    if(classMode){
                                        publicVectorTable.push_back(strt);
                                    }
                                }
                            }else{
                                ostr.str("");
                                ostr<<"变量"<<varNameDest<<"已经定义，无法再添加。("<<str0[i]<<")";
                                message(ostr.str());
                            }
                        }
                    }else if(replaceArray[j][1]=='V'){

                        k=findNext(str0[i],"var (\\w+)",0,1);
                        while(k>=0){
                            strt=substring(str0[i],getFoundPos(k),getFoundPos(k)+getFoundLen(k));
                            if(objectBasedClassMode || findNext(str0[i],"(^|\\s)static ",0,0)<0){
                                publicVectorTable.push_back(strt);
                            }else{
                                staticVectorTable.push_back(strt);
                            }
                            k=findNext(str0[i],",(\\w+)",k,1);
                        }
                        k=findNext(str0[i],"function (\\w+)\\(",0,1);
                        if(k>=0){
                            strt=substring(str0[i],getFoundPos(k),getFoundPos(k)+getFoundLen(k));
                            if(objectBasedClassMode || findNext(str0[i],"(^|\\s)static ",0,0)<0){
                                publicVectorTable.push_back(strt);
                            }else{
                                staticVectorTable.push_back(strt);
                            }
                        }
                    }
                    continue;
                }
                if(conditionArray[j].length()==0 || findNext(str0[i],conditionArray[j])>=0){
                    if(findNo){
                        findNo=false;
                    }
                    replaceAll(str0[i],findArray[j],replaceArray[j]);
                }else{
                    findNo=true;
                }
            }
        }

        if(vta.propertyTable.size()>0){
            lengt=vta.propertyTable.size();
            if(lengt>(int)(vta.propertyTypeTable.size())){
                lengt=vta.propertyTypeTable.size();
            }
            if(!classMode){
                j=0;
                if(str0.size()==0){
                    str0.push_back("");
                }
                for(i=lengt-1;i>=0;i--){
                    if(vta.propertyTypeTable[i]==""){
                        str0[j]="var "+vta.propertyTable[i]+";\n"+str0[j];
                    }else{
                        str0[j]="var "+vta.propertyTable[i]+":"+vta.propertyTypeTable[i]+";\n"+str0[j];
                    }
                }
            }else{
                for(j=0;j<(int)str0.size();j++){
                    if(str0[j].find(" class ")!=string::npos){
                        break;
                    }
                }
                if(j>=(int)str0.size()){
                    j=0;
                }
                for(i=0;i<lengt;i++){
                    if(vta.propertyTypeTable[i]==""){
                        str0[j]+="\npublic var "+vta.propertyTable[i]+";";
                    }else{
                        str0[j]+="\npublic var "+vta.propertyTable[i]+":"+vta.propertyTypeTable[i]+";";
                    }
                }
            }
            concatVaribleTableWith(vtable,vta);
        }
        if(classMode && (publicVectorTable.size()>0 || staticVectorTable.size()>0)){
            for(i=0;i<(int)(publicVectorTable.size());i++){
                j=vectorStringIndexOf(vtable.propertyTable,publicVectorTable[i]);
                if(j>=0 && j<(int)(vtable.propertyConfusedTable.size())
                   && substring(vtable.propertyConfusedTable[j],0,5)!="this."){
                    hasDefine=true;
                    vtable.propertyConfusedTable[j]="this."+vtable.propertyConfusedTable[j];
                }
                j=vectorStringIndexOf(vtable.methodTable,publicVectorTable[i]);
                if(j>=0 && j<(int)(vtable.methodConfusedTable.size())
                   && substring(vtable.methodConfusedTable[j],0,5)!="this."){
                    hasDefine=true;
                    vtable.methodConfusedTable[j]="this."+vtable.methodConfusedTable[j];
                }
            }
            for(i=0;i<(int)(staticVectorTable.size());i++){
                strt=className+".";
                j=vectorStringIndexOf(vtable.propertyTable,staticVectorTable[i]);
                if(j>=0 && j<(int)(vtable.propertyConfusedTable.size())
                   && substring(vtable.propertyConfusedTable[j],0,strt.length())!=strt){
                    hasDefine=true;
                    vtable.propertyConfusedTable[j]=className+"."+vtable.propertyConfusedTable[j];
                }
                j=vectorStringIndexOf(vtable.methodTable,staticVectorTable[i]);
                if(j>=0 && j<(int)(vtable.methodConfusedTable.size())
                   && substring(vtable.methodConfusedTable[j],0,strt.length())!=strt){
                    hasDefine=true;
                    vtable.methodConfusedTable[j]=className+"."+vtable.methodConfusedTable[j];
                }
            }
        }

        i=0;
        while(i<(int)str0.size()){
            j=str0[i].find('\n');
            if(j>=0){
                strt=substring(str0[i],j+1);
                str0.insert(str0.begin()+(i+1),strt);
                str0[i]=substring(str0[i],0,j);
                moveComment(ctable,i,1);
            }
            i++;
        }
    }

    if(hasDefine){
        if(!classMode){
            strt="class []{";
            str0.insert(str0.begin(),strt);
            str0.push_back("}");
        }
        confuse(str0,vtable,vtf,classNameTable,classReplacedTable,"","",(!as2Mode && classMode)?-2:-1);
        if(classMode){
            for(i=0;i<(int)(str0.size());i++){
                replaceAll(str0[i],"(\\w+\\.)this\\.","$1");
            }
        }
        if(!classMode && str0.size()>=2){
            str0.pop_back();
            str0.erase(str0.begin());
        }
    }
    if(classMode && str0.size()>0 && findNext(str0[0],"^package(\\s|\\{)",0,0)>=0){
        for(i=(int)(str0.size())-1;i>=0;i--){
            if(str0[i]=="}"){
                str0[i]="";
                str0.erase(str0.begin()+i);
                moveComment(ctable,i,-1,true);
                break;
            }else if(str0[i].length()>0 && str0[i][(int)(str0[i].length())-1]=='}'){
                str0[i]=substring(str0[i],0,(int)(str0[i].length())-1);
                break;
            }
        }
        str0[0]="";
        str0.erase(str0.begin());
        moveComment(ctable,0,-1,true);
    }
    if(findArrayAfterDefine.size()>0){
        lengt=findArrayAfterDefine.size();
        if(lengt>(int)(replaceArrayAfterDefine.size())){
            lengt=replaceArrayAfterDefine.size();
        }
        if(lengt>(int)(conditionArrayAfterDefine.size())){
            lengt=conditionArrayAfterDefine.size();
        }
        for(i=0;i<(int)str0.size();i++){
            findNo=false;
            for(j=0;j<lengt;j++){
                if(conditionArrayAfterDefine[j].length()!=0 && findNo && j>0 && conditionArrayAfterDefine[j]==conditionArrayAfterDefine[j-1]){
                    continue;
                }
                if(conditionArrayAfterDefine[j].length()==0 || findNext(str0[i],conditionArrayAfterDefine[j])>=0){
                    if(findNo){
                        findNo=false;
                    }
                    replaceAll(str0[i],findArrayAfterDefine[j],replaceArrayAfterDefine[j]);
                }else{
                    findNo=true;
                }
            }
        }
        i=0;
        while(i<(int)str0.size()){
            j=str0[i].find('\n');
            if(j>=0){
                strt=substring(str0[i],j+1);
                str0.insert(str0.begin()+(i+1),strt);
                str0[i]=substring(str0[i],0,j);
                moveComment(ctable,i,1);
            }
            i++;
        }
    }

    for(i=0;i<(int)str0.size();i++){
        if(str0[i]=="@_#"){
            j=mvt0.propertyTable.size();
            ostr.str("");
            ostr<<"//movie varibles count:"<<j;
            str0[i]=ostr.str();
        }else if(substring(str0[i],0,2)=="@_"){
            strt=substring(str0[i],2,(int)(str0[i].length())-1);
            j=atoi(strt.c_str())+1;
            ostr.str("");
            ostr<<"//frame code found:"<<j;
            str0[i]=ostr.str();
        }
    }


    attachConstTable(str0,ctable,true,true,false);
    saveFile(pathNamet,str0,(openRet==1 || extName==".xfl"));



    message("\n转换完成");
    return 0;
}


MainProgress::MainProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainProgress)
{
    ui->setupUi(this);
    Qt::WindowFlags flags=Qt::Dialog;

    flags |=Qt::WindowMinimizeButtonHint;
    flags |=Qt::WindowMaximizeButtonHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    /*ui->logText->setPlainText("12345\nabcde");
    QString str;
    str = ui->logText->toPlainText();
    str+="\n一二三四五";
    ui->logText->setPlainText(str);*/
    //resize();
   installEventFilter(this);
    ui->progressBar->setVisible(false);

    //ui_log=ui->logText;
    //ui_progress=ui->progressBar;
    //readFileInfoAndSave(flashFileName);

   //ui_log=ui->logText;
   //ui_progress=ui->progressBar;
   //transferring=true;
   //transferTherad();

   transferTheradClass=new TransferTheradClass();
   //transferTheradClass->attachLogSignal();
   if(transferMode==0){
       transferTheradClass->transferTheradFunction=asToJsThread;
   }else if(transferMode==1){
       transferTheradClass->transferTheradFunction=as3ConfuseTherad;
   }
   QObject::connect(transferTheradClass,SIGNAL(attachLogSignal(QString)),this,SLOT(attachLogSlot(QString)));
   QObject::connect(transferTheradClass,SIGNAL(setCloseableSignal(bool)),this,SLOT(setCloseableSlot(bool)));
   transferTheradClass->start();
   //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
}

bool MainProgress::eventFilter(QObject *o, QEvent *e){

    if(o==this && e->type()==QEvent::Resize)
    {

        //resize();
    }
    return false;
}

void MainProgress::keyPressEvent(QKeyEvent *event){
    switch(event->key())
      {
         //进行界面退出，重写Esc键，否则重写reject()方法
        case Qt::Key_Escape:
          break;
       default:
          QDialog::keyPressEvent(event);
      }
}

void MainProgress::resize(){
    /*int bottomOffset0=ui->progressBar->height();
    int bottomOffset=(int)(bottomOffset0*1.5);
    ui->logText->move(0,0);
    int bottomY=this->height()-bottomOffset;
    ui->logText->resize(this->width(),bottomY>0?bottomY:0);
    ui->progressBar->move(0,bottomY+(bottomOffset-bottomOffset0)/2);
    ui->progressBar->resize(this->width(),bottomOffset0);*/
}

void MainProgress::attachLogSlot(QString s){
   QString strt=ui->logText->toPlainText()+s;
   strt+="\n";
   ui->logText->setText(strt);

   ui->logText->moveCursor(QTextCursor::End);
}

void MainProgress::setCloseableSlot(bool closeable){
    /*if(closeable){
        Qt::WindowFlags flags=Qt::Dialog;
        flags |=Qt::WindowMinimizeButtonHint;
        flags |=Qt::WindowMaximizeButtonHint;
        flags |=Qt::WindowCloseButtonHint;
        setWindowFlags(flags);
    }else{
        setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    }*/
    //setWindowFlags(windowFlags() &~ Qt::WindowCloseButtonHint);
}

void MainProgress::closeEvent(QCloseEvent *event){
    if(transferring){
        event->ignore();
    }else{
        event->accept();
    }
}

MainProgress::~MainProgress()
{
    //delete transferTheradClass;
    transferTheradClass=NULL;
    ui_log=NULL;
    ui_progress=NULL;
    delete ui;
}
