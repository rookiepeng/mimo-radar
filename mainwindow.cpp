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
    connect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
    connect(ui->button_Refresh, SIGNAL(clicked()), this, SLOT(onRefreshButtonClicked()));

    connect(ui->button_Plot, SIGNAL(clicked(bool)), this, SLOT(openPlot()));

    //connect(myudp, SIGNAL(newData(QVector<float>,QVector<float>)), &plot, SLOT(updatePlot(QVector<float>, QVector<float>)));
    connect(myudp, SIGNAL(newData(QVector<QPointF>)), &plot, SLOT(updatePlot(QVector<QPointF>)));
    connect(myudp, SIGNAL(newData(QVector<QPointF>)), this, SLOT(openPlot()));

    connect(ui->toolButton_SaveTo, SIGNAL(clicked(bool)), this, SLOT(saveToDir()));
}

/***********************************
 *
 * Connect button clicked
 *
 ***********************************/
void MainWindow::onConnectButtonClicked()
{
    disconnect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
    ui->statusBar->showMessage("Connecting to Radar ...", 0);

    if (setupConnection(UDPSERVER))
    {
        ui->lineEdit_UdpListenPort->setDisabled(true);
        connect(myudp, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
        connect(myudp, SIGNAL(newMessage(QString, QVector<float>)), this, SLOT(onAppendMessage(QString, QVector<float>)));

        if (setupConnection(TCPCLIENT))
        {
            ui->lineEdit_TcpClientTargetIP->setDisabled(true);
            ui->lineEdit_TcpClientTargetPort->setDisabled(true);
            ui->button_Connect->setText("Stop");

            connect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
            connect(mytcpclient, SIGNAL(myClientConnected(QString, quint16)), this, SLOT(onTcpClientNewConnection(QString, quint16)));
            connect(mytcpclient, SIGNAL(connectionFailed()), this, SLOT(onTcpClientTimeOut()));
        }
    }
    else
    {
        connect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
        ui->statusBar->showMessage("Disconnected", 0);

        msgBox.setText("UDP ERROR: Failed to listen to the UDP port.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
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
    disconnect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
    connect(mytcpclient, SIGNAL(myClientDisconnected()), this, SLOT(onDisconnected()));

    ui->button_Connect->setDisabled(false);
    ui->button_Connect->setText("Disconnect");

    ui->button_TcpClientSend->setDisabled(false);
    ui->lineEdit_TcpClientSend->setDisabled(false);
    ui->textBrowser_TcpClientMessage->setDisabled(false);

    connect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onDisconnectButtonClicked()));

    connect(mytcpclient, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    connect(ui->button_TcpClientSend, SIGNAL(clicked()), this, SLOT(onTcpClientSendMessage()));
    connect(ui->lineEdit_TcpClientSend, SIGNAL(returnPressed()), this, SLOT(onTcpClientSendMessage()));

    ui->statusBar->showMessage("Connected to Radar: " + from + ": " + QString::number(port), 0);
}

/***********************************
 *
 * Stop button clicked
 *
 ***********************************/
void MainWindow::onStopButtonClicked()
{
    disconnect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));

    ui->statusBar->showMessage("Disconnected", 0);
    disconnect(mytcpclient, SIGNAL(myClientConnected(QString, quint16)), this, SLOT(onTcpClientNewConnection(QString, quint16)));
    disconnect(mytcpclient, SIGNAL(connectionFailed()), this, SLOT(onTcpClientTimeOut()));
    ui->button_Connect->setText("Connect");
    mytcpclient->abortConnection();
    ui->lineEdit_TcpClientTargetIP->setDisabled(false);
    ui->lineEdit_TcpClientTargetPort->setDisabled(false);

    ui->button_TcpClientSend->setDisabled(true);
    ui->lineEdit_TcpClientSend->setDisabled(true);
    ui->textBrowser_TcpClientMessage->setDisabled(true);

    disconnect(myudp, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    disconnect(myudp, SIGNAL(newMessage(QString, QVector<float>)), this, SLOT(onAppendMessage(QString, QVector<float>)));
    myudp->unbindPort();
    ui->lineEdit_UdpListenPort->setDisabled(false);

    connect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
}

/***********************************
 *
 * TCP client connection time out
 *
 ***********************************/
void MainWindow::onTcpClientTimeOut()
{
    disconnect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
    disconnect(mytcpclient, SIGNAL(myClientConnected(QString, quint16)), this, SLOT(onTcpClientNewConnection(QString, quint16)));
    disconnect(mytcpclient, SIGNAL(connectionFailed()), this, SLOT(onTcpClientTimeOut()));

    ui->button_Connect->setText("Connect");
    ui->lineEdit_TcpClientTargetIP->setDisabled(false);
    ui->lineEdit_TcpClientTargetPort->setDisabled(false);

    disconnect(myudp, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    disconnect(myudp, SIGNAL(newMessage(QString, QVector<float>)), this, SLOT(onAppendMessage(QString, QVector<float>)));
    myudp->unbindPort();
    ui->lineEdit_UdpListenPort->setDisabled(false);

    mytcpclient->closeClient();
    connect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));

    msgBox.setText("TCP ERROR: Connection time out.");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

/***********************************
 *
 * Diconnect button clicked
 *
 ***********************************/
void MainWindow::onDisconnectButtonClicked()
{
    mytcpclient->disconnectCurrentConnection();
}

/***********************************
 *
 * Disconnected
 *
 ***********************************/
void MainWindow::onDisconnected()
{
    ui->statusBar->showMessage("Disconnected", 0);
    disconnect(mytcpclient, SIGNAL(myClientDisconnected()), this, SLOT(onDisconnected()));
    disconnect(mytcpclient, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    disconnect(ui->button_TcpClientSend, SIGNAL(clicked()), this, SLOT(onTcpClientSendMessage()));
    disconnect(ui->lineEdit_TcpClientSend, SIGNAL(returnPressed()), this, SLOT(onTcpClientSendMessage()));
    disconnect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onDisconnectButtonClicked()));
    ui->button_Connect->setText("Connect");

    ui->button_TcpClientSend->setDisabled(true);
    ui->lineEdit_TcpClientSend->setDisabled(true);
    ui->textBrowser_TcpClientMessage->setDisabled(true);

    ui->button_Connect->setDisabled(false);
    ui->lineEdit_TcpClientTargetIP->setDisabled(false);
    ui->lineEdit_TcpClientTargetPort->setDisabled(false);

    mytcpclient->closeClient();
    mytcpclient->close();

    // UDP
    disconnect(myudp, SIGNAL(newMessage(QString, QString)), this, SLOT(onAppendMessage(QString, QString)));
    disconnect(myudp, SIGNAL(newMessage(QString, QVector<float>)), this, SLOT(onAppendMessage(QString, QVector<float>)));
    myudp->unbindPort();
    ui->lineEdit_UdpListenPort->setDisabled(false);

    connect(ui->button_Connect, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
}

/***********************************
 *
 * Append a message to message browser
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
    else if (from == "Me")
    {
        QTextTable *table = cursor.insertTable(1, 2, tableFormatSend);
        table->cellAt(0, 0).firstCursorPosition().insertText('<' + from + "> ");
        table->cellAt(0, 1).firstCursorPosition().insertText(message);
    }
    else
    {
        QTextTable *table = cursor.insertTable(1, 2, tableFormatReceive);
        table->cellAt(0, 0).firstCursorPosition().insertText('<' + from + "> ");
        table->cellAt(0, 1).firstCursorPosition().insertText(message);
    }
    QScrollBar *bar = ui->textBrowser_TcpClientMessage->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::onAppendMessage(const QString &from, const QVector<float> &data)
{
    //if (from.isEmpty() || message.isEmpty())
    //{
    //    return;
    //}

    QTextCursor cursor(ui->textBrowser_TcpClientMessage->textCursor());
    cursor.movePosition(QTextCursor::End);

    QTextTable *table = cursor.insertTable(1, 2, tableFormatData);
    table->cellAt(0, 0).firstCursorPosition().insertText('<' + from + "> ");

    QString dataString;
    qint16 i;
    for (i = 0; i < data.size() - 1; i++)
    {
        dataString.append(QString::number(data.at(i)) + ", ");
    }
    dataString.append(QString::number(data.at(i)));

    table->cellAt(0, 1).firstCursorPosition().insertText(dataString);

    QScrollBar *bar = ui->textBrowser_TcpClientMessage->verticalScrollBar();
    bar->setValue(bar->maximum());

    //qDebug()<<QDateTime::currentDateTime().toString("MMddyyyy_hhmmss");

    QString filename = fileDir + "/TX_" + txChannel + " RX_" + rxChannel + " " + QDateTime::currentDateTime().toString("MMddyyyy_hhmmss");
    QFile fileout(filename);
    if (fileout.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream out(&fileout);
        out << dataString;
        fileout.close();
    }
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

    if ((QString::compare("TX", text.left(2), Qt::CaseInsensitive)) == 0)
    {
        bool ok;
        //qDebug()<<text.mid(2, text.length()-2).toUInt(&ok,16);
        txChannel = QString::number(text.mid(2, text.length() - 2).toUInt(&ok, 16), 16);
    }
    else if ((QString::compare("RX", text.left(2), Qt::CaseInsensitive)) == 0)
    {
        bool ok;
        rxChannel = QString::number(text.mid(2, text.length() - 2).toUInt(&ok, 16), 16);
    }
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

    tableFormatSend.setBorder(0);

    tableFormatReceive.setBorder(0);
    tableFormatReceive.setBackground(QColor("#E0F7FA"));

    tableFormatData.setBorder(0);
    tableFormatData.setBackground(QColor("#FCE4EC"));

    ui->tabWidget->setCurrentIndex(0);

    ui->label_AppVersion->setText(APPVERSION);
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

    ui->lineEdit_SaveTo->setText(settings.value("DATADIR", QDir::currentPath()).toString());
    fileDir = ui->lineEdit_SaveTo->text();

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

    settings.setValue("DATADIR", ui->lineEdit_SaveTo->text());

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

    plot.close();
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

void MainWindow::openPlot()
{
    if (plot.isHidden())
    {
        plot.show();
    }
}

void MainWindow::saveToDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Save File"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
    {
    }
    else
    {
        fileDir = dir;
        ui->lineEdit_SaveTo->setText(fileDir);
    }
    //qDebug() << fileDir;
    //qDebug() << QDir::currentPath();
}
