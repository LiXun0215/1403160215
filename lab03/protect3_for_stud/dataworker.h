#ifndef DATAWORKER_H
#define DATAWORKER_H

#include <QObject>
#include <QDateTime>

class QNetworkAccessManager;
class QNetworkReply;
class QSslError;


class dataWorker : public QObject
{
    Q_OBJECT
public:
    explicit dataWorker(QObject *parent = 0);
    explicit dataWorker(QString date, QObject *parent = 0);

    void setRequestCity(QString newCity);
    void setRequestDate(QString newDate);

    void setDatatype(int type);
    int datatype;

    QString requestCity();
    QString requestDate();

    void doRequest();

protected:
    QString requestUrl();
    void httpGet(QString url);
    void initNetwork();
    void parseHTML(const QString sourceText);
    void parseData(const QString sourceText);
    void exportDataToFile(const QString dataText);

protected slots:
    void httpsFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QString _requestCity;
    QString _requestDate;

    QList<QDateTime> dataDate;              // 日期
    QList<qreal> dataHigh;                  // 最高温度
    QList<qreal> dataLow;                   // 最低温度
    QList<qreal> dataAQI;
    QList<qreal> dataPM25;

    const QString splitter;
    const QString dataPath;

signals:
    void dataParseFinished(QList<QDateTime> date,QList<qreal> high, QList<qreal> low);
    void dataError();
    void httpRequestError(QString error);//http请求错误信号，当出现http请求错误时，发送该信号

};


#endif // DATAWORKER_H

