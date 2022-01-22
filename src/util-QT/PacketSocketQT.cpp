/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright(c) 2004
 *
 * Author(s):
 *	Volker Fischer, Julian Cable, Oliver Haffenden
 *
 * Description:
 *
 * This is an implementation of the CPacketSocket interface that wraps up a QT socket.
 *
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or(at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#include "PacketSocketQT.h"
#include <QStringList>
#include <QTimer>
#include <QUdpSocket>
#include <QTcpSocket>
#include <iostream>
#include <sstream>

#include <errno.h>
#ifndef _WIN32
# include <netinet/in.h>
# include <arpa/inet.h>
#endif

#ifdef _WIN32
/* Always include winsock2.h before windows.h */
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
#endif

/* Some defines needed for compatibility when using Linux */
#ifndef _WIN32
typedef int SOCKET;
# define SOCKET_ERROR				(-1)
# define INVALID_SOCKET				(-1)
#endif

CPacketSocketQT::CPacketSocketQT():
    pPacketSink(NULL), HostAddrOut(), iHostPortOut(-1),
    writeBuf(),udp(true),
    udpSocket(NULL), tcpSocket(NULL)
{
}

CPacketSocketQT::~CPacketSocketQT()
{
}

// Set the sink which will receive the packets
void
CPacketSocketQT::SetPacketSink(CPacketSink * pSink)
{
    pPacketSink = pSink;
}

// Stop sending packets to the sink
void
CPacketSocketQT::ResetPacketSink(void)
{
    pPacketSink = NULL;
}

// Send packet to the socket
void
CPacketSocketQT::SendPacket(const vector < _BYTE > &vecbydata, uint32_t addr, uint16_t port)
{
    /*int bytes_written;*/
    (void)addr; (void)port;
    if(udpSocket != NULL)
        /*bytes_written =*/ udpSocket->writeDatagram((char*)&vecbydata[0], vecbydata.size(), HostAddrOut, iHostPortOut);
    else if(tcpSocket != NULL)
       /*bytes_written =*/ tcpSocket->write((char*)&vecbydata[0], vecbydata.size());
}

QStringList
CPacketSocketQT::parseDest(const string & strNewAddr)
{
    return QString(strNewAddr.c_str()).split(":", QString::KeepEmptyParts);
}

bool
CPacketSocketQT::SetDestination(const string & strNewAddr)
{
    /* syntax
       1:  <port>                send to port on localhost
       2:  <ip>:<port>           send to port on host or port on m/c group
       3:  <ip>:<ip>:<port>      send to port on m/c group via interface
       prefix with "t" for tcp
     */
    int ttl = 127;
    bool bAddressOK = true;
    QHostAddress AddrInterface;
    QStringList parts = parseDest(strNewAddr);
    QString first = parts[0].toLower();
    if(first.startsWith("t"))
    {
        udp = false;
        parts[0] = first.right(1);
        //qDebug("tcp: %s", first);
    }

    switch(parts.count())
    {
    case 1: // Just a port - send to ourselves
        bAddressOK = HostAddrOut.setAddress("127.0.0.1");
        iHostPortOut = parts[0].toUInt();
        break;
    case 2: // host and port, unicast
        bAddressOK = HostAddrOut.setAddress(parts[0]);
        iHostPortOut = parts[1].toUInt();
        break;
    case 3: // interface, host and port, usually multicast udp
        AddrInterface.setAddress(parts[0]);
        bAddressOK = HostAddrOut.setAddress(parts[1]);
        iHostPortOut = parts[2].toUInt();
        break;
    default:
        bAddressOK = false;
    }
    if(udp)
    {
        if(udpSocket == NULL)
            udpSocket = new QUdpSocket();
# if QT_VERSION < 0x040800
        const SOCKET s = udpSocket->socketDescriptor();
        if(setsockopt(s, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl))==SOCKET_ERROR)
            bAddressOK = false;
        uint32_t mc_if = htonl(AddrInterface.toIPv4Address());
        if(mc_if != 0)
        {
            if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF,
                          (char *) &mc_if, sizeof(mc_if)) == SOCKET_ERROR)
                bAddressOK = false;
        }
# else
        udpSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, ttl);
        if(AddrInterface != QHostAddress(QHostAddress::Any))
             udpSocket->setMulticastInterface(GetInterface(AddrInterface));
# endif
    }
    else
    {
        if(tcpSocket == NULL)
            tcpSocket = new QTcpSocket();
        tcpSocket->connectToHost(HostAddrOut, iHostPortOut);
        bAddressOK = tcpSocket->waitForConnected(5000);
    }
    return bAddressOK;
}

bool
CPacketSocketQT::GetDestination(string & str)
{
    stringstream s;
    s << HostAddrOut.toString().toLatin1().constData() << ":" << iHostPortOut;
    str = s.str();
    return true;
}


bool
CPacketSocketQT::SetOrigin(const string & strNewAddr)
{
    /* syntax (unwanted fields can be empty, e.g. <source ip>::<group ip>:<port>
       1:  <port>
       2:  <group ip>:<port>
       3:  <interface ip>:<group ip>:<port>
       4:  <source ip>:<interface ip>:<group ip>:<port>
       5: - for TCP - no need to separately set origin
     */

    if(strNewAddr == "-")
    {
        udp = false;
        if(tcpSocket == NULL)
            tcpSocket = new QTcpSocket();
        return true;
    }

    if(udpSocket == NULL)
        udpSocket = new QUdpSocket();

    int iPort=-1;
    QHostAddress AddrGroup, AddrInterface, AddrSource;
    QStringList parts = parseDest(strNewAddr);
    bool ok=true;
    switch(parts.count())
    {
    case 1:
        iPort = parts[0].toUInt(&ok);
        break;
    case 2:
        iPort = parts[1].toUInt(&ok);
        ok &= AddrGroup.setAddress(parts[0]);
        break;
    case 3:
        iPort = parts[2].toUInt(&ok);
        if(parts[0].length() > 0)
            ok &= AddrInterface.setAddress(parts[0]);
        if(parts[1].length() > 0)
            ok &= AddrGroup.setAddress(parts[1]);
        break;
    case 4:
        iPort = parts[3].toUInt(&ok);
        if(parts[0].length() > 0)
            ok &= AddrSource.setAddress(parts[0]);
        if(parts[1].length() > 0)
            ok &= AddrInterface.setAddress(parts[1]);
        if(parts[2].length() > 0)
            ok &= AddrGroup.setAddress(parts[2]);
        break;
    default:
        ok = false;
    }

    if(ok)
    {
        return doSetSource(AddrGroup, AddrInterface, iPort, AddrSource);
    }
    return false;
}

bool CPacketSocketQT::doSetSource(QHostAddress AddrGroup, QHostAddress AddrInterface, int iPort, QHostAddress AddrSource)
{
    if(udp)
    {
        if(AddrSource == QHostAddress(QHostAddress::Any))
            sourceAddr = 0;
        else
            sourceAddr = AddrSource.toIPv4Address();
        /* Multicast ? */
        uint32_t gp = AddrGroup.toIPv4Address();
        if(gp == 0)
        {
            /* Initialize the listening socket. */
            udpSocket->bind(AddrInterface, iPort);
        }
        else if((gp & 0xe0000000) == 0xe0000000)	/* multicast! */
        {
            bool ok = udpSocket->bind(iPort, QUdpSocket::ShareAddress);
            if(ok == false)
            {
                throw CGenErr("Can't bind to port to receive packets");
            }
#if QT_VERSION < 0x040800
            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = htonl(AddrGroup.toIPv4Address());
            mreq.imr_interface.s_addr = htonl(AddrInterface.toIPv4Address());
            int n = setsockopt(udpSocket->socketDescriptor(), IPPROTO_IP, IP_ADD_MEMBERSHIP,(char *) &mreq,	sizeof(mreq));
            if(n == SOCKET_ERROR)
                ok = false;
#else
            if(AddrInterface == QHostAddress(QHostAddress::Any))
                ok = udpSocket->joinMulticastGroup(AddrGroup);
            else
                ok = udpSocket->joinMulticastGroup(AddrGroup, GetInterface(AddrInterface));
#endif
            if(!ok)
            {
                qDebug("Can't join multicast group");
                //throw CGenErr(string());
            }
        }
        else /* one address specified, but not multicast - listen on a specific interface */
        {
            /* Initialize the listening socket. */
            udpSocket->bind(AddrGroup, iPort);
        }
    }
    return true;
}

QNetworkInterface
CPacketSocketQT::GetInterface(QHostAddress AddrInterface)
{
    QList<QNetworkInterface> l = QNetworkInterface::allInterfaces () ;
    for(int i=0; i<l.size(); i++) {
        QList<QHostAddress> h = l[i].allAddresses();
        QString s;
        for(int j=0; j<h.size(); j++) {
            s += h[j].toString() + " ";
            if(h[j].toIPv4Address() == AddrInterface.toIPv4Address()) {
                return l[j];
            }
        }
    }
    return QNetworkInterface::allInterfaces().first();
}

void
CPacketSocketQT::poll()
{
    if(udp)
        pollDatagram();
    else
        pollStream();
}

void
CPacketSocketQT::pollStream()
{
    vector < _BYTE > vecbydata(MAX_SIZE_BYTES_NETW_BUF);
    /* Read block from network interface */
    int iNumBytesRead = tcpSocket->read((char *) &vecbydata[0], MAX_SIZE_BYTES_NETW_BUF);
    if(iNumBytesRead > 0)
    {
        /* Decode the incoming packet */
        if(pPacketSink != NULL)
        {
            vecbydata.resize(iNumBytesRead);
            QHostAddress peer = tcpSocket->peerAddress();
            uint32_t addr = peer.toIPv4Address();
            int port = tcpSocket->peerPort();
            if(sourceAddr == 0 || sourceAddr == addr) // optionally filter on source address
                pPacketSink->SendPacket(vecbydata, addr, port);
        }
    }
}

void
CPacketSocketQT::pollDatagram()
{
    vector < _BYTE > vecbydata(MAX_SIZE_BYTES_NETW_BUF);
    while (udpSocket->hasPendingDatagrams()) {
        vecbydata.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram((char*)&vecbydata[0], vecbydata.size(), &sender, &senderPort);

        uint32_t addr = sender.toIPv4Address();
        if(sourceAddr == 0)
            pPacketSink->SendPacket(vecbydata, addr, senderPort);
        else if(sourceAddr == addr) // optionally filter on source address
            pPacketSink->SendPacket(vecbydata, addr, senderPort);
    }
}
