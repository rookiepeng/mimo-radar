/****************************************************************************************
//    myudp.cpp
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

#include "myudp.h"

MyUDP::MyUDP(QObject *parent) : QUdpSocket(parent)
{
    socket = new QUdpSocket();
}

bool MyUDP::bindPort(QHostAddress addr, qint16 port)
{
    socket->abort();
    bool isBinded = socket->bind(addr, port);
    if (isBinded)
    {
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }
    return isBinded;
}

void MyUDP::sendMessage(QHostAddress sender, quint16 senderPort, QString string)
{
    QByteArray Data;
    Data.append(string);

    // Sends the datagram datagram
    // to the host address and at port.
    socket->writeDatagram(Data, sender, senderPort);
}

void MyUDP::readyRead()
{
    // when data comes in
    //QByteArray ADCSeg3;
    //QByteArray ADCSeg2;
    //QByteArray ADCSeg1;
    //QByteArray ADCSeg0;

    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    socket->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);

    if (buffer.left(7) == "ADCDATA")
    {
        acceptingADCData = true;
        array.clear();
    }

    if (acceptingADCData)
    {
        array.append(buffer);
        //qDebug()<<array.toHex();
    }
    else
    {
        emit newMessage(sender.toString(), buffer);
    }

    if (array.right(7) == "ADCSTOP")
    {
        //array.append(buffer);
        acceptingADCData = false;
        //qDebug()<<array.size();
        qDebug() << (qint32)array.at(0);
        array = array.mid(7, array.size() - 14);

        for (qint16 i = 0; i < 1024; i++)
        {
            //timeStamp.append(((((quint32)array.at(i)) << 16) + (((quint32)array.at(i + 1024)) << 8) + ((quint32)array.at(i + 2048))) >> 6);
            adcData.append(((double)((((((quint32)array.at(i)) << 8) + ((quint32)array.at(i + 1024))) >> 2) & 0x00000FFF)) / pow(2, 12) * 1.48);
            //adcData.append(((((((quint32) array.at(i+2048))<<8) + ((quint32) array.at(i+3072)))>>2) << 20)>>20);
        }
        emit newMessage(sender.toString(), array.toHex());
        //qDebug()<<timeStamp;
        qDebug() << array.size();
        qDebug() << adcData;

        array.clear();
        adcData.clear();
        //timeStamp.clear();
        //qDebug()<<array.right(7);
    }
}

void MyUDP::unbindPort()
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    socket->close();
}
