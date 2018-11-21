#include "dataworker.h"
#include <QtNetwork>
#include <QXmlStreamReader>
#include <QNetworkRequest>
#include <QRegExp>

/**
 * @brief dataWorker::dataWorker 构造函数
 * @param parent
 *
 * 默认构造函数，创建网络访问管理类QNetworkAccessManager对象<br/>
 * 并连接相应的信号与槽。
 *
 */
dataWorker::dataWorker(QObject *parent) :
    QObject(parent),splitter("#-#-#"),dataPath("data")
{
    initNetwork();
}

/**
 * @brief 设置请求年月的setter函数
 * @param newDate 新的年月
 *
 * 该函数简单的将传入年月做一个处理，删除"-"，<br/>
 * 如传入“2018-10”，则转换为“201810”的格式，以符合HTTP请求链接的格式。
 *
 */
void dataWorker::setRequestDate(QString newDate)
{
    _requestDate = newDate.replace("-","");
}

void dataWorker::setRequestCity(QString newCity)//动态查询城市数据
{
    _requestCity=newCity;
}

void dataWorker::setDatatype(int type )//查询数据函数，查气温则datatype=0，查空气质量则datatype=1
{
     datatype = type;
}

/**
 * @brief getter函数，获得当前的年月
 * @return 请求年月
 */
QString dataWorker::requestDate()//获取当前的年月
{
    return _requestDate;
}

QString dataWorker::requestCity()//获取当前城市
{
    return _requestCity;
}

/**
 * @brief 开始发送请求
 *
 * 该函数首先检查是否存在本地文件，如果有则读取本地数据文件；<br/>
 * 否则，发起http请求，从网络获取数据。
 */
void dataWorker::doRequest()
{
    if(datatype==0)
    {
        // 导入数据，首先检查是否已经存在数据文件
        QString fName = QString("%1/%2%3-%4.txt").arg(dataPath,"weather_",_requestCity,_requestDate);

        QStringList dataList;
        QFile f(fName);
        if(f.open(QIODevice::ReadOnly|QIODevice::Text)){  // 成功打开数据文件，则由文件中读取
            qDebug().noquote()<<QString("数据由文件%1导入...").arg(fName);
            QTextStream stream (&f);
            while(!stream.atEnd())
                 dataList<<stream.readLine();
            // 数据导入完成，开始解析
            parseData(dataList.join(splitter).simplified());//解析数据函数
        }else{
            // 如果无数据文件，则从网络获取
            qDebug().noquote()<<QString("数据由网络获取...");
            httpGet(requestUrl());//发起http请求，从网络获取用户选定的日期和城市的天气
        }
    }

    if(datatype==1)
    {
        // 导入数据，首先检查是否已经存在数据文件
        QString fName = QString("%1/%2%3-%4.txt").arg(dataPath,"aqi_",_requestCity,_requestDate);

        QStringList dataList;
        QFile f(fName);

        if(f.open(QIODevice::ReadOnly|QIODevice::Text)){  // 成功打开数据文件，则由文件中读取
            qDebug().noquote()<<QString("数据由文件%1导入...").arg(fName);
            QTextStream stream (&f);
            while(!stream.atEnd())
                 dataList<<stream.readLine();
            // 数据导入完成，开始解析
            parseData(dataList.join(splitter).simplified());
        }else{
            // 如果无数据文件，则从网络获取
            qDebug().noquote()<<QString("数据由网络获取...");
            httpGet(requestUrl());//发起http请求，从网络获取用户选定的日期和城市的空气质量
        }
    }
}


/**
 * @brief 构造实际请求链接
 * @return 数据页面地址
 *
 * 该函数将请求年月插入模板中，获得实际数据页面的链接地址。
 */
QString dataWorker::requestUrl()
{
    QString r;
    if(datatype==0)
    {
        r= QString("http://lishi.tianqi.com/%1/%2.html").arg(_requestCity).arg(_requestDate);//天气
        qDebug()<<r;//将请求年月插入模板中，获得实际数据页面的天气链接地址
        return r;
     }
    if(datatype==1)
    {
        r= QString("http://www.tianqihoubao.com/aqi/%1-%2.html").arg(_requestCity).arg(_requestDate); //空气质量
        qDebug()<<r;//将请求年月插入模板中，获得实际数据页面的空气质量链接地址
        return r;
    }

    return r;
}


/**
 * @brief 初始化网络请求管理类QNetworkAccessManager类
 *
 *  该函数创建一个QNetworkAccessManager类的指针对象，并连接相应的信号和槽。
 */
void dataWorker::initNetwork()
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,this, &dataWorker::httpsFinished);
}


/**
 * @brief 解析HTML
 * @param sourceText HTML文本
 *
 * 该函数使用QXmlStreamReader对HTML文本进行解析。<br/>
 * 注意： HTML必须是符合XML规范的文本（标签对完整）<br/>
 *
 * QXmlStreamReader类提供了一个针对良好格式化的基于流方式的XML快速解析器，<br/>
 * 适合于实现一次性解析任务（“一次性解析”任务，可以理解为只需读取文档一次，<br/>
 * 从头到尾一次性遍历处理XML文档，期间不会有反复的情况）。<br/>
 * <br/>
 *
 */
void dataWorker::parseHTML(const QString sourceText)//解析HTML文本函数
{
    // 使用QXmlStreamReader解析Html文档
    QXmlStreamReader reader(sourceText.simplified().trimmed());
    //simplified把一个字符串首尾的空格全部清除，字符串中间的连续空格都统一转化成一个空格
    //trimmed过滤掉字符串两端的空白符（如空格，制表符，和换行符）
    QStringList strData;//解析出来的数据存入一个字符串列表对象中
         while (!reader.atEnd()) {
          reader.readNext();//每次readNext()调用，解析器都会读取下一个数据（只有不是尾部）
          if (reader.isStartElement()) {//如果是开始节点
              if(datatype==0)
              {
                  if (reader.name() == "ul"){         // 查找Html标签：ul
                      strData<<reader.readElementText(QXmlStreamReader::IncludeChildElements).trimmed();
                  }
   //当查找到符合要求的标签后，调用readElementText函数会获得当前标签内的所有数据，其他<>内的不会读取
              }

              if(datatype==1)
              {
                  if (reader.name() == "tr"){         // 查找Html标签：tr
                      strData<<reader.readElementText(QXmlStreamReader::IncludeChildElements).trimmed();
                  }
              }

          }
      }
    if (reader.hasError()) {
        qDebug()<< "  读取错误： " << reader.errorString();
        emit  dataError();
    }else{
        if(!strData.isEmpty()){
            parseData(strData.join(splitter));//该函数处理parseHTML函数提取的文本，对其按照数据格式进行分割。
            //join()的参数被插入到每对被组合的字符串之间，即用分隔符分开数据
            exportDataToFile(strData.join(splitter));//保存数据到本地函数
        }
    }
}


/**
 * @brief 解析数据
 * @param sourceText 需要解析的文本
 *
 * 该函数处理parseHTML函数提取的文本，对其按照数据格式进行分割。 <br/>
 * 本程序只需使用前三列数据，在解析时每行文本只需使用前三个数据即可。<br/>
 * 数据解析完成后，发送dataParseFinished信号通知界面进行数据更新。<br/>
 */
void dataWorker::parseData(const QString sourceText)
{
    QStringList dataList = sourceText.split(splitter);
    dataDate.clear();
    dataAQI.clear();
    dataPM25.clear();
    dataHigh.clear();
    dataLow.clear();
    dataList.removeFirst();                  // 第一条数据是表头，删除
    for (QString s : dataList){
        QStringList dataList = s.split(" ",QString::SkipEmptyParts);
        qDebug()<<"开始解析"<<dataList;
        QDateTime momentInTime = QDateTime::fromString(dataList.at(0),"yyyy-MM-dd");
        dataDate.append(momentInTime);

        if(datatype==0)
        {
            dataHigh.append(dataList.at(1).toDouble());
            dataLow.append(dataList.at(2).toDouble());

        }
        if(datatype==1)
        {
            dataAQI.append(dataList.at(2).toDouble());//添加数据
            dataPM25.append(dataList.at(4).toDouble());
        }
    }
        if(datatype==0)
        {
            emit dataParseFinished(dataDate,dataHigh,dataLow);
        }//该信号在解析数据完成后将数据以三个列表的形式作为信号参数发射，提醒界面更新数据
        if(datatype==1)
        {
            emit dataParseFinished(dataDate,dataAQI,dataPM25);
        }


}


/**
 * @brief 将数据输出为文件
 * @param dataText 需要输出的数据文本
 *
 * 该类使用QFile和QTextStream类实现文本的保存。
 */
void dataWorker::exportDataToFile(const QString dataText)
{
    QStringList data = dataText.split(splitter);
    QDir dir;
    if( ! dir.exists(dataPath) )
        qDebug()<<dir.mkdir(dataPath);

    if(datatype==0)
    {
        QString fName = QString("%1/%2%3-%4.txt").arg(dataPath,"weather_",_requestCity,_requestDate);

        QFile f(fName);
        if(f.open(QIODevice::WriteOnly|QIODevice::Text)){
            QTextStream stream (&f);
            for( QString d : data)
                stream << d <<"\n";
        }else{
            qDebug()<<"打开文件错误";
        }
    }

    if(datatype==1)
    {
        QString fName = QString("%1/%2%3-%4.txt").arg(dataPath,"aqi_",_requestCity,_requestDate);

        QFile f(fName);
        if(f.open(QIODevice::WriteOnly|QIODevice::Text)){
            QTextStream stream (&f);
            for( QString d : data)
                stream << d <<"\n";
        }else{
            qDebug()<<"打开文件错误";
        }
    }

}

/**
 * @brief 执行http请求（GET方法）
 * @param url 请求目标网址
 */
void dataWorker::httpGet(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    manager->get(request);
}

/**
 * @brief 网络请求完成finished信号的响应槽函数
 * @param reply web服务器响应
 *
 */
void dataWorker::httpsFinished(QNetworkReply *reply)//在处理槽函数对获得数据进行处理
{
    int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString error="";
        if (reply->error()) {
            // 通知GUI，http访问出错
            error = QString("访问出错：代码 %1,原因 %2").arg(v).arg(reply->errorString());
            emit httpRequestError(error);
            reply->deleteLater();
            reply = Q_NULLPTR;
            return;
        }
        if(v != 200){
            error = QString("访问出错：代码 %1,原因 %2").arg(v).arg(reply->errorString());
            emit httpRequestError(error);
            reply->deleteLater();
            reply = Q_NULLPTR;
            return;
        }

    //if (reply->error()) {
        //qDebug()<<reply->errorString();
        //reply->deleteLater();
        //reply = Q_NULLPTR;
       // return;
   // }
   // int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //if(v != 200){
       // qDebug()<<"HTTP 返回代码："<<v;
        //reply->deleteLater();
        //reply = Q_NULLPTR;
      //  return;
   // }


    qDebug()<<"开始读取返回的数据：";
    QString html = QString::fromLocal8Bit(reply->readAll());

    // 对QNetworkReply对象，在使用完成后必须由程序员进行释放
    // 释放时，不能直接使用delete函数，而使用deleteLater函数
    reply->deleteLater();
    reply = Q_NULLPTR;

    qDebug()<<"返回数据长度："<<html.size();

    // 解析前简单处理，将包含内容的完整<div>..</div>标签内的文本内容，
    // 并滤除其中的空白字符"\r\n\t"
    if(datatype==0)
    {
       int begin = html.indexOf("<div class=\"tqtongji2\">");
       int end = html.indexOf("<div class=\"lishicity03\">");
       html = html.mid(begin,end-begin);
       html = html.left(html.indexOf("<div style=\"clear:both\">"));
       html = html.simplified().trimmed();

       qDebug()<<"开始解析数据"<<html;
       parseHTML(html);
      }
    if(datatype==1)
    {
        int begin=html.indexOf("<div class=\"api_month_list\">");
        int end=html.indexOf("<div id=\"chartdiv\" align=\"center\">");
        html=html.mid(begin,end-begin);
        html=html.left(html.indexOf("<p >"));
        html = html.simplified().trimmed();

        qDebug()<<"开始解析数据"<<html;
        parseHTML(html);
    }
}
