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

    for (quint16 i = 0; i < 8192; i++)
    {
        timeStamp.append(i / 250.0);
    }
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
    }
    else
    {
        emit newMessage(sender.toString(), buffer);
    }

    if (array.right(7) == "ADCSTOP")
    {
        acceptingADCData = false;
        array = array.mid(7, array.size() - 14);
        qint32 i;
        for (i = 0; i < 1024; i+=8)
        {
            // ch3 1
            timeStampMCU.append(((((quint32)array.at(i+1+1024 * 12)) << 16) + (((quint32)array.at(i+1 + 1024*13)) << 8) + ((quint32)array.at(i+1 + 1024*14))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+1 + 1024 * 10)) << 8) + ((quint16)array.at(i+1 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch0 2
            timeStampMCU.append(((((quint32)array.at(i+2)) << 16) + (((quint32)array.at(i+2 + 1024)) << 8) + ((quint32)array.at(i+2 + 1024*2))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+2 + 1024 * 2)) << 8) + ((quint16)array.at(i+2 + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch1 2
            timeStampMCU.append(((((quint32)array.at(i+2+1024 * 4)) << 16) + (((quint32)array.at(i+2 + 1024*5)) << 8) + ((quint32)array.at(i+2 + 1024*6))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+2 + 1024 * 6)) << 8) + ((quint16)array.at(i+2 + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch0 1
            timeStampMCU.append(((((quint32)array.at(i+1)) << 16) + (((quint32)array.at(i+1 + 1024)) << 8) + ((quint32)array.at(i+1 + 1024*2))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+1 + 1024 * 2)) << 8) + ((quint16)array.at(i+1 + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch1 1
            timeStampMCU.append(((((quint32)array.at(i+1+1024 * 4)) << 16) + (((quint32)array.at(i+1 + 1024*5)) << 8) + ((quint32)array.at(i+1 + 1024*6))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+1 + 1024 * 6)) << 8) + ((quint16)array.at(i+1 + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch2 1
            timeStampMCU.append(((((quint32)array.at(i+1+1024 * 8)) << 16) + (((quint32)array.at(i+1 + 1024*9)) << 8) + ((quint32)array.at(i+1 + 1024*10))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+1 + 1024 * 10)) << 8) + ((quint16)array.at(i+1 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);

            // ch1 3
            timeStampMCU.append(((((quint32)array.at(i+3+1024 * 4)) << 16) + (((quint32)array.at(i+3 + 1024*5)) << 8) + ((quint32)array.at(i+3 + 1024*6))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+3 + 1024 * 6)) << 8) + ((quint16)array.at(i+3 + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch2 3
            timeStampMCU.append(((((quint32)array.at(i+3+1024 * 8)) << 16) + (((quint32)array.at(i+3 + 1024*9)) << 8) + ((quint32)array.at(i+3 + 1024*10))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+3 + 1024 * 10)) << 8) + ((quint16)array.at(i+3 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch3 3
            timeStampMCU.append(((((quint32)array.at(i+3+1024 * 12)) << 16) + (((quint32)array.at(i+3 + 1024*13)) << 8) + ((quint32)array.at(i+3 + 1024*14))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+3 + 1024 * 10)) << 8) + ((quint16)array.at(i+3 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch2 2
            timeStampMCU.append(((((quint32)array.at(i+2+1024 * 8)) << 16) + (((quint32)array.at(i+2 + 1024*9)) << 8) + ((quint32)array.at(i+2 + 1024*10))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+2 + 1024 * 10)) << 8) + ((quint16)array.at(i+2 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch3 2
            timeStampMCU.append(((((quint32)array.at(i+2+1024 * 12)) << 16) + (((quint32)array.at(i+2 + 1024*13)) << 8) + ((quint32)array.at(i+2 + 1024*14))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+2 + 1024 * 10)) << 8) + ((quint16)array.at(i+2 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch0 3
            timeStampMCU.append(((((quint32)array.at(i+3)) << 16) + (((quint32)array.at(i+3 + 1024)) << 8) + ((quint32)array.at(i+3 + 1024*2))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+3 + 1024 * 2)) << 8) + ((quint16)array.at(i+3 + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);

            // ch0 5
            timeStampMCU.append(((((quint32)array.at(i+5)) << 16) + (((quint32)array.at(i+5 + 1024)) << 8) + ((quint32)array.at(i+5 + 1024*2))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+5 + 1024 * 2)) << 8) + ((quint16)array.at(i+5 + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch1 5
            timeStampMCU.append(((((quint32)array.at(i+5+1024 * 4)) << 16) + (((quint32)array.at(i+5 + 1024*5)) << 8) + ((quint32)array.at(i+5 + 1024*6))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+5 + 1024 * 6)) << 8) + ((quint16)array.at(i+5 + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch2 5
            timeStampMCU.append(((((quint32)array.at(i+5+1024 * 8)) << 16) + (((quint32)array.at(i+5 + 1024*9)) << 8) + ((quint32)array.at(i+5 + 1024*10))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+5 + 1024 * 10)) << 8) + ((quint16)array.at(i+5 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch0 4
            timeStampMCU.append(((((quint32)array.at(i+4)) << 16) + (((quint32)array.at(i+4 + 1024)) << 8) + ((quint32)array.at(i+4 + 1024*2))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+4 + 1024 * 2)) << 8) + ((quint16)array.at(i+4 + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch1 4
            timeStampMCU.append(((((quint32)array.at(i+4+1024 * 4)) << 16) + (((quint32)array.at(i+4 + 1024*5)) << 8) + ((quint32)array.at(i+4 + 1024*6))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+4 + 1024 * 6)) << 8) + ((quint16)array.at(i+4 + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch2 4
            timeStampMCU.append(((((quint32)array.at(i+4+1024 * 8)) << 16) + (((quint32)array.at(i+4 + 1024*9)) << 8) + ((quint32)array.at(i+4 + 1024*10))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+4 + 1024 * 10)) << 8) + ((quint16)array.at(i+4 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch3 4
            timeStampMCU.append(((((quint32)array.at(i+4+1024 * 12)) << 16) + (((quint32)array.at(i+4 + 1024*13)) << 8) + ((quint32)array.at(i+4 + 1024*14))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+4 + 1024 * 10)) << 8) + ((quint16)array.at(i+4 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);

            // ch2 6
            timeStampMCU.append(((((quint32)array.at(i+6+1024 * 8)) << 16) + (((quint32)array.at(i+6 + 1024*9)) << 8) + ((quint32)array.at(i+6 + 1024*10))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+6 + 1024 * 10)) << 8) + ((quint16)array.at(i+6 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch3 6
            timeStampMCU.append(((((quint32)array.at(i+6+1024 * 12)) << 16) + (((quint32)array.at(i+6 + 1024*13)) << 8) + ((quint32)array.at(i+6 + 1024*14))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+6 + 1024 * 10)) << 8) + ((quint16)array.at(i+6 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch0 7
            timeStampMCU.append(((((quint32)array.at(i+7)) << 16) + (((quint32)array.at(i+7 + 1024)) << 8) + ((quint32)array.at(i+7 + 1024*2))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+7 + 1024 * 2)) << 8) + ((quint16)array.at(i+7 + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch3 5
            timeStampMCU.append(((((quint32)array.at(i+5+1024 * 12)) << 16) + (((quint32)array.at(i+5 + 1024*13)) << 8) + ((quint32)array.at(i+5 + 1024*14))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+5 + 1024 * 10)) << 8) + ((quint16)array.at(i+5 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch0 6
            timeStampMCU.append(((((quint32)array.at(i+6)) << 16) + (((quint32)array.at(i+6 + 1024)) << 8) + ((quint32)array.at(i+6 + 1024*2))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+6 + 1024 * 2)) << 8) + ((quint16)array.at(i+6 + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch1 6
            timeStampMCU.append(((((quint32)array.at(i+6+1024 * 4)) << 16) + (((quint32)array.at(i+6 + 1024*5)) << 8) + ((quint32)array.at(i+6 + 1024*6))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+6 + 1024 * 6)) << 8) + ((quint16)array.at(i+6 + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);

            // ch0 8
            timeStampMCU.append(((((quint32)array.at(i+8)) << 16) + (((quint32)array.at(i+8 + 1024)) << 8) + ((quint32)array.at(i+8 + 1024*2))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+8 + 1024 * 2)) << 8) + ((quint16)array.at(i+8 + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch1 8
            timeStampMCU.append(((((quint32)array.at(i+8+1024 * 4)) << 16) + (((quint32)array.at(i+8 + 1024*5)) << 8) + ((quint32)array.at(i+8 + 1024*6))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+8 + 1024 * 6)) << 8) + ((quint16)array.at(i+8 + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch2 8
            timeStampMCU.append(((((quint32)array.at(i+8+1024 * 8)) << 16) + (((quint32)array.at(i+8 + 1024*9)) << 8) + ((quint32)array.at(i+8 + 1024*10))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+8 + 1024 * 10)) << 8) + ((quint16)array.at(i+8 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch3 8
            timeStampMCU.append(((((quint32)array.at(i+8+1024 * 12)) << 16) + (((quint32)array.at(i+8 + 1024*13)) << 8) + ((quint32)array.at(i+8 + 1024*14))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+8 + 1024 * 10)) << 8) + ((quint16)array.at(i+8 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch1 7
            timeStampMCU.append(((((quint32)array.at(i+7+1024 * 4)) << 16) + (((quint32)array.at(i+7 + 1024*5)) << 8) + ((quint32)array.at(i+7 + 1024*6))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+7 + 1024 * 6)) << 8) + ((quint16)array.at(i+7 + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch2 7
            timeStampMCU.append(((((quint32)array.at(i+7+1024 * 8)) << 16) + (((quint32)array.at(i+7 + 1024*9)) << 8) + ((quint32)array.at(i+7 + 1024*10))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+7 + 1024 * 10)) << 8) + ((quint16)array.at(i+7 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            // ch3 7
            timeStampMCU.append(((((quint32)array.at(i+7+1024 * 12)) << 16) + (((quint32)array.at(i+7 + 1024*13)) << 8) + ((quint32)array.at(i+7 + 1024*14))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i+7 + 1024 * 10)) << 8) + ((quint16)array.at(i+7 + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);

            // Ping
            //adcData.append(((float)((((((quint16)array.at(i + 1024 * 2)) << 8) + ((quint16)array.at(i + 1024 * 3))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            //adcData.append(((float)((((((quint16)array.at(i + 1024 * 6)) << 8) + ((quint16)array.at(i + 1024 * 7))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            //adcData.append(((float)((((((quint16)array.at(i + 1024 * 10)) << 8) + ((quint16)array.at(i + 1024 * 11))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            //adcData.append(((float)((((((quint16)array.at(i + 1024 * 14)) << 8) + ((quint16)array.at(i + 1024 * 15))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
        }

        // ch3 1
        timeStampMCU.append(((((quint32)array.at(i+1+1024 * 12)) << 16) + (((quint32)array.at(i+1 + 1024*13)) << 8) + ((quint32)array.at(i+1 + 1024*14))) >> 6);
        // ch0 2
        timeStampMCU.append(((((quint32)array.at(i+2)) << 16) + (((quint32)array.at(i+2 + 1024)) << 8) + ((quint32)array.at(i+2 + 1024*2))) >> 6);
        // ch1 2
        timeStampMCU.append(((((quint32)array.at(i+2+1024 * 4)) << 16) + (((quint32)array.at(i+2 + 1024*5)) << 8) + ((quint32)array.at(i+2 + 1024*6))) >> 6);
        // ch0 1
        timeStampMCU.append(((((quint32)array.at(i+1)) << 16) + (((quint32)array.at(i+1 + 1024)) << 8) + ((quint32)array.at(i+1 + 1024*2))) >> 6);
        // ch1 1
        timeStampMCU.append(((((quint32)array.at(i+1+1024 * 4)) << 16) + (((quint32)array.at(i+1 + 1024*5)) << 8) + ((quint32)array.at(i+1 + 1024*6))) >> 6);
        // ch2 1
        timeStampMCU.append(((((quint32)array.at(i+1+1024 * 8)) << 16) + (((quint32)array.at(i+1 + 1024*9)) << 8) + ((quint32)array.at(i+1 + 1024*10))) >> 6);

        // ch1 3
        timeStampMCU.append(((((quint32)array.at(i+3+1024 * 4)) << 16) + (((quint32)array.at(i+3 + 1024*5)) << 8) + ((quint32)array.at(i+3 + 1024*6))) >> 6);
        // ch2 3
        timeStampMCU.append(((((quint32)array.at(i+3+1024 * 8)) << 16) + (((quint32)array.at(i+3 + 1024*9)) << 8) + ((quint32)array.at(i+3 + 1024*10))) >> 6);
        // ch3 3
        timeStampMCU.append(((((quint32)array.at(i+3+1024 * 12)) << 16) + (((quint32)array.at(i+3 + 1024*13)) << 8) + ((quint32)array.at(i+3 + 1024*14))) >> 6);
        // ch2 2
        timeStampMCU.append(((((quint32)array.at(i+2+1024 * 8)) << 16) + (((quint32)array.at(i+2 + 1024*9)) << 8) + ((quint32)array.at(i+2 + 1024*10))) >> 6);
        // ch3 2
        timeStampMCU.append(((((quint32)array.at(i+2+1024 * 12)) << 16) + (((quint32)array.at(i+2 + 1024*13)) << 8) + ((quint32)array.at(i+2 + 1024*14))) >> 6);
        // ch0 3
        timeStampMCU.append(((((quint32)array.at(i+3)) << 16) + (((quint32)array.at(i+3 + 1024)) << 8) + ((quint32)array.at(i+3 + 1024*2))) >> 6);

        // ch0 5
        timeStampMCU.append(((((quint32)array.at(i+5)) << 16) + (((quint32)array.at(i+5 + 1024)) << 8) + ((quint32)array.at(i+5 + 1024*2))) >> 6);
        // ch1 5
        timeStampMCU.append(((((quint32)array.at(i+5+1024 * 4)) << 16) + (((quint32)array.at(i+5 + 1024*5)) << 8) + ((quint32)array.at(i+5 + 1024*6))) >> 6);
        // ch2 5
        timeStampMCU.append(((((quint32)array.at(i+5+1024 * 8)) << 16) + (((quint32)array.at(i+5 + 1024*9)) << 8) + ((quint32)array.at(i+5 + 1024*10))) >> 6);
        // ch0 4
        timeStampMCU.append(((((quint32)array.at(i+4)) << 16) + (((quint32)array.at(i+4 + 1024)) << 8) + ((quint32)array.at(i+4 + 1024*2))) >> 6);
        // ch1 4
        timeStampMCU.append(((((quint32)array.at(i+4+1024 * 4)) << 16) + (((quint32)array.at(i+4 + 1024*5)) << 8) + ((quint32)array.at(i+4 + 1024*6))) >> 6);
        // ch2 4
        timeStampMCU.append(((((quint32)array.at(i+4+1024 * 8)) << 16) + (((quint32)array.at(i+4 + 1024*9)) << 8) + ((quint32)array.at(i+4 + 1024*10))) >> 6);
        // ch3 4
        timeStampMCU.append(((((quint32)array.at(i+4+1024 * 12)) << 16) + (((quint32)array.at(i+4 + 1024*13)) << 8) + ((quint32)array.at(i+4 + 1024*14))) >> 6);

        // ch2 6
        timeStampMCU.append(((((quint32)array.at(i+6+1024 * 8)) << 16) + (((quint32)array.at(i+6 + 1024*9)) << 8) + ((quint32)array.at(i+6 + 1024*10))) >> 6);
        // ch3 6
        timeStampMCU.append(((((quint32)array.at(i+6+1024 * 12)) << 16) + (((quint32)array.at(i+6 + 1024*13)) << 8) + ((quint32)array.at(i+6 + 1024*14))) >> 6);
        // ch0 7
        timeStampMCU.append(((((quint32)array.at(i+7)) << 16) + (((quint32)array.at(i+7 + 1024)) << 8) + ((quint32)array.at(i+7 + 1024*2))) >> 6);
        // ch3 5
        timeStampMCU.append(((((quint32)array.at(i+5+1024 * 12)) << 16) + (((quint32)array.at(i+5 + 1024*13)) << 8) + ((quint32)array.at(i+5 + 1024*14))) >> 6);
        // ch0 6
        timeStampMCU.append(((((quint32)array.at(i+6)) << 16) + (((quint32)array.at(i+6 + 1024)) << 8) + ((quint32)array.at(i+6 + 1024*2))) >> 6);
        // ch1 6
        timeStampMCU.append(((((quint32)array.at(i+6+1024 * 4)) << 16) + (((quint32)array.at(i+6 + 1024*5)) << 8) + ((quint32)array.at(i+6 + 1024*6))) >> 6);


        for (qint32 i = 0; i < 1024; i++)
        {
            // Pong
            //timeStampMCU.append(((((quint32)array.at(i+1024 * 16)) << 16) + (((quint32)array.at(i + 1024*17)) << 8) + ((quint32)array.at(i + 1024*18))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i + 1024 * 18)) << 8) + ((quint16)array.at(i + 1024 * 19))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            //timeStampMCU.append(((((quint32)array.at(i+1024 * 20)) << 16) + (((quint32)array.at(i + 1024*21)) << 8) + ((quint32)array.at(i + 1024*22))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i + 1024 * 22)) << 8) + ((quint16)array.at(i + 1024 * 23))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            //timeStampMCU.append(((((quint32)array.at(i+1024 * 24)) << 16) + (((quint32)array.at(i + 1024*25)) << 8) + ((quint32)array.at(i + 1024*26))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i + 1024 * 26)) << 8) + ((quint16)array.at(i + 1024 * 27))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
            //timeStampMCU.append(((((quint32)array.at(i+1024 * 28)) << 16) + (((quint32)array.at(i + 1024*29)) << 8) + ((quint32)array.at(i + 1024*30))) >> 6);
            adcData.append(((float)((((((quint16)array.at(i + 1024 * 30)) << 8) + ((quint16)array.at(i + 1024 * 31))) >> 2) & 0x0FFF)) / pow(2, 12) * 1.48);
        }
        emit newMessage(sender.toString(), adcData);

        // prepare data for plotting
        for (quint16 i = 0; i < 8192; i++)
        {
            plotData.append(QPointF(timeStamp.at(i), adcData.at(i)));
        }
        emit newData(plotData);

        //qDebug() << adcData.size();
        //qDebug() << adcData.mid(0, 1024);
        //qDebug() << timeStamp.mid(0, 1024);
        //qDebug() << adcData.mid(1024,1024);
        //qDebug() << adcData.mid(1024*2,1024);
        //qDebug() << adcData.mid(1024*3,1024);
        //qDebug() << adcData.mid(1024*4,1024);
        //qDebug() << adcData.mid(1024*5,1024);
        //qDebug() << adcData.mid(1024*6,1024);
        //qDebug() << adcData.mid(1024 * 7, 1024);
        qDebug() << timeStampMCU;

        array.clear();
        adcData.clear();
        plotData.clear();
        timeStampMCU.clear();
    }
}

void MyUDP::unbindPort()
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    socket->close();
}
