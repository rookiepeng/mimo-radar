# MIMO Radar

MIMO radar program on PC.

## Hardware

The hardware is designed 24-GHz MIMO FMCW radar system. The baseband signal is sample by TI Simplelink CC3200 with sampling rate of 250 ksps. The data is then tranmitted to the PC client through WiFi.

## Firmware

The firmware source code of CC3200 can be found in

```
/CC3200
```

### Build

Build the source code with TI Code Composer Studio (CCS).

Dependency:

* [CC3200 SDK 1.3.0](http://www.ti.com/tool/CC3200SDK)
* [CC3200 Service Pack 1.0.1.11-2.10.0.0](http://www.ti.com/tool/CC3200SDK)

The built binary file must be downloaded into CC3200 by using [Uniflash v3.4.1](http://www.ti.com/tool/UNIFLASH).

## Client

Currently, there is only a PC client avaiable, which is built by Qt5.
The source code of the PC client can be found in

```
/PC Client
```

### Build

The source code can be compiled with [Qt5](https://www.qt.io/) in Windows, Linux and MacOS. Qt Creator can be used.

## Usage

Here are the steps to setup the system:

1. On the PC, search the WiFi hotspot named **MIMO-Radar** and connect it.
2. Open the PC Client, on the **Network** tab, click **Refresh** button to update IP address.
3. Set **TCP IP** to **192.168.1.1**, **TCP Port** to **1234** and **UDP Port** to **1234**.
4. Click **Connect** button to connect with the board.
5. Send **TX <N>** to set the transmitter channel.
6. Send **RX <N>** to set the receiver channel.
7. Send **ADC Start** to record data.
