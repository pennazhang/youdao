#include "BookInfo.h"
#include "youdaosetting.h"
#include <QDir>
#include <QBuffer>
#include <QDebug>
#include "YoudaoDatabase.h"
#include "mainwindow.h"

BookInfo *	g_pBookInfo;


BookInfo::BookInfo() 
{ 
	readFromReg(); 
}

void BookInfo::readFromReg()
{
	int count = g_pRegSetting->getBookCount();
	QString fileName;
	m_bookCount = 0;

	for (int i = 0; i < count; i++)
	{
		fileName = g_pRegSetting->getBookFileName(i);
		addBook(fileName);
	}
}

void BookInfo::writeToReg()
{
	g_pRegSetting->setBookCount(m_bookCount);
	for (int i = 0; i < m_bookCount; i++)
	{
		g_pRegSetting->setBookFileName(i, m_bookInfo[i].m_bookFileName);
	}
}

void BookInfo::refreshBookInfo()
{
	int i;
	for (i = m_bookCount - 1; i >= 0; i--)
	{
		QString strBookName = m_bookInfo[i].m_bookFileName;
		int nTotalCount, level0, level1, level2, level3, level4, level5, level6, level7, totalScore;

        QFile file(g_path + strBookName);
        if (!file.open(QIODevice::ReadOnly))
        {
            deleteBook(i);
            continue;
        }
        int fileSize = (int)file.size();
        file.close();

		if (g_database.getDBSummary(strBookName, nTotalCount, level0, level1, level2, level3, level4, level5, level6, level7, totalScore) != true)
		{
			deleteBook(i);
		}
		else
		{
			m_bookInfo[i].m_wordCount = nTotalCount;
            m_bookInfo[i].m_fileSize = fileSize;
			m_bookInfo[i].m_level0 = level0;
			m_bookInfo[i].m_level1 = level1;
			m_bookInfo[i].m_level2 = level2;
			m_bookInfo[i].m_level3 = level3;
			m_bookInfo[i].m_level4 = level4;
			m_bookInfo[i].m_level5 = level5;
			m_bookInfo[i].m_level6 = level6;
			m_bookInfo[i].m_level7 = level7;
			m_bookInfo[i].m_totalScore = totalScore;
		}
	}
}

bool BookInfo::addBook(QString strBookFileName)
{
	int i;

	if (m_bookCount >= MAX_BOOK_COUNT)
	{
		return (false);
	}
	for (i = 0; i < m_bookCount; i++)
	{
		if (m_bookInfo[i].m_bookFileName == strBookFileName)
		{
			break;
		}
	}

    QFile file(g_path + strBookFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return (false);
    }
    int fileSize = (int)file.size();
    file.close();


	/* Let's check the file info */
	int nTotalCount, level0, level1, level2, level3, level4, level5, level6, level7, totalScore;
    if (g_database.getDBSummary(strBookFileName, nTotalCount, level0, level1, level2, level3, level4, level5, level6, level7, totalScore) != true)
    {
        return (false);
    }
	m_bookInfo[i].m_bookFileName = strBookFileName;
    m_bookInfo[i].m_fileSize = fileSize;
	m_bookInfo[i].m_wordCount = nTotalCount;
	m_bookInfo[i].m_level0 = level0;
	m_bookInfo[i].m_level1 = level1;
	m_bookInfo[i].m_level2 = level2;
	m_bookInfo[i].m_level3 = level3;
	m_bookInfo[i].m_level4 = level4;
	m_bookInfo[i].m_level5 = level5;
	m_bookInfo[i].m_level6 = level6;
	m_bookInfo[i].m_level7 = level7;
	m_bookInfo[i].m_totalScore = totalScore;

	if (i >= m_bookCount)
	{
		m_bookCount++;
	}
	return (true);
}

void BookInfo::deleteBook(int index)
{
	if (m_bookCount == 0)
	{
		return;
	}

	for (int i = index + 1; i < m_bookCount; i++)
	{
		m_bookInfo[i - 1] = m_bookInfo[i];
	}
	m_bookCount--;
}

QString BookInfo::getBookFileName(int i)
{
	if (i >= m_bookCount)
	{
		return (QString(""));
	}
	return (m_bookInfo[i].m_bookFileName);
}

BOOK_INFO * BookInfo::getBookInfo(int i)
{
	if (i >= m_bookCount)
	{
		return (NULL);
	}
	else
	{
		return (m_bookInfo + i);
	}
}


QDataStream & BookInfo::operator <<(QDataStream &s) 
{ 
	s << m_bookCount;
	for (int i = 0; i < m_bookCount; i++)
	{
        s << m_bookInfo[i].m_bookFileName << m_bookInfo[i].m_fileSize << m_bookInfo[i].m_wordCount << m_bookInfo[i].m_level0 << m_bookInfo[i].m_level1 << m_bookInfo[i].m_level2
			<< m_bookInfo[i].m_level3 << m_bookInfo[i].m_level4 << m_bookInfo[i].m_level5 << m_bookInfo[i].m_level6 << m_bookInfo[i].m_level7;
	}

	return (s);
}

QDataStream & BookInfo::operator >>(QDataStream &s) 
{ 
	s >> m_bookCount;
	for (int i = 0; i < m_bookCount; i++)
	{
        s >> m_bookInfo[i].m_bookFileName >> m_bookInfo[i].m_fileSize <<m_bookInfo[i].m_wordCount >> m_bookInfo[i].m_level0 >> m_bookInfo[i].m_level1 >> m_bookInfo[i].m_level2
			>> m_bookInfo[i].m_level3 >> m_bookInfo[i].m_level4 >> m_bookInfo[i].m_level5 >> m_bookInfo[i].m_level6 >> m_bookInfo[i].m_level7;
	}

	return (s);
}

QDataStream &operator<<(QDataStream &s, BookInfo& data) { return (data.operator<<(s)); }
QDataStream &operator>>(QDataStream& s, BookInfo& data) { return (data.operator>>(s)); }

int BookInfo::writeToBuffer(QByteArray & ba)
{
	int ret;
	ba.resize(1000);  

	QBuffer buffer(&ba);  
	buffer.open(QIODevice::WriteOnly);  

	//ÊäÈë  
	QDataStream out(&buffer);  
	this->operator<<(out);
	ret = buffer.pos();
	buffer.close(); 

	return (ret);
}

void BookInfo::readFromBuffer(QByteArray & ba)
{
	QBuffer buf(&ba);
	buf.open(QIODevice::ReadOnly);  
	
	QDataStream in(&buf);
	this->operator>>(in);
	buf.close();  
}
