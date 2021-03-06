#include "BookInfo.h"
#include "youdaosetting.h"
#include <QDir>
#include <QBuffer>
#include <QDebug>

BookInfo *	g_pDeviceBookInfo;


BookInfo::BookInfo() 
{ 
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
		s >> m_bookInfo[i].m_bookFileName >> m_bookInfo[i].m_fileSize >> m_bookInfo[i].m_wordCount >> m_bookInfo[i].m_level0 >> m_bookInfo[i].m_level1 >> m_bookInfo[i].m_level2
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

	//����  
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
