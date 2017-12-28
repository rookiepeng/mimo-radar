/****************************************************************************************
//    Copyright (C) 2017  Zhengyu Peng, zhengyu.peng@ttu.edu
*****************************************************************************************/

#ifndef MYUDP_H
#define MYUDP_H

#include <QUdpSocket>
#include <QPointF>
#include <QFile>

class MyUDP : public QUdpSocket
{
  Q_OBJECT

public:
  explicit MyUDP(QObject *parent = nullptr);
  bool bindPort(QHostAddress addr, qint16 port);
  void unbindPort();

signals:
  void newMessage(const QString &from, const QString &message);
  void newMessage(const QString &from, const QVector<float> &data);
  void newData(const QVector<QPointF> &plotData);

public slots:
  void readyRead();
  void sendMessage(QHostAddress sender, quint16 senderPort, QString string);

private:
  QUdpSocket *socket;
  QByteArray array;
  QVector<quint32> timeStampMCU;
  QVector<float> adcData;
  QVector<float> timeStamp; // millisecond

  QVector<QPointF> plotData;
  bool acceptingADCData = false;
};

#endif // MYUDP_H
