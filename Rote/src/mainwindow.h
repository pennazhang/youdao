#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "infowidget.h"
#include "RoteWidget.h"
#include <QUdpSocket>
#include "youdaosetting.h"
#include "typedef.h"
#include "SettingWidget.h"
#include "BrowseWidget.h"
#include <QTcpServer>
#include <QTcpSocket>
#include "bookInfo.h"

/* Not used, but we need to mark it */
#define SOFT_VERSION	"2.1.2.0"


#define LOCAL_PORT		45453
#define BROAD_CAST_PORT		45454

#define MAX_FRAME_LEN  1480

extern QString g_path;
void FormatString(char lpszBuffer[501], UINT8 * pucMsg, int len);

#pragma pack(4)
typedef struct
{
	U32 m_state;
	U8 m_param[12];
	U32 m_tickCount;
} STATE_DATA;

typedef struct
{
	QHostAddress	m_deviceAddr;
	int				m_devicePort;
	QString			m_deviceName;
	int				m_socketIndex;
} DEVICE_INFO;


typedef enum
{
	CMD_ID_CHECK_SOCKET_INFO	= 0,		// SOCKET_INFO
	CMD_ID_CHECK_SOCKET_INFO_ACK = 1,	// SOCEKET_INFO
	CMD_ID_GET_BOOK_LIST		= 2,
	CMD_ID_GET_BOOK_LIST_ACK	= 3,
	CMD_ID_GET_BOOK_INFO		= 4,
	CMD_ID_GET_BOOK_INFO_ACK	= 5,
	CMD_ID_COPY_TO_DEVICE		= 6,
	CMD_ID_COPY_TO_DEVICE_ACK	= 7,
	CMD_ID_DELETE_FROM_DEVICE		= 8,
	CMD_ID_DELETE_FROM_DEVICE_ACK	= 9,
	CMD_ID_COPY_FILE_TO_DEVICE		= 10,
	CMD_ID_COPY_FILE_TO_DEVICE_ACK	= 11,
	CMD_ID_COPY_FROM_DEVICE			= 12,
	CMD_ID_COPY_FROM_DEVICE_ACK		= 13,
/*
	CMD_ID_GET_FILE_SIZE		= 2,	// char *FileName
	CMD_ID_GET_FILE_SIZE_ACK	= 3,	// int fileLen
	CMD_ID_GET_FILE_DATA		= 4,	// int startAddr, int len
	CMD_ID_GET_FILE_DATA_ACK	= 5,	// char * fileContent
	CMD_ID_DELETE_FILE			= 6,	// char *FileName
	CMD_ID_DELETE_FILE_ACK		= 7,	// int	0: success, 1: fail
	CMD_ID_ADD_FILE				= 8,	// char * fileName;
	CMD_ID_ADD_FILE_ACK			= 9,	// int  0: success, 1: fail
*/
} CMD_ID;

typedef struct 
{
	 UINT32 m_sendAddr;
	 UINT16 m_sendPort;
	 UINT32 m_recvAddr;
	 UINT32 m_recvPort;
} SOCKET_INFO;

typedef struct 
{
	CMD_ID			m_cmdID;	//	m_cmdID = SOCKET_INFO
	SOCKET_INFO		m_socketInfo;
} SOCKET_INFO_MSG;

typedef struct 
{
	UINT32		m_magicNumber;	// m_magicNumber = 0xac659e04;
	UINT32		m_size;			// size = sizeof(GET_BOOK_LIST_MSG)
	CMD_ID		m_cmdID;	// m_cmdID = CMD_ID_GET_BOOK_LIST
} GET_BOOK_LIST_MSG;

typedef struct
{
	UINT32		m_magicNumber;	// m_magicNumber = 0xac659e04;
	UINT32		m_size;			// size = sizeof(GET_BOOK_LIST_MSG)
	CMD_ID		m_cmdID;	// m_cmdID = CMD_ID_GET_BOOK_INFO_ACK

	/*
	s >> m_bookCount;
	for (int i = 0; i < m_bookCount; i++)
	{
        s >> m_bookInfo[i].m_bookFileName >> m_bookInfo[i].m_fileSize <<m_bookInfo[i].m_wordCount >> m_bookInfo[i].m_level0 >> m_bookInfo[i].m_level1 >> m_bookInfo[i].m_level2
			>> m_bookInfo[i].m_level3 >> m_bookInfo[i].m_level4 >> m_bookInfo[i].m_level5 >> m_bookInfo[i].m_level6 >> m_bookInfo[i].m_level7;
	}
	*/
	UINT8		m_bookData[1];	
} GET_BOOK_LIST_ACK_MSG;

typedef struct
{
	UINT32		m_magicNumber;	// m_magicNumber = 0xac659e04;
	UINT32		m_size;			// size = sizeof(DELETE_BOOK_FROM_DEVICE_MSG)
	CMD_ID		m_cmdID;	//  m_cmdID = CMD_ID_DELETE_FROM_DEVICE
	int			m_bookIndex;	// 0 - (bookCount - 1)
} DELETE_BOOK_FROM_DEVICE_MSG;

typedef struct
{
	UINT32		m_magicNumber;	// m_magicNumber = 0xac659e04;
	UINT32		m_size;			// size = sizeof(DELETE_BOOK_FROM_DEVICE_MSG)
	CMD_ID		m_cmdID;	//  m_cmdID = CMD_ID_DELETE_FROM_DEVICE_ACK
	int			m_result;	// 0: OK, 1: Fail
} DELETE_BOOK_FROM_DEVICE_ACK_MSG;

typedef struct
{
	CMD_ID			m_cmdID;	//  m_cmdID = CMD_ID_COPY_TO_DEVICE_BEGIN

	/*
		int bookSize;
		QString bookName;
	*/
	UINT8		m_bookData[1];	
} COPY_TO_DEVICE_MSG;

typedef struct
{
	CMD_ID			m_cmdID;	//  m_cmdID = CMD_ID_COPY_TO_DEVICE_ACK
	int				m_result;	// 0: OK, 1: Fail
} COPY_TO_DEVICE_ACK_MSG;

typedef struct
{
	UINT32		m_magicNumber;	// m_magicNumber = 0xac659e04;
	UINT32		m_size;			// size = sizeof(GET_BOOK_LIST_MSG)
	CMD_ID		m_cmdID;	//  m_cmdID = CMD_ID_COPY_FILE_TO_DEVICE
} COPY_FILE_TO_DEVICE_MSG;

typedef struct
{
	UINT32		m_magicNumber;	// m_magicNumber = 0xac659e04;
	UINT32		m_size;			// size = sizeof(GET_BOOK_LIST_MSG)
	CMD_ID		m_cmdID;	//  m_cmdID = CMD_ID_COPY_FROM_DEVICE
	UINT32		m_bookIndex;	// 0 - (bookCount - 1)
} COPY_FILE_TO_DEVICE_MSG_ACK;

typedef struct
{
	UINT32		m_magicNumber;	// m_magicNumber = 0xac659e04;
	UINT32		m_size;			// size = sizeof(GET_BOOK_LIST_MSG)
	CMD_ID		m_cmdID;	//  m_cmdID = CMD_ID_COPY_FROM_DEVICE
	UINT32		m_bookIndex;	// 0 - (bookCount - 1)
} COPY_BOOK_FROM_DEVICE_MSG;

typedef struct
{
	UINT32		m_magicNumber;	// m_magicNumber = 0xac659e04;
	UINT32		m_size;			// size = sizeof(GET_BOOK_LIST_MSG)
	CMD_ID		m_cmdID;		//  m_cmdID = CMD_ID_COPY_FROM_DEVICE_ACK
	UINT32		m_bookIndex;	// 0 - (bookCount - 1)
	UINT8		m_fileData[0];	// fileSize = m_size - 16
} COPY_BOOK_FROM_DEVICE_MSG_ACK;

#pragma pack()

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
	QString getSelectedBook() { return m_pInfoWidget->getSelectedBook(); }
	void deleteWord(QString word);
	void setMethod(int methodIndex);
	void getCurrentBookInfo(BOOK_INFO &currentBookInfo);

protected:
	void resizeEvent(QResizeEvent * /* event */);
	void dispatchSocketMessage();

public slots:
	void onClickReview();
	void onClickRote();
	void onClickExitRote();
	void onRecvBroadcast();
	void onRecvUDPData();
	void onSettingCancel();
	void onSettingStart();
	void onBrowseExit();
	void onClickBrowse();
	void acceptConnection();
	void onRecvTCPData();

protected:
	BrowseWidget *m_pBrowseWidget;
	InfoWidget *m_pInfoWidget;
	SettingWidget *m_pSettingWidget;
	RoteWidget *m_pRoteWidget;
	QStackedWidget *m_pStackedWidget;

	QUdpSocket *m_pBroadcastSocket;
	QTcpServer *m_pTcpServer;
	QTcpSocket *m_pTcpConnection;
	QUdpSocket *m_pUdpSocket;
	QHostAddress m_localAddr;

	QByteArray m_copyFileData;
	QString m_currentBookName;
	int		m_browseExit;	// 0: Exit to InfoWidget,  1: Exit to SettingWidget

	QByteArray m_receivedWord;
};

extern SOCKET_INFO g_currentSocketInfo;
extern MainWindow *g_pMainWindow;
void dumpSocketInfo(SOCKET_INFO *pInfo);
bool findLocalAddr(SOCKET_INFO * pInfo, QHostAddress &localAddr);

#endif // MAINWINDOW_H
