#include "mainwindow.h"
#include <QNetworkInterface>
#include <QHostInfo>
#include "YoudaoSetting.h"
#include <QBuffer>
#include "bookInfo.h"
#include <QMessageBox>
#include "YoudaoDatabase.h"

#define MAGIC_NUMBER	0xac659e04

MainWindow *g_pMainWindow;
SOCKET_INFO g_currentSocketInfo;

void FormatString(char lpszBuffer[501], UINT8 * pucMsg, int len)
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	m_pStackedWidget = new QStackedWidget(this);

	m_pInfoWidget = new InfoWidget(this);
	m_pStackedWidget->addWidget(m_pInfoWidget);

	m_pBrowseWidget = new BrowseWidget(this);
	m_pStackedWidget->addWidget(m_pBrowseWidget);

	m_pSettingWidget = new SettingWidget(this);
	m_pStackedWidget->addWidget(m_pSettingWidget);

	m_pRoteWidget = new RoteWidget(this);
	m_pStackedWidget->addWidget(m_pRoteWidget);

	m_pStackedWidget->setCurrentWidget(m_pInfoWidget);
	setMinimumSize(320, 455);

	m_pBroadcastSocket = new QUdpSocket(this);
	if (m_pBroadcastSocket->bind(BROAD_CAST_PORT, QUdpSocket::ShareAddress) == false)
	{
		qDebug() << "Fatal error: bind Broadcast port failed!";
	}
	connect(m_pBroadcastSocket, SIGNAL(readyRead()), this, SLOT(onRecvBroadcast()));

	m_pTcpServer = new QTcpServer(this);
	m_pTcpServer->listen(QHostAddress::Any, LOCAL_PORT);
    connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

	m_pUdpSocket = new QUdpSocket(this);
	connect(m_pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvUDPData()));

	m_pTcpConnection = nullptr;
}

void MainWindow::dispatchSocketMessage()
{
	const char *pData = m_receivedWord.data();
	unsigned int *pMagicWord;
	int nLen = m_receivedWord.length();
	int start = 0;
	int size, fileSize;
	QByteArray msgAck, tempArray;
	GET_BOOK_LIST_ACK_MSG bookListAckMsg;
	COPY_FILE_TO_DEVICE_MSG_ACK copyFileToDeviceMsg;
	DELETE_BOOK_FROM_DEVICE_ACK_MSG deleteFileAckMsg;
	QString fileName;
	int fileIndex;


	/* dispatch word here */
	for (start = 0; start < nLen - 3; start++)
	{
		pMagicWord = (unsigned int *)(pData + start);
		if (*pMagicWord == MAGIC_NUMBER)
		{
			break;
		}
	}

	pData += start;
	nLen -= start;
	unsigned int msgLen = 0;
	unsigned int cmdID;
//	unsigned int tempData;
	if (nLen > 4)
	{
		msgLen = *(unsigned int *)(pData + 4);
		if (msgLen <= (UINT32)nLen)
		{
			cmdID = *(unsigned int *)(pData + 8);
			switch(cmdID)
			{

			case CMD_ID_COPY_FROM_DEVICE:
			{
				qDebug() << "Received: CMD_ID_COPY_FROM_DEVICE" << endl;
				start += msgLen;

				int bookIndex = *(unsigned int *)(pData + 12);
				copyFileToDeviceMsg.m_magicNumber = MAGIC_NUMBER;
				copyFileToDeviceMsg.m_cmdID = CMD_ID_COPY_FROM_DEVICE_ACK;
				copyFileToDeviceMsg.m_bookIndex = bookIndex;
				
				if (bookIndex >= g_pBookInfo->getBookCount())
				{
					QMessageBox::warning(this, "Copy File Failed", QString("Failed to copy file from device, for the invalid bookindex: %1").arg(bookIndex), QMessageBox::Ok);
					break;
				}
				fileName = g_pBookInfo->getBookFileName(bookIndex);
				if (fileName.isEmpty())
				{
					QMessageBox::warning(this, "Copy File Failed", QString("The file name is empty"), QMessageBox::Ok);
					break;
				}

				QFile file(g_path + fileName);
				if (!file.open(QFile::ReadOnly)) 
				{        
					m_copyFileData.resize(0);
					QMessageBox::warning(this, "Copy File Failed", QString("Failed to read the file: %1").arg(fileName), QMessageBox::Ok);
					break;
				}
				fileSize = file.size();
				QByteArray fileData = file.read(fileSize);
				file.close();

				copyFileToDeviceMsg.m_size = 16 + fileSize;
				m_pTcpConnection->write((const char *)&copyFileToDeviceMsg, 16);
				m_pTcpConnection->write(fileData);

				break;
			}

			case CMD_ID_GET_BOOK_LIST:
				qDebug() << "Received: CMD_ID_GET_BOOK_LIST" << endl;
				start += msgLen;

				bookListAckMsg.m_magicNumber = MAGIC_NUMBER;
				bookListAckMsg.m_cmdID = CMD_ID_GET_BOOK_LIST_ACK;
				size = g_pBookInfo->writeToBuffer(tempArray);
				bookListAckMsg.m_size = 12 + size;

				msgAck.append((const char *)&bookListAckMsg, 12);
				msgAck.append(tempArray, size);
				m_pTcpConnection->write(msgAck);
				break;

			case CMD_ID_DELETE_FROM_DEVICE:
				qDebug() << "Received: CMD_ID_DELETE_FROM_DEVICE" << endl;
				start += msgLen;

				deleteFileAckMsg.m_magicNumber = MAGIC_NUMBER;
				deleteFileAckMsg.m_cmdID = CMD_ID_DELETE_FROM_DEVICE_ACK;
				deleteFileAckMsg.m_size = sizeof(deleteFileAckMsg);
				deleteFileAckMsg.m_result = 0;
				m_pTcpConnection->write((const char *)&deleteFileAckMsg, deleteFileAckMsg.m_size);

				fileIndex = *(int *)(pData + 12);
				g_pBookInfo->deleteBook(fileIndex);
				g_pBookInfo->writeToReg();
				break;

			case CMD_ID_COPY_FILE_TO_DEVICE:
				qDebug() << "Received: CMD_ID_COPY_FILE_TO_DEVICE" << endl;
				start += msgLen;

				COPY_FILE_TO_DEVICE_MSG_ACK ack;
				ack.m_magicNumber = MAGIC_NUMBER;
				ack.m_size = 12;
				ack.m_cmdID = CMD_ID_COPY_FILE_TO_DEVICE_ACK;
				m_pTcpConnection->write((const char *)&ack, 12);

				QByteArray read(pData + 12, msgLen - 12);
				QBuffer buf(&read);
				buf.open(QIODevice::ReadOnly);  
				QDataStream in(&buf);
				int bookSize;
				QString bookName;
				in >> bookSize >> bookName;
				buf.close();

				qDebug() << "book size = " << bookSize << ", name = " << bookName;
				QFile::remove(g_path + bookName);
				QFile file(g_path + bookName);
				if (!file.open(QIODevice::WriteOnly))
				{
					QMessageBox::warning(this, "Create File Failed", QString("Failed to create File: %1").arg(m_currentBookName), QMessageBox::Ok);
					break;
				}
				file.write(pData + msgLen - bookSize, bookSize);
				file.close();

				g_pBookInfo->addBook(bookName);
				g_pBookInfo->writeToReg();

#if 0
				UINT16 frameID = pCopyFileData->m_frameIndex;
				UINT16 size = pCopyFileData->m_frameLen;
				memcpy(m_copyFileData.data() + frameID * MAX_FRAME_LEN, pCopyFileData->m_frameData, size);

				ack.m_frameIndex = frameID;
				m_pUdpSocket->writeDatagram((char *)&ack, sizeof(COPY_FILE_TO_DEVICE_MSG_ACK), QHostAddress(g_currentSocketInfo.m_recvAddr), g_currentSocketInfo.m_recvPort);
				if ((frameID + 1) * MAX_FRAME_LEN >= m_copyFileData.size())
				{
					QFile::remove(g_path + m_currentBookName);
					QFile file(g_path + m_currentBookName);
					if (!file.open(QIODevice::WriteOnly))
					{
						QMessageBox::warning(this, "Create File Failed", QString("Failed to create File: %1").arg(m_currentBookName), QMessageBox::Ok);
						break;
					}
					file.write(m_copyFileData);
					file.close();
					qDebug() << "Copy file to " << m_currentBookName;

					g_pBookInfo->addBook(m_currentBookName);
					g_pBookInfo->writeToReg();

				}
#endif
				break;
			}
		}
	}

	if (start != 0)
	{
		m_receivedWord = m_receivedWord.right(m_receivedWord.length() - start);
		dispatchSocketMessage();
	}
}

void MainWindow::onRecvTCPData()
{
//    bytesReceived += (int)tcpServerConnection->bytesAvailable();
    QByteArray readData = m_pTcpConnection->readAll();
	qDebug() << "Read bytes: " << readData.length();
	m_receivedWord += readData;

	dispatchSocketMessage();
}

void MainWindow::acceptConnection()
{
	qDebug() << "accept TCP Connection called here..." << endl;
	if (m_pTcpConnection != nullptr)
	{
		m_pTcpConnection->close();
		delete m_pTcpConnection;
	}
	m_receivedWord.clear();

    m_pTcpConnection = m_pTcpServer->nextPendingConnection();
    connect(m_pTcpConnection, SIGNAL(readyRead()), this, SLOT(onRecvTCPData()));
    connect(m_pTcpConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
}

void MainWindow::onRecvBroadcast()
{
	char aucData[2000];
	int nLen;
	static bool s_connectFlag = false;

    while (m_pBroadcastSocket->hasPendingDatagrams()) 
	{
		QHostAddress localAddr;
        nLen = m_pBroadcastSocket->pendingDatagramSize();
		m_pBroadcastSocket->readDatagram(aucData, nLen);

		/* Valid Broadcast package */
		SOCKET_INFO_MSG *pMsg = (SOCKET_INFO_MSG *)aucData;
		if (pMsg->m_cmdID == CMD_ID_CHECK_SOCKET_INFO)
		{
			dumpSocketInfo(&pMsg->m_socketInfo);
			if (findLocalAddr(&pMsg->m_socketInfo, localAddr) == false)
			{
				qDebug() << "Can't find the local address!";
			}
			else
			{
				qDebug() << "Find the local Addr: " << localAddr.toString();
				if (s_connectFlag == false)
				{
					if (m_pUdpSocket->bind(localAddr, LOCAL_PORT) == false)
					{
						QMessageBox::warning(this, "Bind socket failed", "Failed to bind the UDP Socket", QMessageBox::Ok);
						continue;
					}
					else
					{
						s_connectFlag = true;
						m_localAddr = localAddr;
					}
				}
				
				if (m_localAddr == localAddr)
				{
					g_currentSocketInfo.m_sendAddr = localAddr.toIPv4Address();
					g_currentSocketInfo.m_sendPort = LOCAL_PORT;
					g_currentSocketInfo.m_recvAddr = pMsg->m_socketInfo.m_sendAddr;
					g_currentSocketInfo.m_recvPort = pMsg->m_socketInfo.m_sendPort;

					pMsg->m_cmdID = CMD_ID_CHECK_SOCKET_INFO_ACK;
					memcpy(&pMsg->m_socketInfo, &g_currentSocketInfo, sizeof(SOCKET_INFO));

					QString localHostName = QHostInfo::localHostName();
					int nHostNameLen = localHostName.toLatin1().size();
					qDebug() << "Host: " << localHostName.toLatin1().data();
					memcpy(aucData + sizeof(SOCKET_INFO_MSG), localHostName.toLatin1().data(), nHostNameLen);
					m_pUdpSocket->writeDatagram((char *)pMsg, sizeof(SOCKET_INFO_MSG) + nHostNameLen, QHostAddress(g_currentSocketInfo.m_recvAddr), g_currentSocketInfo.m_recvPort);
					qDebug() << "broadcase Send to: " << QHostAddress(g_currentSocketInfo.m_recvAddr).toString() << ":" << g_currentSocketInfo.m_recvPort << " " << sizeof(SOCKET_INFO_MSG) + nHostNameLen << "Bytes:";
				}
			}
		}
    }
}

void dumpSocketInfo(SOCKET_INFO *pInfo)
{
	QHostAddress sendAddress, recvAddress;
	sendAddress.setAddress(pInfo->m_sendAddr);
	recvAddress.setAddress(pInfo->m_recvAddr);
	qDebug() << "Sender " << sendAddress.toString() << ":" <<pInfo->m_sendPort << ", Recver " << recvAddress.toString() <<":"<<pInfo->m_recvPort;
}

/* 根据远端地址，找出一个与远端地址处于同一网段的本地IP地址 */
bool findLocalAddr(SOCKET_INFO * pInfo, QHostAddress &localAddr)
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
			localAddr = entry.ip();
			if ((localAddr.protocol() ==  QAbstractSocket::IPv4Protocol) && (localAddr.isLoopback() == false))
			{
				if (entry.broadcast().toIPv4Address() == pInfo->m_recvAddr)
				{
					return (true); 
				}
				else
				{
					qDebug() << localAddr.toString() << ": " << entry.broadcast().toString();
				}
			}
		}
	}

	return (false);
}

void MainWindow::onRecvUDPData()
{
	char aucData[2000];
	int nRecvLen;
	QByteArray ba;

	GET_BOOK_LIST_MSG *pBookListMsg = (GET_BOOK_LIST_MSG *)aucData;
	GET_BOOK_LIST_ACK_MSG *pBookListAckMsg = (GET_BOOK_LIST_ACK_MSG *)aucData;
	DELETE_BOOK_FROM_DEVICE_MSG *pDeleteBookMsg = (DELETE_BOOK_FROM_DEVICE_MSG *)aucData;
//	COPY_FILE_TO_DEVICE_MSG *pCopyFileData = (COPY_FILE_TO_DEVICE_MSG *)aucData;
//	COPY_BOOK_FROM_DEVICE_MSG *pCopyFromDevice = (COPY_BOOK_FROM_DEVICE_MSG *)aucData;

	while (m_pUdpSocket->hasPendingDatagrams()) 
	{
        nRecvLen = m_pUdpSocket->pendingDatagramSize();
		m_pUdpSocket->readDatagram(aucData, nRecvLen);
		char buffer[501];
		FormatString(buffer, (BYTE *)aucData, nRecvLen);
		qDebug() << "=======================Start==============================";
		qDebug() << "Recv " << nRecvLen << " Bytes: " << buffer;

		switch (pBookListMsg->m_cmdID)
		{
		case CMD_ID_GET_BOOK_LIST:
			qDebug() << "CMD_ID_GET_BOOK_LIST";
			int size;
			size = g_pBookInfo->writeToBuffer(ba);
			pBookListAckMsg->m_cmdID = CMD_ID_GET_BOOK_LIST_ACK;
			memcpy(pBookListAckMsg->m_bookData,	ba.data(), size);
			m_pUdpSocket->writeDatagram((char *)aucData, 4 + size, QHostAddress(g_currentSocketInfo.m_recvAddr), g_currentSocketInfo.m_recvPort);
#if 0
			{
				char msg[501];
				FormatString(msg, (UINT8 *)aucData, 16);
				qDebug() << "Send to: " << QHostAddress(g_currentSocketInfo.m_recvAddr).toString() << ":" << g_currentSocketInfo.m_recvPort << " " << 4 + size << "Bytes:" << msg;
			}
#endif			
			break;

		case CMD_ID_DELETE_FROM_DEVICE:
			{
				qDebug() << "Delete book " << pDeleteBookMsg->m_bookIndex;
				g_pBookInfo->deleteBook(pDeleteBookMsg->m_bookIndex);
				g_pBookInfo->writeToReg();

				DELETE_BOOK_FROM_DEVICE_ACK_MSG deleteAck;
				deleteAck.m_cmdID = CMD_ID_DELETE_FROM_DEVICE_ACK;
				deleteAck.m_result = 0;
				m_pUdpSocket->writeDatagram((char *)&deleteAck, sizeof(DELETE_BOOK_FROM_DEVICE_ACK_MSG), QHostAddress(g_currentSocketInfo.m_recvAddr), g_currentSocketInfo.m_recvPort);
#if 0
				{
					char msg[501];
					FormatString(msg, (UINT8 *)&deleteAck, sizeof(DELETE_BOOK_FROM_DEVICE_ACK_MSG));
					qDebug() << "Send to: " << QHostAddress(g_currentSocketInfo.m_recvAddr).toString() << ":" << g_currentSocketInfo.m_recvPort << " " << sizeof(DELETE_BOOK_FROM_DEVICE_ACK_MSG) << "Bytes:" << msg;
				}
#endif
			}
			break;

		case CMD_ID_COPY_TO_DEVICE:
			{
				QByteArray read(aucData + 4, nRecvLen - 4);
				QBuffer buf(&read);
				buf.open(QIODevice::ReadOnly);  
				QDataStream in(&buf);
				int bookSize;
				QString bookName;
				in >> bookSize >> bookName;
				buf.close();
				m_currentBookName = bookName;
				qDebug() << "book size = " << bookSize << ", name = " << bookName;

				COPY_TO_DEVICE_ACK_MSG ack;
				ack.m_cmdID = CMD_ID_COPY_TO_DEVICE_ACK;
				ack.m_result = 0;
				m_pUdpSocket->writeDatagram((char *)&ack, sizeof(COPY_TO_DEVICE_ACK_MSG), QHostAddress(g_currentSocketInfo.m_recvAddr), g_currentSocketInfo.m_recvPort);

				m_copyFileData.resize(bookSize);
			}
			break;

		case CMD_ID_COPY_FILE_TO_DEVICE:
			{
#if 0
				UINT16 frameID = pCopyFileData->m_frameIndex;
				UINT16 size = pCopyFileData->m_frameLen;
				memcpy(m_copyFileData.data() + frameID * MAX_FRAME_LEN, pCopyFileData->m_frameData, size);

				COPY_FILE_TO_DEVICE_MSG_ACK ack;
				ack.m_cmdID = CMD_ID_COPY_FILE_TO_DEVICE_ACK;
				ack.m_frameIndex = frameID;
				m_pUdpSocket->writeDatagram((char *)&ack, sizeof(COPY_FILE_TO_DEVICE_MSG_ACK), QHostAddress(g_currentSocketInfo.m_recvAddr), g_currentSocketInfo.m_recvPort);
				if ((frameID + 1) * MAX_FRAME_LEN >= m_copyFileData.size())
				{
					QFile::remove(g_path + m_currentBookName);
					QFile file(g_path + m_currentBookName);
					if (!file.open(QIODevice::WriteOnly))
					{
						QMessageBox::warning(this, "Create File Failed", QString("Failed to create File: %1").arg(m_currentBookName), QMessageBox::Ok);
						break;
					}
					file.write(m_copyFileData);
					file.close();
					qDebug() << "Copy file to " << m_currentBookName;

					g_pBookInfo->addBook(m_currentBookName);
					g_pBookInfo->writeToReg();

				}
#endif
			}
			break;

		case CMD_ID_COPY_FROM_DEVICE:
			{
#if 0
				UINT16 bookIndex = pCopyFromDevice->m_bookIndex;
				UINT16 frameIndex = pCopyFromDevice->m_frameIndex;
				int size;

				if (frameIndex == 0)
				{
					if (bookIndex >= g_pBookInfo->getBookCount())
					{
						QMessageBox::warning(this, "Copy File Failed", QString("Failed to copy file from device, for the invalid bookindex: %1").arg(bookIndex), QMessageBox::Ok);
						break;
					}
					QString fileName = g_pBookInfo->getBookFileName(bookIndex);
					if (fileName.isEmpty())
					{
						QMessageBox::warning(this, "Copy File Failed", QString("The file name is empty"), QMessageBox::Ok);
						break;
					}

					QFile file(g_path + fileName);
					if (!file.open(QFile::ReadOnly)) 
					{        
						m_copyFileData.resize(0);
						QMessageBox::warning(this, "Copy File Failed", QString("Failed to read the file: %1").arg(fileName), QMessageBox::Ok);
						break;
					}
					qint64 fileSize = file.size();
					m_copyFileData = file.read(fileSize);
					file.close();
				}

				if ((frameIndex * MAX_FRAME_LEN) >= m_copyFileData.size())
				{
					size = 0;
				}
				else
				{
					size = m_copyFileData.size() - frameIndex * MAX_FRAME_LEN;
					if (size > MAX_FRAME_LEN)
					{
						size = MAX_FRAME_LEN;
					}
				}

				char aucData[2000];
				COPY_BOOK_FROM_DEVICE_MSG_ACK *pMsg = (COPY_BOOK_FROM_DEVICE_MSG_ACK *)aucData;
				pMsg->m_cmdID = CMD_ID_COPY_FROM_DEVICE_ACK;
				pMsg->m_frameIndex = frameIndex;
				pMsg->m_frameSize = size;
				if (size != 0)
				{
					memcpy(pMsg->m_frameData, m_copyFileData.data() + frameIndex * MAX_FRAME_LEN, size);
				}
				m_pUdpSocket->writeDatagram((char *)pMsg, sizeof(COPY_BOOK_FROM_DEVICE_MSG_ACK) + size, QHostAddress(g_currentSocketInfo.m_recvAddr), g_currentSocketInfo.m_recvPort);
#if 0
				{
					char msg[501];
					FormatString(msg, (UINT8 *)pMsg, 16);
					qDebug() << "Send to: " << QHostAddress(g_currentSocketInfo.m_recvAddr).toString() << ":" << g_currentSocketInfo.m_recvPort << " " << sizeof(COPY_BOOK_FROM_DEVICE_MSG_ACK) + size << "Bytes:" << msg;
				}
#endif
#endif
			}
			break;

		}
		qDebug() << "=======================End==============================";

	}
}


MainWindow::~MainWindow()
{

}

void MainWindow::resizeEvent(QResizeEvent * )
{
	m_pStackedWidget->setGeometry(0, 0, width(), height());
}

void MainWindow::onClickRote()
{
	QString bookName = getSelectedBook();
	if (bookName.isEmpty())
	{
		return;
	}

	if (m_pSettingWidget->loadDatabase(bookName) == false)
	{
		return;
	}

	QDate lastDate = g_pRegSetting->getLastDate();
	int dayDiff = lastDate.daysTo(QDate::currentDate());
	if ((dayDiff > 365) || (dayDiff < 0))
	{
		QMessageBox::warning(nullptr, tr("Please check date"), tr("Current date is: %1, and last date is: %2. Please check the current date!").arg(QDate::currentDate().toString()).arg(lastDate.toString()), QMessageBox::Ok);
	}
	g_pRegSetting->setLastDate(QDate::currentDate());

	g_roteManager.setDBName(bookName);
	m_pStackedWidget->setCurrentWidget(m_pSettingWidget);
}

void MainWindow::onClickReview()
{
	QString bookName = getSelectedBook();
	if (bookName.isEmpty())
	{
		return;
	}

	m_browseExit = 0;
	m_pStackedWidget->setCurrentWidget(m_pBrowseWidget);
	m_pBrowseWidget->loadDatabase(bookName);
}

void MainWindow::onClickBrowse()
{
	m_browseExit = 1;
	m_pStackedWidget->setCurrentWidget(m_pBrowseWidget);
	m_pBrowseWidget->loadRoteManager(&g_roteManager);
}

void MainWindow::onClickExitRote()
{
//	m_pStackedWidget->setCurrentWidget(m_pInfoWidget);
	m_pInfoWidget->refreshBook();

	if (m_pRoteWidget->getForgottonWordCount() != 0)
	{
		m_browseExit = 0;
		m_pStackedWidget->setCurrentWidget(m_pBrowseWidget);
		m_pBrowseWidget->loadForgottenWord();
	}
	else
	{
		m_pStackedWidget->setCurrentWidget(m_pInfoWidget);
	}
}

void MainWindow::onSettingCancel()
{
	m_pStackedWidget->setCurrentWidget(m_pInfoWidget);
}

void MainWindow::onSettingStart()
{
	m_pStackedWidget->setCurrentWidget(m_pRoteWidget);
	m_pRoteWidget->startRote();
}

void MainWindow::onBrowseExit()
{
	QString bookName = getSelectedBook();
	if (m_browseExit == 0)
	{
		if (m_pBrowseWidget->getDeleteFlag() == true)
		{
			m_pInfoWidget->refreshBook();
		}
		m_pStackedWidget->setCurrentWidget(m_pInfoWidget);
	}
	else if (m_browseExit == 1)
	{
		if (m_pBrowseWidget->getDeleteFlag() == true)
		{
			m_pInfoWidget->refreshBook();
//			m_pSettingWidget->saveRoteSetting();
			m_pSettingWidget->loadDatabase(bookName);
			g_roteManager.setDBName(bookName);
		}
		m_pStackedWidget->setCurrentWidget(m_pSettingWidget);
	}
}

void MainWindow::deleteWord(QString word)
{
	/*  */
	QStringList stringList;
	stringList.append(word);
	g_database.removeWord(getSelectedBook(), stringList);
	g_database.deleteBrowseWord(getSelectedBook(), word);
}

void MainWindow::setMethod(int methodIndex)
{
	m_pRoteWidget->setMethod(methodIndex);
}

void MainWindow::getCurrentBookInfo(BOOK_INFO &currentBookInfo)
{
	int currentBookIndex = m_pInfoWidget->getCurrentBookIndex();
	BOOK_INFO *pBookInfo = g_pBookInfo->getBookInfo(currentBookIndex);
	currentBookInfo = *pBookInfo;
}