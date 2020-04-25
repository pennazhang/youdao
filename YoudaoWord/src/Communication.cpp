#include "Communication.h"
#include <QMetaObject>
#include <QFileInfo>
#include <QMessageBox>
#include <QString>
#include <QWidget>
#include <QBuffer>

Communication *g_pCommunication;


void FormatString(char lpszBuffer[501], BYTE * pucMsg, int len)
{
    int count = 0, i;
    char lpszTemp[] =  "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF ";

	count = len;
	if (count > 500 / 3)
	{
		count = 500 / 3;
	}

	for (i = 0; i < count; i++)
	{
		memcpy(lpszBuffer + 3 * i, lpszTemp + 3 * pucMsg[i], 3);
//			sprintf(g_lpszFormatString + 3 * i, "%02x ", (int)(pucContent[i]));
	}

	lpszBuffer[count * 3] = 0;
	return;
}

Communication::Communication()
{
	m_nSocketCount = 0;
}

void Communication::createSocket(QWidget *pWidget, int localPort, int remoteBoardcastPort)
{
	QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
	
	//获取所有网络接口的列表
	foreach(QNetworkInterface interface,list)
	{  
		QList<QNetworkAddressEntry> entryList = interface.addressEntries();

		//获取IP地址条目列表，每个条目中包含一个IP地址，一个子网掩码和一个广播地址
		foreach(QNetworkAddressEntry entry,entryList)

		{//遍历每一个IP地址条目
			//IP地址
			QHostAddress localAddr = entry.ip();
			if ((localAddr.protocol() ==  QAbstractSocket::IPv4Protocol) && (localAddr.isLoopback() == false))
			{
				QUdpSocket *pUdpSocket = new QUdpSocket(pWidget);
				pUdpSocket->bind(localAddr, localPort);
				m_pSocket[m_nSocketCount].m_localAddr = localAddr;
				m_pSocket[m_nSocketCount].m_localPort = localPort;
				m_pSocket[m_nSocketCount].m_boardcastAddr = entry.broadcast();
				m_pSocket[m_nSocketCount].m_boardcastPort = remoteBoardcastPort;
				m_pSocket[m_nSocketCount].m_pUdpSocket = pUdpSocket;
				qDebug()<<"IPAddr" << localAddr.toString() << ", Broadcast: " << entry.broadcast().toString();
				switch (m_nSocketCount)
				{
				case 0:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_0()));
					break;
				case 1:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_1()));
					break;
				case 2:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_2()));
					break;
				case 3:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_3()));
					break;
				case 4:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_4()));
					break;
				case 5:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_5()));
					break;
				case 6:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_6()));
					break;
				case 7:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_7()));
					break;
				case 8:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_8()));
					break;
				case 9:
					QObject::connect(pUdpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvData_9()));
					break;
				}
				m_nSocketCount++;
			}
		}
	}

	m_pTcpSocket = new QTcpSocket(pWidget);
	QObject::connect(m_pTcpSocket, SIGNAL(connected()), pWidget, SLOT(onDeviceConnected()));
	QObject::connect(m_pTcpSocket, SIGNAL(readyRead()), pWidget, SLOT(onRecvTCPData()));
}

void Communication::disconnectDevice()
{
	m_pTcpSocket->close();
}

bool Communication::isDeviceConnected()
{
	return (m_pTcpSocket->state() == QAbstractSocket::ConnectedState);
}

Communication::~Communication(void)
{
	for (int i = 0; i < m_nSocketCount; i++)
	{
		m_pSocket[i].m_pUdpSocket->close();
	}

	m_pTcpSocket->close();
}


void Communication::refreshDevice(void)
{
	SOCKET_INFO_MSG msg;
	msg.m_cmdID = CMD_ID_CHECK_SOCKET_INFO;

	for (int i = 0; i < m_nSocketCount; i++)
	{
		msg.m_socketInfo.m_sendAddr = m_pSocket[i].m_localAddr.toIPv4Address();
		msg.m_socketInfo.m_sendPort = m_pSocket[i].m_localPort;
		msg.m_socketInfo.m_recvAddr = m_pSocket[i].m_boardcastAddr.toIPv4Address();
		msg.m_socketInfo.m_recvPort = m_pSocket[i].m_boardcastPort;

		m_pSocket[i].m_pUdpSocket->writeDatagram((char *)&msg, sizeof(SOCKET_INFO_MSG), m_pSocket[i].m_boardcastAddr, m_pSocket[i].m_boardcastPort);
	}
}

void Communication::connectToDevice(QHostAddress deviceAddr, int devicePort)
{
	m_pTcpSocket->connectToHost(deviceAddr, devicePort);
}

void Communication::getBookList()
{
	GET_BOOK_LIST_MSG msg;

	msg.m_magicNumber = MAGIC_NUMBER;
	msg.m_size = sizeof(GET_BOOK_LIST_MSG);
	msg.m_cmdID = CMD_ID_GET_BOOK_LIST;

	m_pTcpSocket->write((char *)&msg, sizeof(GET_BOOK_LIST_MSG));
}

void Communication::backupFile(int bookIndex)
{
	COPY_BOOK_FROM_DEVICE_MSG msg;
	msg.m_magicNumber = MAGIC_NUMBER;
	msg.m_size = sizeof(COPY_BOOK_FROM_DEVICE_MSG);
	msg.m_cmdID = CMD_ID_COPY_FROM_DEVICE;
	msg.m_bookIndex = bookIndex;
	m_pTcpSocket->write((const char *)&msg, sizeof(COPY_BOOK_FROM_DEVICE_MSG));
}

void Communication::deleteFile(int bookIndex)
{
	DELETE_BOOK_FROM_DEVICE_MSG msg;
	msg.m_magicNumber = MAGIC_NUMBER;
	msg.m_size = sizeof(DELETE_BOOK_FROM_DEVICE_MSG);
	msg.m_cmdID = CMD_ID_DELETE_FROM_DEVICE;
	msg.m_bookIndex = bookIndex;
	m_pTcpSocket->write((const char *)&msg, msg.m_size );
}

void Communication::copyFileNameToDevice(QString dbPath)
{

}

void Communication::copyFileToDevice(QString dbPath)
{
	QFileInfo fi(dbPath);
	if (!fi.exists())
	{
		QMessageBox::warning(nullptr, QString("File Copy Failed"), QString("Can't find the selected file: %1").arg(dbPath), "OK");
		return;
	}

	int fileSize = fi.size();
	QFile file(dbPath);
	if (!file.open(QFile::ReadOnly)) 
	{
		QMessageBox::warning(nullptr, QString("File Copy Failed"), QString("Can't open the selected file: %1").arg(dbPath), "OK");
		return;
	}
	QByteArray fileContent = file.readAll();
	file.close();
	if (fileContent.length() != fileSize)
	{
		QMessageBox::warning(nullptr, QString("File Copy Failed"), QString("Can't read the selected file: %1").arg(dbPath), "OK");
		return;
	}

	/* Part 1: message header, len = 12 */
	COPY_FILE_TO_DEVICE_MSG msg;
	msg.m_magicNumber = MAGIC_NUMBER;
	msg.m_cmdID = CMD_ID_COPY_FILE_TO_DEVICE;

	/* Part 2: file description, len = descriptionSize */
	int descriptionSize;
	QByteArray description;
//	ba.resize(fileSize + 100);
	QBuffer buffer(&description);  
	buffer.open(QIODevice::WriteOnly);  

	// totalSize = 4 Byte(fileSize) + file Name length
	QDataStream out(&buffer);  
	out << fileSize << fi.fileName();
	descriptionSize = buffer.pos();
	buffer.close(); 

	msg.m_size = 12 + descriptionSize + fileSize;
	QByteArray sendBuffer;
	sendBuffer.append((const char *)&msg, 12);
	sendBuffer.append(description, descriptionSize);
	sendBuffer.append(fileContent, fileSize);

	m_pTcpSocket->write(sendBuffer);
}

void dumpSocketInfo(SOCKET_INFO *pInfo)
{
	QHostAddress sendAddress, recvAddress;
	sendAddress.setAddress(pInfo->m_sendAddr);
	recvAddress.setAddress(pInfo->m_recvAddr);
	qDebug() << "Sender " << sendAddress.toString() << ":" <<pInfo->m_sendPort << ", Recver " << recvAddress.toString() <<":"<<pInfo->m_recvPort;
}

U32 STATE(U8 a, U8 b, U8 c, U8 d)
{
	U32 ret = 0;

	ret |= (d << 24);
	ret |= (c << 16);
	ret |= (b << 8);
	ret |= a;

	return (ret);
}

U8 getSub0(U32 state)
{
	return (state & 0xff);
}

U8 getSub1(U32 state)
{
	return ((state >> 8) & 0xff);
}

U8 getSub2(U32 state)
{
	return ((state >> 16) & 0xff);
}

U8 getSub3(U32 state)
{
	return ((state >> 24) & 0xff);
}

