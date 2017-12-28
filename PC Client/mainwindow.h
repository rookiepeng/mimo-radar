/****************************************************************************************
//    Copyright (C) 2017  Zhengyu Peng, zhengyu.peng@ttu.edu
*****************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextTable>
#include <QScrollBar>
#include <QSettings>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QFileDialog>
#include <QDateTime>

#include "myudp.h"
#include "mytcpclient.h"
#include "plot.h"

#define TCPCLIENT 20
#define UDPSERVER 30
#define APPVERSION "V1.0"

QT_CHARTS_USE_NAMESPACE

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

private slots:
  /******************************************************************************
     *
     * Network Interface
     *
     ******************************************************************************/
  void onRefreshButtonClicked();

  /******************************************************************************
     *
     * TCP Client and UDP Server
     *
     ******************************************************************************/
  void onConnectButtonClicked();
  void onTcpClientNewConnection(const QString &from, quint16 port);
  void onStopButtonClicked();
  void onTcpClientTimeOut();
  void onDisconnectButtonClicked();
  void onDisconnected();
  void onTcpClientSendMessage();

  void onAppendMessage(const QString &from, const QString &message);
  void onAppendMessage(const QString &from, const QVector<float> &data);

  /******************************************************************************
     *
     * Waveform
     *
     ******************************************************************************/
  void openPlot();

  void saveToDir();

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent *event);

private:
  Ui::MainWindow *ui;
  void initUI();
  void loadSettings();
  void saveSettings();
  void findLocalIPs();
  bool setupConnection(quint8 type);

  void restoreWindowState();

  QTextTableFormat tableFormatSend, tableFormatData, tableFormatReceive;

  MyUDP *myudp = nullptr;
  MyTCPClient *mytcpclient = nullptr;

  QHostAddress tcpClientTargetAddr;
  quint16 tcpClientTargetPort;

  QHostAddress localAddr;

  quint16 tcpServerListenPort;

  quint16 udpListenPort;
  QHostAddress udpTargetAddr;
  quint16 udpTargetPort;

  QString settingsFileDir;
  QList<QNetworkInterface> interfaceList;
  quint8 type;

  QMessageBox msgBox;

  Plot plot;

  QString fileDir;
  QString txChannel="0",rxChannel="0";
};

#endif // MAINWINDOW_H
