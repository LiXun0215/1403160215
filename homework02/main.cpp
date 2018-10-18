#include <QDebug>
#include <QVector>
#include <QTextStream>
#include <QFile>
#include <QCoreApplication>
#include <QList>

namespace SK {
enum SortKind{
    col01    =   0x00000001<<0,         //!< 第1列
    col02    =   0x00000001<<1,         //!< 第2列
    col03    =   0x00000001<<2,         //!< 第3列
    col04    =   0x00000001<<3,         //!< 第4列
    col05    =   0x00000001<<4,         //!< 第5列
    col06    =   0x00000001<<5,         //!< 第6列
    col07    =   0x00000001<<6,         //!< 第7列
    col08    =   0x00000001<<7,         //!< 第8列
    col09    =   0x00000001<<8,         //!< 第9列
    col10    =   0x00000001<<9,         //!< 第10列
    col11    =   0x00000001<<10,        //!< 第11列
    col12    =   0x00000001<<11,        //!< 第12列
    col13    =   0x00000001<<12,        //!< 第13列
    col14    =   0x00000001<<13,        //!< 第14列
    col15    =   0x00000001<<14,        //!< 第15列
    col16    =   0x00000001<<15,        //!< 第16列
    col17    =   0x00000001<<16,        //!< 第17列
    col18    =   0x00000001<<17,        //!< 第18列
    col19    =   0x00000001<<18,        //!< 第19列
    col20    =   0x00000001<<19,        //!< 第20列
    col21    =   0x00000001<<20,        //!< 第21列
    col22    =   0x00000001<<21,        //!< 第22列
    col23    =   0x00000001<<22,        //!< 第23列
    col24    =   0x00000001<<23,        //!< 第24列
    col25    =   0x00000001<<24,        //!< 第25列
    col26    =   0x00000001<<25,        //!< 第26列
    col27    =   0x00000001<<26,        //!< 第27列
    col28    =   0x00000001<<27,        //!< 第28列
    col29    =   0x00000001<<28,        //!< 第29列
    col30    =   0x00000001<<29,        //!< 第30列
    col31    =   0x00000001<<30,        //!< 第31列
    col32    =   0x00000001<<31,        //!< 第32列
};
}


typedef struct{
   QStringList alldata;                                                 // 补全结构定义
} studData;

QDebug operator<< (QDebug d, const studData &data)                      //定义重载函数
{
    for(int i=0;i<data.alldata.size();i++)

       {

           d.noquote()<<data.alldata.at(i);
       }

       qDebug()<<"";

       return d;

 }




// 比较类，用于std::sort第三个参数
class myCmp {
public:
    myCmp(int selectedColumn) { this->currentColumn = selectedColumn; }
    bool operator() (const studData& d1,const studData& d2);
private:
    int currentColumn;
};

bool myCmp::operator()(const studData &d1, const studData &d2)
{
    if(d1.alldata.at (currentColumn+1)>d2.alldata.at(currentColumn+1))

            return 0 ;

        else

            return 1;
}


class ScoreSorter                                                                  //新建分数排序类
{
public:
    ScoreSorter(QString dataFile);
    void readFile();
    void doSort();


private:
        QString filename;
        QList <studData> data;
        QStringList    num;
        studData list;
};










// 请补全
ScoreSorter::ScoreSorter(QString dataFile)
{
    filename=dataFile;
}


//void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)

    // 自定义qDebug











void ScoreSorter::readFile()                                                        //读文件函数

 {

     QFile File(filename);

     if(!File.open(QIODevice::ReadOnly | QIODevice::Text))                           //以只读方式读取文件
     {

             qDebug()<<"无法打开文件"<<endl;
     }



     QString num_t(File.readLine());

         num = num_t.split(" ", QString::SkipEmptyParts);

     while(!File.atEnd())
     {

         QString str(File.readLine());

         list.alldata = str.split(" ", QString::SkipEmptyParts);

         if((list.alldata).last() == "\n") list.alldata.removeLast();

         if(list.alldata.size()==0) continue;

         data.append(list);

     }
     File.close();
}




void ScoreSorter::doSort()                                        //定义排序函数
{
for(int j=1;j<num.count();j++)
   {
      myCmp cmp(j-1);                                              //左移0位开始
      std::sort(data.begin(),data.end(),cmp);
      qDebug()<<"当前是第"<<j<<"列排序，排序后输出为:";
      qDebug()<<num;
        for(int i=0;i<data.size();i++)
          {
                qDebug()<<data.at(i);

          }

     qDebug()<<"\n";
   }


}






int main()
{
    //qInstallMessageHandler(myMessageOutput);
    QString datafile = "C:/Users/Lixun/Desktop/build-homework02-Desktop_Qt_5_9_3_MinGW_32bit-Debug/debug/data.txt";

    // 如果排序后文件已存在，则删除之
    QFile f("sorted_"+datafile);
    if (f.exists())
    {
        f.remove();
    }

    ScoreSorter s(datafile);
    s.readFile();
    s.doSort();

        return 0;
}
