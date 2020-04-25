#pragma once
#include "typedef.h"

typedef struct
{
	QString m_bookFileName;
    int     m_fileSize;
	int		m_wordCount;
	int		m_level0;
	int		m_level1;
	int		m_level2;
	int		m_level3;
	int		m_level4;
	int		m_level5;
	int		m_level6;
	int		m_level7;
	int		m_totalScore;
} BOOK_INFO;

#define MAX_BOOK_COUNT	30
class BookInfo 
{
public:
	BookInfo();
	void readFromReg();
	void writeToReg();
	int getBookCount() { return m_bookCount; }	
	bool addBook(QString strBookFileName);
	void deleteBook(int index);
	QString getBookFileName(int i);
	QDataStream & operator <<(QDataStream &s);
	QDataStream & operator >>(QDataStream &s);

	int writeToBuffer(QByteArray & ba);
	void readFromBuffer(QByteArray & ba);
	BOOK_INFO *getBookInfo(int i);
	void refreshBookInfo();

protected:
	int		m_bookCount;
	BOOK_INFO m_bookInfo[MAX_BOOK_COUNT];
};

extern QDataStream &operator<<(QDataStream &out, BookInfo &per);
extern QDataStream &operator>>(QDataStream &out, BookInfo &per);

extern BookInfo	*g_pBookInfo;
