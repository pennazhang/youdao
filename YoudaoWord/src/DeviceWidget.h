#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QUdpSocket>
#include <QHostAddress>
#include <QListWidget>
#include "Communication.h"
#include <QTimer>
#include "waitDialog.h"

typedef unsigned char UINT8;
typedef struct
{
	UINT8 cmdID;
} UPD_MESSAGE;

typedef enum
{
	STATE_IDLE = 0,
	STATE_COPY_TO_DEVICE = 1,
	STATE_COPY_FROM_DEVICE = 2,
	STATE_ADD_RECORD = 3,
	STATE_BACKUP_DEVICE = 4,
	STATE_DELETE_FILE = 5,
} DEVICE_STATE;

class DeviceWidget : public QWidget
{
    Q_OBJECT

public:
	explicit DeviceWidget(QWidget *parent);
	~DeviceWidget(void);
	QStringList getWordList();

protected:
	void resizeEvent(QResizeEvent *);
	void onRecvData(int socketIndex);
	void setProgress(int percent);
	void endOperation();
	void startOperation(DEVICE_STATE state);
	void refreshButton();
	void clearAllDevice(void);
	void dispatchSocketMessage(void);

	/*
	Return value:
			-1		: if add device failed ( device already exist. )
			0 - 19	: if add device success.
	*/
	int addDevice(QHostAddress deviceAddr, int devicePort, QString deviceName);

protected slots:
	void onRefreshDevice(void);
	void onAddDevice();
	void onRecvData_0(void);
	void onRecvData_1(void);
	void onRecvData_2(void);
	void onRecvData_3(void);
	void onRecvData_4(void);
	void onRecvData_5(void);
	void onRecvData_6(void);
	void onRecvData_7(void);
	void onRecvData_8(void);
	void onRecvData_9(void);
	void onDeviceConnected(void);
	void onRecvTCPData(void);
	void onDeviceChanged(int index);
	void connectDevice(void);
	void onCopyToDevice(void);
	void onCopyFromDevice(void);
	void onDeleteFile(void);
	void onDeviceBookSelected(int currentRow);
	void onTimerOut(void);
	void onBackup();

protected:
	QLabel *m_pSelectLabel, *m_pTotalCount, *m_pLevel0, *m_pLevel1, *m_pLevel2, *m_pLevel3, *m_pLevel4, *m_pLevel5, *m_pLevel6, *m_pLevel7;
	QPushButton *m_pCopyToButton, *m_pCopyFromButton, *m_pDeleteButton, *m_pRefreshButton, *m_pBackupButton;
	QPushButton *m_pConnectButton, *m_pAddDeviceButton;
	QComboBox *m_pDeviceCombo;
	QListWidget *m_pListWidget;
	Communication m_Communication;
	int m_currentSelectBook;
	bool m_bookSelectFlag;	// true: book select function is enable; false: book select function is false;
	QTimer *m_pTimer;
	WaitDialog *m_pWaitDlg;
	int	m_progressTimeOut;		//work only when m_deviceState != STATE_IDLE, it is used to check that operation is timeout or not.
	DEVICE_STATE m_deviceState;
	QString m_backupDir;
	DEVICE_INFO m_deviceInfo[20];	/* At most 20 devices for one single scan */
	int m_deviceInfoCount;
	int m_currentDeviceIndex;
	QByteArray m_receivedWord;
};

