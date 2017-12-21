/****************************************************************************************
//    mytcpclient.cpp
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

#include "mytcpclient.h"

MyTCPClient::MyTCPClient(QObject *parent) : QTcpSocket(parent)
{
    tcpSocket = new QTcpSocket();
}

void MyTCPClient::connectTo(QHostAddress addr, quint16 port)
{
    tcpSocket->connectToHost(addr, port);
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
}

void MyTCPClient::onConnected()
{
    disconnect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(messageReady()));
    emit myClientConnected(tcpSocket->peerAddress().toString(), tcpSocket->peerPort());
}

void MyTCPClient::onStateChanged(QAbstractSocket::SocketState state)
{
    disconnect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        emit connectionFailed();
        //qDebug()<<"connecting timeout";
        break;
    case QAbstractSocket::HostLookupState:
        //qDebug()<<"HostLookupState";
        break;
    case QAbstractSocket::ConnectingState:
        //qDebug()<<"ConnectingState";
        break;
    case QAbstractSocket::ConnectedState:
        //qDebug()<<"ConnectedState";
        break;
    case QAbstractSocket::BoundState:
        //qDebug()<<"BoundState";
        break;
    case QAbstractSocket::ListeningState:
        //qDebug()<<"ListeningState";
        break;
    case QAbstractSocket::ClosingState:
        //qDebug()<<"ClosingState";
        break;
    }
}

void MyTCPClient::onDisconnected()
{
    disconnect(tcpSocket, SIGNAL(disconnected()));
    disconnect(tcpSocket, SIGNAL(readyRead()));
    tcpSocket->close();
    emit myClientDisconnected();
}

void MyTCPClient::closeClient()
{
    tcpSocket->close();
}

void MyTCPClient::abortConnection()
{
    tcpSocket->abort();
}

void MyTCPClient::messageReady()
{
    //array = tcpSocket->readAll();
    QString tempString = tcpSocket->readAll();
    //emit newMessage(tcpSocket->peerAddress().toString(), tempString);

    if (tempString.left(7) == "ADCDATA")
    {
        acceptingADCData = true;
        array.clear();
        //array=tempString;
    }

    if (acceptingADCData)
    {
        array.append(tempString);
        //qDebug()<<array.toHex();
    }
    else
    {
        emit newMessage(tcpSocket->peerAddress().toString(), tempString);
    }

    if (array.right(7) == "ADCSTOP")
    {
        //array.append(tempString);
        acceptingADCData = false;
        array = array.mid(7, array.size() - 14);
        for (qint16 i = 0; i < 1024; i++)
        {
            timeStamp.append(((((quint32)array.at(i)) << 16) + (((quint32)array.at(i + 1024)) << 8) + ((quint32)array.at(i + 2048))) >> 6);

            adcData.append(((double)((((((quint32)array.at(i + 2048)) << 8) + ((quint32)array.at(i + 3072))) >> 2) & 0x00000FFF)) / pow(2, 12) * 1.48);
            //adcData.append(((((((quint32) array.at(i+2048))<<8) + ((quint32) array.at(i+3072)))>>2) << 20)>>20);
        }
        emit newMessage(tcpSocket->peerAddress().toString(), array.toHex());
        //qDebug()<<array;
        //array.clear();
        qDebug() << timeStamp;
        qDebug() << array.size();
        qDebug() << adcData;

        array.clear();
        adcData.clear();
        timeStamp.clear();
    }
}

void MyTCPClient::sendMessage(QString string)
{
    QByteArray Data;
    Data.append(string);
    if (tcpSocket->state() == QTcpSocket::ConnectedState)
    {
        tcpSocket->write(Data);
        tcpSocket->flush();
    }
}

void MyTCPClient::disconnectCurrentConnection()
{
    tcpSocket->disconnectFromHost();
}
