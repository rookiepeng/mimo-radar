/****************************************************************************************
//    Copyright (C) 2017  Zhengyu Peng, zhengyu.peng@ttu.edu
*****************************************************************************************/

#ifndef MYTCPCLIENT_H
#define MYTCPCLIENT_H

#include <QTcpSocket>
#include <QTcpServer>

class MyTCPClient : public QTcpSocket
{
  Q_OBJECT
public:
  explicit MyTCPClient(QObject *parent = nullptr);
  void connectTo(QHostAddress addr, quint16 port);
  void sendMessage(QString string);
  void disconnectCurrentConnection();
  void closeClient();
  void abortConnection();

signals:
  void newMessage(const QString &from, const QString &message);
  void myClientConnected(const QString &from, quint16 port);
  void myClientDisconnected();
  void connectionFailed();

private slots:
  void onConnected();
  void onDisconnected();
  void messageReady();
  void onStateChanged(QAbstractSocket::SocketState state);

private:
  QTcpSocket *tcpSocket;
  QByteArray array;
  QVector<quint32> timeStamp;
  QVector<double> adcData;
  bool acceptingADCData = false;
};

#endif // MYTCPCLIENT_H
