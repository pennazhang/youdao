#include "DeviceWidget.h"
#include "widget.h"
#include <QDebug>
#include <QByteArray>
#include <windows.h>
#include <QBuffer>
#include <QDataStream>
#include <QFileInfo>
#include <QMessageBox>
#include "bookInfo.h"
#include <QFileDialog>
#include "BrowseDialog.h"
#include "YoudaoSetting.h"
#include <QInputDialog>

#define LOCAL_PORT	40000
#define ROTE_SERVER_PORT 45453
#define BROAD_CAST_PORT 45454

STATE_DATA s_currentState = {0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0 };
STATE_DATA s_nextState = {0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0 };

DeviceWidget::DeviceWidget(QWidget *parent) :
    QWidget(parent)
{
	m_pSelectLabel = new QLabel(tr("Select a device with same network:"), this);

	m_pCopyToButton = new QPushButton(tr("Copy to device"), this);
	m_pCopyToButton->setObjectName(QStringLiteral("m_pCopyToDevice"));
	connect(m_pCopyToButton, SIGNAL(clicked()), this, SLOT(onCopyToDevice()));

	m_pCopyFromButton = new QPushButton(tr("Copy from device"), this);
	m_pCopyFromButton->setObjectName(QStringLiteral("m_pCopyToComputer"));
	connect(m_pCopyFromButton, SIGNAL(clicked()), this, SLOT(onCopyFromDevice()));

	m_pBackupButton = new QPushButton(tr("Backup device file"), this);
	m_pBackupButton->setObjectName(QStringLiteral("m_pBackupButton"));
	connect(m_pBackupButton, SIGNAL(clicked()), this, SLOT(onBackup()));

	m_pDeleteButton = new QPushButton(tr("Delete device file"), this);
	connect(m_pDeleteButton, SIGNAL(clicked()), this, SLOT(onDeleteFile()));

	m_pRefreshButton = new QPushButton(tr("Refresh"), this);
//	m_pRefreshButton->setObjectName(QStringLiteral("m_pRefreshButton"));
	connect(m_pRefreshButton, SIGNAL(clicked()), this, SLOT(onRefreshDevice()));

	
	m_pConnectButton = new QPushButton("", this);
	m_pConnectButton->setObjectName(QStringLiteral("m_pConnectButton"));
	connect(m_pConnectButton, SIGNAL(clicked()), this, SLOT(connectDevice()));

	
	m_pAddDeviceButton = new QPushButton(tr("Add Device"), this);
//	m_pAddDeviceButton->setObjectName(QStringLiteral("m_pConnectButton"));
	connect(m_pAddDeviceButton, SIGNAL(clicked()), this, SLOT(onAddDevice()));

	m_pDeviceCombo = new QComboBox(this);
	connect(m_pDeviceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onDeviceChanged(int)));

	m_pListWidget = new QListWidget(this);
	connect(m_pListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onDeviceBookSelected(int)));

	m_Communication.createSocket(this, LOCAL_PORT, BROAD_CAST_PORT);

	m_pTotalCount = new QLabel("Total: 2054", this);
	m_pLevel0 = new QLabel("0: 13", this);
	m_pLevel1 = new QLabel("1: 146", this);
	m_pLevel2 = new QLabel("2: 342", this);
	m_pLevel3 = new QLabel("3: 709", this);
	m_pLevel4 = new QLabel("4: 341", this);
	m_pLevel5 = new QLabel("5: 523", this);
	m_pLevel6 = new QLabel("6: 500", this);
	m_pLevel7 = new QLabel("7: 208", this);

	m_currentSelectBook = -1;
	m_bookSelectFlag = true;
	onRefreshDevice();

	m_pWaitDlg = new WaitDialog(g_pMainWidget);
	m_pWaitDlg->close();

	m_deviceState = STATE_IDLE;
	m_pTimer = new QTimer(this);
	m_pTimer->start(1000);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

	m_deviceInfoCount = 0;
	m_currentDeviceIndex = 0;
}

typedef enum
{
	CMD_ID_REFRESH_DEVICE = 0,
};

void DeviceWidget::onDeviceBookSelected(int currentRow)
{
	if (m_bookSelectFlag != true)
	{
		return;
	}
	m_currentSelectBook = currentRow;
	if (currentRow < 0)
	{
		return;
	}

	BOOK_INFO *pBookInfo = g_pDeviceBookInfo->getBookInfo(currentRow);
	m_pTotalCount->setText(QString(tr("Total: %1")).arg(pBookInfo->m_wordCount));
	m_pLevel0->setText(QString(tr("0: %1")).arg(pBookInfo->m_level0));
	m_pLevel1->setText(QString(tr("1: %1")).arg(pBookInfo->m_level1));
	m_pLevel2->setText(QString(tr("2: %1")).arg(pBookInfo->m_level2));
	m_pLevel3->setText(QString(tr("3: %1")).arg(pBookInfo->m_level3));
	m_pLevel4->setText(QString(tr("4: %1")).arg(pBookInfo->m_level4));
	m_pLevel5->setText(QString(tr("5: %1")).arg(pBookInfo->m_level5));
	m_pLevel6->setText(QString(tr("6: %1")).arg(pBookInfo->m_level6));
	m_pLevel7->setText(QString(tr("7+: %1")).arg(pBookInfo->m_level7));
}

void DeviceWidget::onAddDevice(void)
{
	QString defaultText = "192.168.1.100";
	QString text;
	bool isOK; 
	QHostAddress deviceAddress;

	text = QInputDialog::getText(NULL, tr("Add Device"), 
			tr("Please Enter the Device's IP Address:"), 
			QLineEdit::Normal, 
			"192.168.1.100", 
			&isOK); 
	if(isOK) 
	{
		if (deviceAddress.setAddress(text) == false)
		{
//			defaultText = text;
			QMessageBox::warning(nullptr, tr("Invalid IP Address"), tr("What you enter is an invalid IP Address, please check it again!"), tr("OK"));
			return;
		}
		QString strHostName = "Unknown Device";
		addDevice(deviceAddress, ROTE_SERVER_PORT, strHostName);
		m_pDeviceCombo->addItem(strHostName + QString(":") + deviceAddress.toString());
	}
}

void DeviceWidget::onRefreshDevice(void)
{
	static int currentCount = 0;
	int i;

	int count = m_pDeviceCombo->count();
	for (i = 0; i < count; i++)
	{
		m_pDeviceCombo->removeItem(0);	
	}

	count = m_pListWidget->count();
	for (i = 0; i < count; i++)
	{
		QListWidgetItem * pItem = m_pListWidget->takeItem(0);
		m_pListWidget->removeItemWidget(pItem);
	}

	g_pDeviceBookInfo->clear();
	m_deviceState = STATE_IDLE;
#if 0
	for (i = 0; i < 5; i++)
	{
		m_pDeviceCombo->addItem(QString("My Device ") + QString::number(currentCount + i));
	}
	currentCount++;
#endif
	m_Communication.disconnectDevice();
	m_Communication.refreshDevice();
	clearAllDevice();
	refreshButton();
}

void DeviceWidget::clearAllDevice(void)
{
	m_deviceInfoCount = 0;
	m_currentDeviceIndex = 0;
}

DeviceWidget::~DeviceWidget(void)
{
}

void DeviceWidget::resizeEvent(QResizeEvent *)
{
	int left = 15;
	m_pSelectLabel->setGeometry(left, 10, 200, 16);
	m_pRefreshButton->setGeometry(212 - 50, 8, 75, 25);
	m_pAddDeviceButton->setGeometry(250, 8, 75, 25);
	
	m_pDeviceCombo->setGeometry(left, 34, 195, 23);
	m_pConnectButton->setGeometry(212, 32, 25, 25);

//	m_pAddButton->setGeometry(250, 20, 100, 23);
	m_pDeleteButton->setGeometry(250, 40, 100, 23);
	m_pCopyToButton->setGeometry(250, 80, 100, 23);
	m_pBackupButton->setGeometry(250, 120, 100, 23);
	m_pCopyFromButton->setGeometry(250, 160, 100, 23);

	m_pListWidget->setGeometry(left, 60, 220, 75);

	m_pTotalCount->setGeometry(left, 135, 200, 20);
	int width = 60;
//	int intervalX = 65;
	m_pLevel0->setGeometry(left, 155, width, 20);
	m_pLevel1->setGeometry(left + width, 155, width, 20);
	m_pLevel2->setGeometry(left + 2 * width, 155, width, 20);
	m_pLevel3->setGeometry(left + 3 * width, 155, width, 20);
	m_pLevel4->setGeometry(left, 175, width, 20);
	m_pLevel5->setGeometry(left + width, 175, width, 20);
	m_pLevel6->setGeometry(left + 2 * width, 175, width, 20);
	m_pLevel7->setGeometry(left + 3 * width, 175, width, 20);
}

void DeviceWidget::onRecvData_0(void)
{
	onRecvData(0);
}

void DeviceWidget::onRecvData_1(void)
{
	onRecvData(1);
}

void DeviceWidget::onRecvData_2(void)
{
	onRecvData(2);
}

void DeviceWidget::onRecvData_3(void)
{
	onRecvData(3);
}

void DeviceWidget::onRecvData_4(void)
{
	onRecvData(4);
}

void DeviceWidget::onRecvData_5(void)
{
	onRecvData(5);
}

void DeviceWidget::onRecvData_6(void)
{
	onRecvData(6);
}

void DeviceWidget::onRecvData_7(void)
{
	onRecvData(7);
}

void DeviceWidget::onRecvData_8(void)
{
	onRecvData(8);
}

void DeviceWidget::onRecvData_9(void)
{
	onRecvData(9);
}

void DeviceWidget::onRecvTCPData(void)
{
	QByteArray readData = m_Communication.readAll();
	qDebug() << "Read bytes: " << readData.length();
	m_receivedWord += readData;

	dispatchSocketMessage();
}

void DeviceWidget::dispatchSocketMessage(void)
{
	const char *pData = m_receivedWord.data();
	unsigned int *pMagicWord;
	int nLen = m_receivedWord.length();
	int start = 0;
//	int size;
	QByteArray msgAck, tempArray;
//	GET_BOOK_LIST_ACK_MSG bookListAckMsg;
	int i, count;


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
			case CMD_ID_DELETE_FROM_DEVICE_ACK:
				qDebug() << "Received: CMD_ID_COPY_FILE_TO_DEVICE_ACK" << endl;
				start += msgLen;

				if (m_deviceState == STATE_DELETE_FILE)
				{
					// Since we have already copy the file to the device, we should show all the file in the device.
					m_Communication.getBookList();
				}

				break;

			case CMD_ID_COPY_FILE_TO_DEVICE_ACK:
				qDebug() << "Received: CMD_ID_COPY_FILE_TO_DEVICE_ACK" << endl;
				start += msgLen;

				if (m_deviceState == STATE_COPY_TO_DEVICE)
				{
					// Since we have already copy the file to the device, we should show all the file in the device.
					m_Communication.getBookList();
				}

				break;

			case CMD_ID_GET_BOOK_LIST_ACK:
				qDebug() << "Received: CMD_ID_GET_BOOK_LIST_ACK" << endl;
				start += msgLen;

#if 0
				bookListAckMsg.m_magicNumber = MAGIC_NUMBER;
				bookListAckMsg.m_cmdID = CMD_ID_GET_BOOK_LIST_ACK;
				size = g_pBookInfo->writeToBuffer(tempArray);
				bookListAckMsg.m_size = 12 + size;

				msgAck.append((const char *)&bookListAckMsg, 12);
				msgAck.append(tempArray, size);
				m_pTcpConnection->write(msgAck);
#endif
				count = m_pListWidget->count();
				/* First delete all books */
				bool flag;
				flag = m_bookSelectFlag;
				m_bookSelectFlag = false;
				for (i = 0; i < count; i++)
				{
					QListWidgetItem * pItem = m_pListWidget->takeItem(0);
					m_pListWidget->removeItemWidget(pItem);
				}

				{
					QByteArray ba(pData + 12, msgLen - 12);
					QBuffer buf(&ba);
					buf.open(QIODevice::ReadOnly);  
					QDataStream in(&buf);
					in >> *g_pDeviceBookInfo;
					buf.close();
				
					for (i = 0; i < g_pDeviceBookInfo->getBookCount(); i++)
					{
						BOOK_INFO *pBookInfo = g_pDeviceBookInfo->getBookInfo(i);
						m_pListWidget->addItem(pBookInfo->m_bookFileName);
					}

					if (m_currentSelectBook >= count)
					{
						m_currentSelectBook = count - 1;
					}
					if (m_currentSelectBook < 0)
					{
						m_currentSelectBook = 0;
					}
					m_bookSelectFlag = flag;
					m_pListWidget->setCurrentRow(m_currentSelectBook);

					if (m_deviceState == STATE_COPY_TO_DEVICE)
					{
						endOperation();
						QMessageBox::information(this, tr("Operation Success"), tr("Operation Success, File is copied to device!"), QMessageBox::Ok); 
					}
					else if (m_deviceState == STATE_DELETE_FILE)
					{
						endOperation();
						QMessageBox::information(this, tr("Operation Success"), tr("Delete Success, File is deleted from device!"), QMessageBox::Ok); 					
					}
#if 0
					else if (m_deviceState == STATE_ADD_RECORD)
					{
						endOperation();
	//					QMessageBox::information(this, tr("Operation Success"), tr("Operation Success, Record is add to device book!"), QMessageBox::Ok); 
						if (g_roteManager.getTotalWordCount() != 0)
						{
							QString strLog = QString(tr("The records have been added into device book, Press OK to check the new words."));
							QMessageBox::warning(this, tr("Add Record Success"), strLog, "OK");
							BrowseDialog browseDialog;
							browseDialog.exec();
						}
					}
#endif
				}
				refreshButton();
				break;
			case CMD_ID_COPY_FROM_DEVICE_ACK:
			{
				COPY_FILE_TO_DEVICE_MSG_ACK *pMsgAck = (COPY_FILE_TO_DEVICE_MSG_ACK *)pData;
				qDebug() << "Received: CMD_ID_COPY_FROM_DEVICE_ACK " << pMsgAck->m_bookIndex << ", file Size = " << pMsgAck->m_size - 16 << endl;
				start += msgLen;

				if (m_deviceState == STATE_BACKUP_DEVICE)
				{
					QString dbName = m_backupDir + g_pDeviceBookInfo->getBookFileName(pMsgAck->m_bookIndex);
					qDebug() << dbName;
					QFile file(dbName);
					if (file.open(QFile::WriteOnly))
					{
						file.write(pData + 16, pMsgAck->m_size - 16);
						file.close();
					}

					if (pMsgAck->m_bookIndex + 1 != g_pDeviceBookInfo->getBookCount())
					{
						m_Communication.backupFile(pMsgAck->m_bookIndex + 1);
					}
					else
					{
						/* The last book */
						endOperation();
						QMessageBox::information(this, tr("Backup Success"), tr("Backup Success, All books are copied from device!"), QMessageBox::Ok); 
					}
					break;
				}
				else if (m_deviceState == STATE_COPY_FROM_DEVICE)
				{
					endOperation();

					QString dbName = m_pListWidget->currentItem()->text();
					QString strDBName = QFileDialog::getSaveFileName(
							this,
							tr("Save book from device"),
							dbName,
							"Youdao DB(*.db);; All files (*.*)"
						);

					if (strDBName.length() != 0)
					{
						QFile file(strDBName);
						if (file.open(QFile::WriteOnly))
						{
							file.write(pData + 16, pMsgAck->m_size - 16);
							file.close();
						}
						else
						{
							QMessageBox::warning(this, tr("Save file failed"), QString(tr("Failed to copy the book from device to %1")).arg(strDBName), "OK");
						}
					}
					break;
				}

			}
			break;
			
			default:
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

void DeviceWidget::onDeviceConnected(void)
{
	refreshButton();
	for (;;)
	{
		if (m_pListWidget->count() != 0)
		{
			QListWidgetItem * pItem = m_pListWidget->takeItem(0);
			m_pListWidget->removeItemWidget(pItem);
		}
		else
		{
			break;
		}
	}
	g_pDeviceBookInfo->clear();

	m_Communication.getBookList();
	m_receivedWord.clear();
}

void DeviceWidget::connectDevice()
{
	int currentIndex = m_pDeviceCombo->currentIndex();
	if (currentIndex == -1)
	{
		return;
	}
	m_Communication.connectToDevice(m_deviceInfo[currentIndex].m_deviceAddr, m_deviceInfo[currentIndex].m_devicePort);
	refreshButton();
}

void DeviceWidget::onDeviceChanged(int )
{
	m_Communication.disconnectDevice();
	for (;;)
	{
		if (m_pListWidget->count() != 0)
		{
			QListWidgetItem * pItem = m_pListWidget->takeItem(0);
			m_pListWidget->removeItemWidget(pItem);
		}
		else
		{
			break;
		}
	}
	g_pDeviceBookInfo->clear();
	refreshButton();
}

void DeviceWidget::onRecvData(int socketIndex)
{
	UPD_SOCKET *pSocket = m_Communication.getUDPSocket();
	QUdpSocket *pUdpSocket = pSocket[socketIndex].m_pUdpSocket;
	int count, i;
	QString strBookName, strDBName;

	SOCKET_INFO_MSG *pSocketInfoMsg;
//	COPY_TO_DEVICE_ACK_MSG *pCopyToDeviceAckMsg;
//	DELETE_BOOK_FROM_DEVICE_ACK_MSG *pDeleteFromDeviceAckMsg;
	QString strHostName;

    while (pSocket[socketIndex].m_pUdpSocket->hasPendingDatagrams()) 
	{
        char aucData[2000];
		int nRecvLen = pUdpSocket->pendingDatagramSize();
        pUdpSocket->readDatagram(aucData, nRecvLen);

		char log[501];
		FormatString(log, (BYTE *)aucData, nRecvLen);
		qDebug() << "Recv " <<nRecvLen << " : " << log;

		CMD_ID cmdID = *(CMD_ID *)aucData;
		switch (cmdID)
		{
		case CMD_ID_CHECK_SOCKET_INFO_ACK:
			pSocketInfoMsg = (SOCKET_INFO_MSG *)aucData;
			aucData[nRecvLen] = 0;
			strHostName.sprintf("%s", aucData + sizeof(SOCKET_INFO_MSG));
			qDebug() << "Recv from: " << strHostName << ":" << pSocketInfoMsg->m_socketInfo.m_sendPort;
			dumpSocketInfo(&pSocketInfoMsg->m_socketInfo);

			addDevice(QHostAddress(pSocketInfoMsg->m_socketInfo.m_sendAddr), pSocketInfoMsg->m_socketInfo.m_sendPort, strHostName);
			m_pDeviceCombo->addItem(strHostName + QString(":") + QHostAddress(pSocketInfoMsg->m_socketInfo.m_sendAddr).toString());
			break;
		case CMD_ID_GET_BOOK_LIST_ACK:
			count = m_pListWidget->count();
			/* First delete all books */
			bool flag;
			flag = m_bookSelectFlag;
			m_bookSelectFlag = false;
			for (i = 0; i < count; i++)
			{
				QListWidgetItem * pItem = m_pListWidget->takeItem(0);
				m_pListWidget->removeItemWidget(pItem);
			}

			{
				QByteArray ba(aucData + 4, nRecvLen - 4);
				QBuffer buf(&ba);
				buf.open(QIODevice::ReadOnly);  
				QDataStream in(&buf);
				in >> *g_pDeviceBookInfo;
				buf.close();
				
				for (i = 0; i < g_pDeviceBookInfo->getBookCount(); i++)
				{
					BOOK_INFO *pBookInfo = g_pDeviceBookInfo->getBookInfo(i);
					m_pListWidget->addItem(pBookInfo->m_bookFileName);
				}

				if (m_currentSelectBook >= count)
				{
					m_currentSelectBook = count - 1;
				}
				if (m_currentSelectBook < 0)
				{
					m_currentSelectBook = 0;
				}
				m_bookSelectFlag = flag;
				m_pListWidget->setCurrentRow(m_currentSelectBook);

				if (m_deviceState == STATE_COPY_TO_DEVICE)
				{
					endOperation();
					QMessageBox::information(this, tr("Operation Success"), tr("Operation Success, File is copied to device!"), QMessageBox::Ok); 
				}
				else if (m_deviceState == STATE_ADD_RECORD)
				{
					endOperation();
//					QMessageBox::information(this, tr("Operation Success"), tr("Operation Success, Record is add to device book!"), QMessageBox::Ok); 
					if (g_roteManager.getTotalWordCount() != 0)
					{
						QString strLog = QString(tr("The records have been added into device book, Press OK to check the new words."));
						QMessageBox::warning(this, tr("Add Record Success"), strLog, "OK");
						BrowseDialog browseDialog;
						browseDialog.exec();
					}
				}
			}
			refreshButton();
			break;

		case CMD_ID_DELETE_FROM_DEVICE_ACK:
			g_pDeviceBookInfo->clear();
			m_Communication.getBookList();		
			break;

		case CMD_ID_COPY_FROM_DEVICE_ACK:
			{
#if 0
				COPY_BOOK_FROM_DEVICE_MSG_ACK *pAck = (COPY_BOOK_FROM_DEVICE_MSG_ACK *)aucData;
	
				int oldSize, totalSize;
				static QByteArray fileContent;
				if (pAck->m_frameIndex == 0)
				{
					fileContent.resize(0);
				}
				oldSize = fileContent.size();

				if (pAck->m_frameSize != 0)
				{
					totalSize = oldSize + pAck->m_frameSize;
					fileContent.resize(totalSize);
					memcpy(fileContent.data() + oldSize, pAck->m_frameData, pAck->m_frameSize);
					m_Communication.copyBookFromDevice(m_currentSelectBook, pAck->m_frameIndex + 1);

					if (m_deviceState == STATE_COPY_FROM_DEVICE)
					{
						setProgress(totalSize * 100 / g_pDeviceBookInfo->getBookInfo(m_currentSelectBook)->m_fileSize);
					}
					else if (m_deviceState == STATE_ADD_RECORD)
					{
						setProgress(totalSize * 50 / g_pDeviceBookInfo->getBookInfo(m_currentSelectBook)->m_fileSize);
					}
					if (m_deviceState == STATE_BACKUP_DEVICE)
					{
						int totalSize = 0;	/* 所有文件总的长度 */
						int currentSize = 0; /* 已经拷贝的文件的长度 */
						for (int index = 0; index < m_currentSelectBook; index++)
						{
							currentSize += g_pDeviceBookInfo->getBookInfo(index)->m_fileSize;
						}
						totalSize = currentSize;
						currentSize += oldSize;
						for (int index = m_currentSelectBook; index < g_pDeviceBookInfo->getBookCount(); index++)
						{
							totalSize += g_pDeviceBookInfo->getBookInfo(index)->m_fileSize;
						}
						setProgress(currentSize * 100 / totalSize);
					}
				}
				else
				{
					/* 现在已经拷贝到一个文件的最后一帧了 */
					if (m_deviceState == STATE_COPY_FROM_DEVICE)
					{
						endOperation();

						QString dbName = m_pListWidget->currentItem()->text();
						strDBName = QFileDialog::getSaveFileName(
							this,
							tr("Save book from device"),
							dbName,
							"Youdao DB(*.db);; All files (*.*)"
						);

						if (strDBName.length() != 0)
						{
							QFile file(strDBName);
							if (file.open(QFile::WriteOnly))
							{
								file.write(fileContent);
								file.close();
							}
							else
							{
								QMessageBox::warning(this, tr("Save file failed"), QString(tr("Failed to copy the book from device to %1")).arg(strDBName), "OK");
							}
						}
					}
					else if (m_deviceState == STATE_BACKUP_DEVICE)
					{
						QString dbName = m_backupDir + g_pDeviceBookInfo->getBookFileName(m_currentSelectBook);
						qDebug() << dbName;
						QFile file(dbName);
						if (file.open(QFile::WriteOnly))
						{
							file.write(fileContent);
							file.close();
						}
						else
						{
							QMessageBox::warning(this, tr("Save file failed"), QString(tr("Failed to copy the book from device to %1")).arg(strDBName), "OK");
						}

						if (m_currentSelectBook == g_pDeviceBookInfo->getBookCount() - 1)
						{
							m_currentSelectBook = m_pListWidget->currentRow();
							endOperation();
						}
						else
						{
							/* 现在我们开始拷贝下一个文件 */
							m_currentSelectBook++;
							m_Communication.copyBookFromDevice(m_currentSelectBook, 0);
						}
					}
					else if (m_deviceState == STATE_ADD_RECORD)
					{
						strDBName = m_pListWidget->currentItem()->text();
						QFile file(strDBName);
						if (file.open(QFile::WriteOnly))
						{
							file.write(fileContent);
							file.close();

							QFile savedFile = (QString("Saved_") + strDBName);
							if (savedFile.open(QFile::WriteOnly))
							{
								savedFile.write(fileContent);
								savedFile.close();
							}

							if (g_pMainWidget->AddRecordToFile(strDBName) == true)
							{
								m_Communication.copyFileNameToDevice(strDBName);
							}
							else
							{
								endOperation();
								QMessageBox::warning(this, tr("Add Record failed"), QString(tr("Failed to Add record to %1")).arg(strDBName), "OK");
							}
						}
						else
						{
							endOperation();
							QMessageBox::warning(this, tr("Save file failed"), QString(tr("Failed to read the book from device to %1")).arg(strDBName), "OK");
						}
					}
				}
#endif
			}
			break;

		}
    }
}

int DeviceWidget::addDevice(QHostAddress deviceAddr, int devicePort, QString deviceName)
{
	int deviceIndex = m_deviceInfoCount;
	m_deviceInfo[m_deviceInfoCount].m_deviceAddr = deviceAddr;
	m_deviceInfo[m_deviceInfoCount].m_devicePort = devicePort;
	m_deviceInfo[m_deviceInfoCount].m_deviceName = deviceName;
	m_deviceInfoCount++;
	m_currentDeviceIndex = m_deviceInfoCount;

	return (deviceIndex);
}

void DeviceWidget::onCopyFromDevice(void)
{
	if (g_pDeviceBookInfo->getBookCount() == 0)
	{
		return;
	}

	startOperation(STATE_COPY_FROM_DEVICE);
	m_Communication.backupFile(m_pListWidget->currentRow());
}

void DeviceWidget::onCopyToDevice(void)
{
	QString dbPath = g_pMainWidget->getCurrentDBName();
	
	QFileInfo fi(dbPath);
	QString dbName = fi.fileName();
	int i;


	if (m_Communication.isDeviceConnected() == false)
	{
		refreshButton();
		return;
	}

	for (i = 0; i < g_pDeviceBookInfo->getBookCount(); i++)
	{
		BOOK_INFO *pBookInfo = g_pDeviceBookInfo->getBookInfo(i);
		if (dbName == pBookInfo->m_bookFileName)
		{
			break;
		}
	}

	QString msg;
	if (i == g_pDeviceBookInfo->getBookCount())
	{
		msg = QString(tr("Do you want to copy the book: %1 to the device?")).arg(dbName);  
	}
	else
	{
		msg = QString(tr("The book: %1 already exists in the device. Do you want to replace it?")).arg(dbName);
	}

	if (QMessageBox::warning(this, QString(tr("Copy file to device")), msg,	QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		startOperation(STATE_COPY_TO_DEVICE);
		m_Communication.copyFileToDevice(dbPath);
	}
}

void DeviceWidget::onBackup()
{
	if (g_pDeviceBookInfo->getBookCount() == 0)
	{
		return;
	}

	QString dirName = QFileDialog::getExistingDirectory(this, tr("Select directory for backup"),
                                             g_pRegSetting->getBackupDir(),
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);

	if (dirName.isEmpty())
	{
		return;
	}
	g_pRegSetting->setBackupDir(dirName);
	dirName += QDate::currentDate().toString("/yyyy_MM_dd");
	qDebug() << dirName;

	QDir dir(dirName);
	if (dir.exists())
	{
		int ret = QMessageBox::warning(this, tr("Directory Exists!"), tr("The backup directrory:%1 exist! Do you want to continue?").arg(dirName), tr("Yes"), tr("No"));
		if (ret != 0)
		{
			return;
		}
	}
	else
	{
		dir.mkpath(".");
	}
	
	m_backupDir = dirName + "/";
	startOperation(STATE_BACKUP_DEVICE);
	m_currentSelectBook = 0;
	m_Communication.backupFile(m_currentSelectBook);
}

void DeviceWidget::onDeleteFile()
{
	if (g_pDeviceBookInfo->getBookCount() == 0)
	{
		return;
	}

	QString dbName = m_pListWidget->currentItem()->text();
	QString msg = QString(tr("Do you want to delete the file: %1 from the device?")).arg(dbName);

	if (QMessageBox::warning(this, QString(tr("Delete file from device")), msg, tr("Yes"), tr("No")) == 0)
	{
		startOperation(STATE_DELETE_FILE);
		m_Communication.deleteFile(m_pListWidget->currentRow());
	}
}

void DeviceWidget::setProgress(int percent)
{
	if (m_deviceState == STATE_IDLE)
	{
		return;
	}

//	m_pWaitDlg->setValue(percent);
	m_progressTimeOut = 0;
	qDebug() << "m_pWaitDlg setValue " << percent;
}

void DeviceWidget::onTimerOut(void)
{
	m_pTimer->start(1000);
}

void DeviceWidget::endOperation()
{
	m_deviceState = STATE_IDLE;
	m_progressTimeOut = 0;
	m_pWaitDlg->close();
	qDebug() << "m_pWaitDlg hide";

}

void DeviceWidget::startOperation(DEVICE_STATE state)
{
	m_deviceState = state;
	m_pWaitDlg->show();
//	m_pWaitDlg->setValue(0);
	qDebug() << "m_pWaitDlg show";
}

void DeviceWidget::refreshButton()
{
	bool connected = m_Communication.isDeviceConnected();
	int nBookCount = g_pDeviceBookInfo->getBookCount();
	m_pConnectButton->setEnabled(!connected);
	m_pCopyToButton->setEnabled(connected);
	m_pCopyFromButton->setEnabled(connected && nBookCount);
	m_pDeleteButton->setEnabled(connected && nBookCount);
	m_pBackupButton->setEnabled(connected && nBookCount);
	if (m_pDeviceCombo->count() == 0)
	{
		m_pConnectButton->setEnabled(false);	
	}
}
