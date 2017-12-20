/****************************************************************************************
//    mainwindow.cpp
//
//    Copyright (C) 2017  Zach (Zhengyu) Peng, https://zpeng.me
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    restoreWindowState();
    ui->setupUi(this);
    initUI();
    findLocalIPs();
    loadSettings();

    /******
     * UDP
     ******/
    if (myudp == nullptr)
    {
        myudp = new MyUDP;
    }

    // buttons
    connect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
    connect(ui->button_Refresh, SIGNAL(clicked()), this, SLOT(onRefreshButtonClicked()));

    connect(ui->openWaveform, SIGNAL(clicked(bool)), this, SLOT(openWaveform()));
}

/***********************************
 *
 * TCP client start button clicked
 *
 ***********************************/
void MainWindow::onConnectButtonClicked()
{
    disconnect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
    ui->statusBar->showMessage("Connecting to Radar ...", 0);

    if (setupConnection(UDPSERVER))
    {
        ui->lineEdit_UdpListenPort->setDisabled(true);
        connect(myudp, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));

        if (setupConnection(TCPCLIENT))
        {
            ui->lineEdit_TcpClientTargetIP->setDisabled(true);
            ui->lineEdit_TcpClientTargetPort->setDisabled(true);
            ui->button_TcpClient->setText("Stop");

            connect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
            connect(mytcpclient, SIGNAL(myClientConnected(QString, quint16)), this, SLOT(onTcpClientNewConnection(QString, quint16)));
            connect(mytcpclient, SIGNAL(connectionFailed()), this, SLOT(onTcpClientTimeOut()));
        }
    }
    else
    {
        //ui->statusBar->showMessage(messageUDP + "Failed to listen to: " + localAddr.toString() + ": " + QString::number(udpListenPort), 2000);
        connect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
        ui->statusBar->showMessage("Disconnected", 0);
    }

    saveSettings();
}

/***********************************
 *
 * TCP client has a new connection
 *
 ***********************************/
void MainWindow::onTcpClientNewConnection(const QString &from, quint16 port)
{
    disconnect(mytcpclient, SIGNAL(myClientConnected(QString, quint16)), this, SLOT(onTcpClientNewConnection(QString, quint16)));
    disconnect(mytcpclient, SIGNAL(connectionFailed()), this, SLOT(onTcpClientTimeOut()));
    disconnect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
    connect(mytcpclient, SIGNAL(myClientDisconnected()), this, SLOT(onDisconnected()));

    ui->button_TcpClient->setDisabled(false);
    ui->button_TcpClient->setText("Disconnect");

    ui->button_TcpClientSend->setDisabled(false);
    ui->lineEdit_TcpClientSend->setDisabled(false);
    ui->textBrowser_TcpClientMessage->setDisabled(false);

    connect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onDisconnectButtonClicked()));

    connect(mytcpclient, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    connect(ui->button_TcpClientSend, SIGNAL(clicked()), this, SLOT(onTcpClientSendMessage()));
    connect(ui->lineEdit_TcpClientSend, SIGNAL(returnPressed()), this, SLOT(onTcpClientSendMessage()));

    ui->statusBar->showMessage("Connected to Radar: " + from + ": " + QString::number(port), 0);
}

/***********************************
 *
 * TCP client stop button clicked
 *
 ***********************************/
void MainWindow::onStopButtonClicked()
{
    disconnect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));

    ui->statusBar->showMessage("Disconnected", 0);
    disconnect(mytcpclient, SIGNAL(myClientConnected(QString, quint16)), this, SLOT(onTcpClientNewConnection(QString, quint16)));
    disconnect(mytcpclient, SIGNAL(connectionFailed()), this, SLOT(onTcpClientTimeOut()));
    ui->button_TcpClient->setText("Connect");
    mytcpclient->abortConnection();
    ui->lineEdit_TcpClientTargetIP->setDisabled(false);
    ui->lineEdit_TcpClientTargetPort->setDisabled(false);

    ui->button_TcpClientSend->setDisabled(true);
    ui->lineEdit_TcpClientSend->setDisabled(true);
    ui->textBrowser_TcpClientMessage->setDisabled(true);

    disconnect(myudp, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    myudp->unbindPort();
    ui->lineEdit_UdpListenPort->setDisabled(false);

    connect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
}

/***********************************
 *
 * TCP client connection time out
 *
 ***********************************/
void MainWindow::onTcpClientTimeOut()
{
    //ui->statusBar->showMessage(messageTCP + "Connection time out", 2000);
    disconnect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
    disconnect(mytcpclient, SIGNAL(myClientConnected(QString, quint16)), this, SLOT(onTcpClientNewConnection(QString, quint16)));
    disconnect(mytcpclient, SIGNAL(connectionFailed()), this, SLOT(onTcpClientTimeOut()));

    ui->button_TcpClient->setText("Connect");
    ui->lineEdit_TcpClientTargetIP->setDisabled(false);
    ui->lineEdit_TcpClientTargetPort->setDisabled(false);

    disconnect(myudp, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    myudp->unbindPort();
    ui->lineEdit_UdpListenPort->setDisabled(false);

    mytcpclient->closeClient();
    connect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
}

/***********************************
 *
 * TCP client diconnect button clicked
 *
 ***********************************/
void MainWindow::onDisconnectButtonClicked()
{
    mytcpclient->disconnectCurrentConnection();
}

/***********************************
 *
 * TCP client disconnected
 *
 ***********************************/
void MainWindow::onDisconnected()
{
    ui->statusBar->showMessage("Disconnected", 0);
    disconnect(mytcpclient, SIGNAL(myClientDisconnected()), this, SLOT(onDisconnected()));
    disconnect(mytcpclient, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    disconnect(ui->button_TcpClientSend, SIGNAL(clicked()), this, SLOT(onTcpClientSendMessage()));
    disconnect(ui->lineEdit_TcpClientSend, SIGNAL(returnPressed()), this, SLOT(onTcpClientSendMessage()));
    disconnect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onDisconnectButtonClicked()));
    ui->button_TcpClient->setText("Connect");

    ui->button_TcpClientSend->setDisabled(true);
    ui->lineEdit_TcpClientSend->setDisabled(true);
    ui->textBrowser_TcpClientMessage->setDisabled(true);

    ui->button_TcpClient->setDisabled(false);
    ui->lineEdit_TcpClientTargetIP->setDisabled(false);
    ui->lineEdit_TcpClientTargetPort->setDisabled(false);

    mytcpclient->closeClient();
    mytcpclient->close();

    // UDP
    disconnect(myudp, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    myudp->unbindPort();
    ui->lineEdit_UdpListenPort->setDisabled(false);

    connect(ui->button_TcpClient, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
}

/***********************************
 *
 * TCP client append a message
 * to message browser
 *
 ***********************************/
void MainWindow::onAppendMessage(const QString &from, const QString &message)
{
    //if (from.isEmpty() || message.isEmpty())
    //{
    //    return;
    //}

    QTextCursor cursor(ui->textBrowser_TcpClientMessage->textCursor());
    cursor.movePosition(QTextCursor::End);

    if (from == "System")
    {
        QColor color = ui->textBrowser_TcpClientMessage->textColor();
        ui->textBrowser_TcpClientMessage->setTextColor(Qt::gray);
        ui->textBrowser_TcpClientMessage->append(message);
        ui->textBrowser_TcpClientMessage->setTextColor(color);
    }
    else
    {
        QTextTable *table = cursor.insertTable(1, 2, tableFormatPri);
        table->cellAt(0, 0).firstCursorPosition().insertText('<' + from + "> ");
        table->cellAt(0, 1).firstCursorPosition().insertText(message);
    }
    QScrollBar *bar = ui->textBrowser_TcpClientMessage->verticalScrollBar();
    bar->setValue(bar->maximum());
}

/***********************************
 *
 * Send message through TCP client
 *
 ***********************************/
void MainWindow::onTcpClientSendMessage()
{
    QString text = ui->lineEdit_TcpClientSend->text();
    if (text.isEmpty())
    {
        return;
    }

    mytcpclient->sendMessage(text);

    onAppendMessage("Me", text);
    ui->lineEdit_TcpClientSend->clear();
}

/***********************************
 *
 * UI initialization
 *
 ***********************************/
void MainWindow::initUI()
{
    QString rule = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    ui->lineEdit_TcpClientTargetIP->setValidator(new QRegExpValidator(QRegExp("^" + rule + "\\." + rule + "\\." + rule + "\\." + rule + "$"), this));
    ui->lineEdit_TcpClientTargetPort->setValidator(new QIntValidator(0, 65535, this));
    ui->lineEdit_UdpListenPort->setValidator(new QIntValidator(0, 65535, this));

    ui->textBrowser_TcpClientMessage->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_TcpClientSend->setFocusPolicy(Qt::StrongFocus);
    ui->lineEdit_TcpClientSend->setFocus();

    ui->button_TcpClientSend->setDisabled(true);
    ui->lineEdit_TcpClientSend->setDisabled(true);
    ui->textBrowser_TcpClientMessage->setDisabled(true);

    ui->statusBar->showMessage("Disconnected", 0);

    tableFormatPri.setBorder(0);
    tableFormatPri.setBackground(QColor("#FCE4EC"));

    tableFormatAlt.setBorder(0);
    tableFormatAlt.setBackground(QColor("#E1F5FE"));
}

/***********************************
 *
 * Setup connections
 *
 ***********************************/
bool MainWindow::setupConnection(quint8 type)
{
    bool isSuccess = false;
    localAddr.setAddress(ui->label_LocalIP->text());

    switch (type)
    {
    case TCPCLIENT:
        isSuccess = true;
        tcpClientTargetAddr.setAddress(ui->lineEdit_TcpClientTargetIP->text());
        tcpClientTargetPort = ui->lineEdit_TcpClientTargetPort->text().toInt();
        if (mytcpclient == nullptr)
        {
            mytcpclient = new MyTCPClient;
        }
        mytcpclient->connectTo(tcpClientTargetAddr, tcpClientTargetPort);
        break;
    case UDPSERVER:
        udpListenPort = ui->lineEdit_UdpListenPort->text().toInt();
        isSuccess = myudp->bindPort(localAddr, udpListenPort);
        break;
    }
    return isSuccess;
}

/***********************************
 *
 * Find IP of local WiFi connections
 *
 ***********************************/
void MainWindow::findLocalIPs()
{
    ui->comboBox_Interface->clear();
    interfaceList.clear();
    QList<QNetworkInterface> listInterface = QNetworkInterface::allInterfaces();
    for (int i = 0; i < listInterface.size(); ++i)
    {
        //qDebug()<<listInterface.at(i).name();
        if (listInterface.at(i).humanReadableName().contains("Wi-Fi") || listInterface.at(i).humanReadableName().contains("wlp"))
        {
            interfaceList.append(listInterface.at(i));
        }
    }

    if (interfaceList.isEmpty())
    {
        // TODO wifilist is empty
    }
    else
    {
        for (int j = 0; j < interfaceList.size(); ++j)
        {
            ui->comboBox_Interface->addItem(interfaceList.at(j).humanReadableName());
        }
    }
}

/***********************************
 *
 * Load settings from local configuration file
 *
 ***********************************/
void MainWindow::loadSettings()
{
    settingsFileDir = QApplication::applicationDirPath() + "/config.ini";
    QSettings settings(settingsFileDir, QSettings::IniFormat);
    ui->lineEdit_TcpClientTargetIP->setText(settings.value("TCP_CLIENT_TARGET_IP", "127.0.0.1").toString());
    ui->lineEdit_TcpClientTargetPort->setText(settings.value("TCP_CLIENT_TARGET_PORT", 1234).toString());

    ui->lineEdit_UdpListenPort->setText(settings.value("UDP_LISTEN_PORT", 1234).toString());

    int index = settings.value("interfaceIndex", 0).toInt();
    if (ui->comboBox_Interface->count() >= index)
    {
        ui->comboBox_Interface->setCurrentIndex(index);
        for (int i = 0; i < interfaceList.at(index).addressEntries().size(); ++i)
        {
            if (interfaceList.at(index).addressEntries().at(i).ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                ui->label_LocalIP->setText(interfaceList.at(index).addressEntries().at(i).ip().toString());
            }
        }
    }
    else if (ui->comboBox_Interface->count() > 0 && ui->comboBox_Interface->count() < index)
    {
        ui->comboBox_Interface->setCurrentIndex(0);
        for (int i = 0; i < interfaceList.at(0).addressEntries().size(); ++i)
        {
            if (interfaceList.at(0).addressEntries().at(i).ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                ui->label_LocalIP->setText(interfaceList.at(0).addressEntries().at(i).ip().toString());
            }
        }
    }
}

/***********************************
 *
 * Save settings to local configuration file
 *
 ***********************************/
void MainWindow::saveSettings()
{
    QSettings settings(settingsFileDir, QSettings::IniFormat);
    settings.setValue("TCP_CLIENT_TARGET_IP", ui->lineEdit_TcpClientTargetIP->text());
    settings.setValue("TCP_CLIENT_TARGET_PORT", ui->lineEdit_TcpClientTargetPort->text());

    settings.setValue("UDP_LISTEN_PORT", ui->lineEdit_UdpListenPort->text());

    settings.setValue("INTERFACE_INDEX", ui->comboBox_Interface->currentIndex());

    settings.sync();
}

void MainWindow::onRefreshButtonClicked()
{
    saveSettings();
    findLocalIPs();
    loadSettings();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("ZPeng", "MIMORadar");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);

    waveform.close();
}

void MainWindow::restoreWindowState()
{
    QSettings settings("ZPeng", "MIMORadar");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openWaveform()
{
    //![1]
    QSplineSeries *series = new QSplineSeries();
    series->setName("spline");
    //![1]

    //![2]
    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
    //![2]

    //![3]
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Simple spline chart example");
    chart->createDefaultAxes();
    chart->axisY()->setRange(0, 10);
    //![3]

    //![4]
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    //![4]

    //![5]
    waveform.setCentralWidget(chartView);
    waveform.resize(400, 300);
    waveform.show();
    //![5]
}

void MainWindow::updateWavform()
{

}