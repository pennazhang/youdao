#include "YoudaoDatabase.h"
#include <QSqlQuery>
#include <QDate>
#include <QDebug>
#include <QtSql/qsql.h>
#include <QSqlError>
#include <QFile>
#include <math.h>


int g_param1 = 20;
double g_param2 = 1.3;
RoteManager g_roteManager;

RoteManager::RoteManager()
{
	m_wordCount = 0;
	m_currentRoteIndex = 0;
}

void RoteManager::clear()
{
	m_wordCount = 0;
	m_currentRoteIndex = 0;
}

void RoteManager::addWord(YoudaoWord *pYoudaoWord)
{
	if (m_wordCount >= TOTAL_ROTE_COUNT)
	{
		return;
	}

	m_roteWord[m_wordCount].m_YoudaoWord = *pYoudaoWord;
	m_roteWord[m_wordCount].m_roteCount = 0;
	m_roteWord[m_wordCount].m_finishFlag = false;

	m_wordCount++;
}

YoudaoWord *RoteManager::getWord(int i)
{
	if ((i >= 0) && (i < m_wordCount))
	{
		return (&m_roteWord[i].m_YoudaoWord);
	}
	else
	{
		return (nullptr);
	}
}


int RoteManager::getLeftWordCount()
{
	int i;
	int left = 0;

	for (i = 0; i < m_wordCount; i++)
	{
		if (m_roteWord[i].m_finishFlag == false)
		{
			left++;
		}
	}
	return (left);
}

void RoteManager::deleteWord(int wordIndex)
{
	if (wordIndex >= m_wordCount)
	{
		return;
	}

	for (int index = wordIndex + 1; index < m_wordCount; index++)
	{
		m_roteWord[index - 1] = m_roteWord[index];
	}
	m_wordCount--;
}

int RoteManager::getTotalWordCount()
{
	return (m_wordCount);
}

int RoteManager::getCurrentRoteWord(YoudaoWord *pYoudaoWord)
{
	int i;

	for (i = m_currentRoteIndex; i < m_wordCount; i++)
	{
		if (m_roteWord[i].m_finishFlag == false)
		{
			*pYoudaoWord = m_roteWord[i].m_YoudaoWord;
			m_currentRoteIndex = i;
			return i;
		}
	}

	for (i = 0; i < m_currentRoteIndex; i++)
	{
		if (m_roteWord[i].m_finishFlag == false)
		{
			*pYoudaoWord = m_roteWord[i].m_YoudaoWord;
			m_currentRoteIndex = i;
			return i;
		}	
	}

	return (-1);
}

/*
	resultFlag:
		TRUE: rote success.
		FALSE: rote failed.
*/
#if 0
void RoteManager::setRoteResult(int index, bool  resultFlag)
{
#if 0
	if ((index >= 0) && (index <= TOTAL_ROTE_COUNT))
	{
		if (m_roteWord[index].m_roteCount == 0)
		{
			int famility = m_roteWord[index].m_YoudaoWord.m_progress;
			if (resultFlag == true)
			{
				famility++;
			}
			else
			{
				famility--;
			}
			if (famility < 1)
			{
				famility = 1;
			}
			g_database.setFamility(m_dbName, m_roteWord[index].m_YoudaoWord.m_word, famility);
		}
		m_roteWord[index].m_roteCount++;
		m_roteWord[index].m_finishFlag = resultFlag;

		if ((m_roteWord[index].m_roteCount == 2) && (resultFlag == false))
		{
			g_database.addBrowseWord(m_dbName, m_roteWord[index].m_YoudaoWord.m_word);
		}
		else if ((m_roteWord[index].m_roteCount <= 2) && (resultFlag == true))
		{
			g_database.deleteBrowseWord(m_dbName, m_roteWord[index].m_YoudaoWord.m_word);					
		}
	}
	m_currentRoteIndex++;
	if (m_currentRoteIndex >= m_wordCount)
	{
		m_currentRoteIndex = 0;
	}
#endif
}
#endif

void RoteManager::setDBName(QString dbName)
{
	m_dbName = dbName;
}

YoudaoDatabase::YoudaoDatabase(void)
{
	m_database = QSqlDatabase::addDatabase("QSQLITE");
}


YoudaoDatabase::~YoudaoDatabase(void)
{
}

int YoudaoDatabase::getBrowseWordCount(QString fileName)
{
	bool ret = false;
	int count = 0;

	m_database.setDatabaseName(fileName);
	if (m_database.open())        
	{
		QSqlQuery query;
		ret = query.exec("select count(*) as count from Focus_table");
		if (ret == true)
		{
			if (query.next())
			{
				count = query.value(0).toInt();
			}
		}
	}

	m_database.close();
	return (count);
}

void YoudaoDatabase::deleteAllBrowseWord(QString fileName)
{
//	bool ret = false;
	QString script;

	m_database.setDatabaseName(fileName);
	if (m_database.open())        
	{
		QSqlQuery query;
		query.exec("delete from Focus_table");
	}
	m_database.close();
}

void YoudaoDatabase::addBrowseWord(QString fileName, QString word)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(fileName);
	if (m_database.open())        
	{
		QSqlQuery query;
		ret = query.exec("select * from Focus_table");
		if (ret == false)
		{
			query.exec("drop table Focus_table");
			qDebug() << "drop table Focus_table";
			script = QString("create table Focus_table(word text primary key)");
			qDebug() << "create table Focus_table(word text primary key)";
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}
		}
			
		script = QString("insert into Focus_table values('%1')").arg(word);
		qDebug() << script;
		query.exec(script);
	}

PROG_END:
	m_database.close();
}

void YoudaoDatabase::deleteBrowseWord(QString fileName, QString strWord)
{
	QString script;

	m_database.setDatabaseName(fileName);
	if (m_database.open())        
	{
		QSqlQuery query;
		QString script = QString("delete from Focus_table where word = '%1'").arg(strWord);
		qDebug() << script;
		query.exec(script);
	}
	m_database.close();
}

int YoudaoDatabase::loadBrowseWord(QString fileName, BrowseData * pBrowserData, int maxWordCount)
{
	bool ret = false;
	int count = 0;

	m_database.setDatabaseName(fileName);
	if (m_database.open())        
	{
		QSqlQuery query;
		QString script = QString("select YoudaoWordBook.* from Focus_table inner join YoudaoWordBook on Focus_table.word=YoudaoWordBook.word");
		ret = query.exec(script);
		if (ret == true)
		{
			//Let's export the words to XML files.
			while (query.next())
			{
				pBrowserData[count].m_strWord = query.value(0).toString();
				pBrowserData[count].m_strExplain = query.value(1).toString();
				count++;
				if (count >= maxWordCount)
				{
					break;
				}
			}
		}
		m_database.close();
	}

	return (count);
}

bool YoudaoDatabase::canRestoreDB(QString fileName)
{
	bool ret = false;

	m_database.setDatabaseName(fileName);
	if (m_database.open())        
	{
		QSqlQuery query;
		ret = query.exec("select * from old_YoudaoWordBook");
		m_database.close();
	}

	return (ret);
}

bool YoudaoDatabase::restoreDB(QString fileName)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(fileName);

	if (m_database.open())        
	{  
		QSqlQuery query;
		ret = query.exec("select * from old_YoudaoWordBook");
		if (ret == true)
		{
			query.exec("drop table YoudaoWordBook");
			ret = query.exec("alter table old_YoudaoWordBook rename to YoudaoWordBook");
		}

		ret = query.exec("select *from Old_Focus_table");
		if (ret == true)
		{
			query.exec("drop table Focus_table");
			ret = query.exec("alter table Old_Focus_table rename to Focus_table");
		}

		m_database.close();
	}

	return (ret);
}


bool YoudaoDatabase::setFamility(QString fileName, QString word, int famility)
{
    m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	QDate today = QDate::currentDate();		
	QString script = QString("update YoudaoWordBook set progress='%1', lastdate='%2-%3-%4' where word='%5'").arg(famility).arg(today.year(), 4, 10, QChar('0')).arg(today.month(), 2, 10, QChar('0')).arg(today.day(), 2, 10, QChar('0')).arg(word);
	qDebug() << script;
	bool ret = query.exec(script);
	if (ret == false)
	{
		return false;
	}

	m_database.close();
	return (true);
}

bool YoudaoDatabase::addtoDB(QString fileName, YoudaoWord *pWord, int wordCount, bool replaceFlag)
{
	int i = 0;
	bool ret = false;
	QString script;

	m_database.setDatabaseName(fileName);
	g_roteManager.clear();

	if (m_database.open())        
	{  

		QSqlQuery query;

		query.exec("drop table table_import_word");
		script = QString("create table table_import_word(word text primary key, trans text, phonetic text, tag text, progress integer, lastdate date)");
		ret = query.exec(script);
		if (ret == false)
		{
			qDebug() << query.lastError().text();
			goto PROG_END;
		}

		for (i = 0; i < wordCount; i++)
		{
			pWord[i].m_word.replace("'", "''");
			pWord[i].m_trans.replace("'", "''");
			pWord[i].m_phonetic.replace("'", "''");
			pWord[i].m_tags.replace("'", "''");
			if (pWord[i].m_progress < 0)
			{
				pWord[i].m_progress = 0;
			}
			if (pWord[i].m_progress > 20)
			{
				pWord[i].m_progress = 20;
			}

			if (pWord[i].m_Date.year() <= 0)
			{
				pWord[i].m_Date = QDate::currentDate();
			}
			script = QString("insert into table_import_word values('%1', '%2', '%3', '%4', %5, '%6-%7-%8')").arg(pWord[i].m_word).arg(pWord[i].m_trans).arg(pWord[i].m_phonetic).arg(pWord[i].m_tags).arg(pWord[i].m_progress).arg(pWord[i].m_Date.year(), 4, 10, QChar('0')).arg(pWord[i].m_Date.month(), 2, 10, QChar('0')).arg(pWord[i].m_Date.day(), 2, 10, QChar('0'));
			qDebug() << "Execute: " << script;
			ret = query.exec(script);
			if (ret == false)
			{
//				break;
			}
		}
#if 0
		if ( i != wordCount)
		{
			goto PROG_END;
		}
#endif
		//�滻����ǰ�ĵ���
		if (replaceFlag == true)
		{
			/* �ҳ����б���ʧ�ܵĵ��� */
			query.exec("drop view view_1");
			script = QString("create view view_1 as select a.* from table_import_word a left outer join YoudaoWordBook b on a.word = b.word where b.word is not null and (a.progress = 1 or a.progress < b.progress)");
			ret = query.exec(script);
			if (ret == true)
			{
				ret = query.exec("select * from view_1");		
				if (ret == true)
				{
					while (query.next())
					{
						YoudaoWord word;
						word.m_word = query.value(0).toString();
						word.m_trans = query.value(1).toString();
						word.m_phonetic = query.value(2).toString();
						word.m_tags = query.value(3).toString();
						word.m_progress = query.value(4).toInt();
						word.m_Date = query.value(5).toDate();

						g_roteManager.addWord(&word);
					}
				}
				
				#if 0
				{
					/* �����б���ʧ�ܵĵ��ʼ��뵽�ص��ע���ݿ��� */
					query.exec("drop table Old_Focus_table");
					query.exec("alter table Focus_table rename to Old_Focus_table");

					script = QString("create table Focus_table(word text primary key)");
					ret = query.exec(script);
					if (ret == false)
					{
						qDebug() << query.lastError().text();
						goto PROG_END;
					}

					script = QString("insert into Focus_table select word from view_1 union select word from Old_Focus_table");
					ret = query.exec(script);
				}
				#else
				{
					// ����滻���µ��ʣ������Ǿ�Ҫ���滻�����ʴӡ��ҵĹ�ע����ɾ��������Ϊ���ҵĹ�ע����Ҫ����Щ������߽����д���ĵ��ʡ���Ȼ�����Ѿ��滻����Щ�����ˣ����ǵ�ȻҪ�����Ǵӡ��ҵĹ�ע����ɾ����
					script = QString("delete from Focus_table where word in (select word from view_1)");
					ret = query.exec(script);
				}
				#endif
			}

			//�ҳ�����YoudaoWordBook���У���table_import_word��û�еĵ���
			query.exec("drop view view_1");
			script = QString("create view view_1 as select a.* from YoudaoWordBook a left outer join table_import_word b on a.word = b.word where b.word is null"); 
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}

			query.exec("drop table table_1");
			script = QString("create table table_1(word text primary key, trans text, phonetic text, tag text, progress integer, lastdate date)");
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}

			script = QString("insert into table_1 select * from table_import_word union all select * from view_1");
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}

		}
		else
		{
			//�ҳ�����table_import_word���У���YoudaoWordBook��û�еĵ���
			query.exec("drop view view_1");
			script = QString("create view view_1 as select a.* from table_import_word a left outer join YoudaoWordBook b on a.word = b.word where b.word is null"); 
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}

			/* ��view_1�е����е��ʷŵ�g_word�У��������Ե�����Щ���� */
			ret = query.exec("select * from view_1");		
			if (ret == true)
			{
				//Let's export the words to XML files.
				while (query.next())
				{
					YoudaoWord word;
					word.m_word = query.value(0).toString();
					word.m_trans = query.value(1).toString();
					word.m_phonetic = query.value(2).toString();
					word.m_tags = query.value(3).toString();
					word.m_progress = query.value(4).toInt();
					word.m_Date = query.value(5).toDate();

					g_roteManager.addWord(&word);
					qDebug() << word.m_word;
				}
			}

			//��table_import_word�еĵ��ʼ��뵽YoudaoWordBook�У�������ظ�����table_import_word�е������ݸ��ǵ�YoudaoWordBook�е�����
			query.exec("drop table table_1");
			script = QString("create table table_1(word text primary key, trans text, phonetic text, tag text, progress integer, lastdate date)");
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}

			script = QString("insert into table_1 select * from YoudaoWordBook union all select * from view_1");
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}
		}

		query.exec("drop table old_YoudaoWordBook");
			
		ret = query.exec("alter table YoudaoWordBook rename to old_YoudaoWordBook");
		if (ret == false)
		{
			qDebug() << query.lastError().text();
			goto PROG_END;
		}

		ret = query.exec("alter table table_1 rename to YoudaoWordBook");
		if (ret == false)
		{
			qDebug() << query.lastError().text();
			goto PROG_END;
		}

		ret = true;
PROG_END:
//		script = QString("drop table table_import_word");
//		query.exec(script);
		m_database.close();
	}

	return (ret);
}

bool YoudaoDatabase::editWord(QString fileName, QString word, QString newWord, QString phoentic, QString newTrans)
{
	bool ret = true;

    m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	newWord.replace("'", "''");
	phoentic.replace("'", "''");
	newTrans.replace("'", "''");
	QSqlQuery query;
//	QString script = QString("delete from YoudaoWordBook where lower(word) = lower('%1')").arg(word);
//	QString script = QString("UPDATE YoudaoWordBook SET trans='%1' WHERE lower(word) = lower('%2')").arg(newTrans).arg(word);
	QString script = QString("UPDATE YoudaoWordBook SET trans='%1', phonetic='%2', word='%3' WHERE lower(word) = lower('%4')").arg(newTrans).arg(phoentic).arg(newWord).arg(word);
	
	if (!query.exec(script))
	{
		qDebug() << "Failed at : " << script;
		ret = false;
	}

	script = QString("UPDATE Focus_table SET word='%1' WHERE lower(word) = lower('%2')").arg(newWord).arg(word);
	if (!query.exec(script))
	{
		// Never Mind, this word is not stored in Focus_table.
		qDebug() << "Failed at : " << script;
	}

	m_database.close();
	return (ret);
}

bool YoudaoDatabase::removeWord(QString fileName, QStringList list)
{
    m_database.setDatabaseName(fileName);
	qDebug() << "DB Name: " << fileName;
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;

	for (int i = 0; i < list.count(); i++)
	{
		QString word = list.at(i);
		word.replace("'", "''");
		if (!word.isEmpty())
		{
			QString script = QString("delete from YoudaoWordBook where lower(word) = lower('%1')").arg(word);
			if (!query.exec(script))
			{
				qDebug() << "Failed at : " << script;
			}
		}
	}

	m_database.close();
	return (true);
}

/*
	desc
		0: Ascendant (default)
		1: descendant
*/
bool YoudaoDatabase::LoadFromDBByPriority(QString fileName, YoudaoWord *pWord, int &wordCount, int maxWordCount, bool desc)
{
	wordCount = 0;

    m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	int	maxRecordNum = maxWordCount, i, priority, f;
	QString script;
	bool ret = false;


	//We need to create logs here.
	QTextStream logStream;
	QFile logFile(fileName + ".txt");
	logFile.open(QIODevice::WriteOnly);
	logStream.setDevice(&logFile);

	if (maxWordCount == 0)
	{
		maxRecordNum = 100000;
		ret = query.exec("select * from YoudaoWordBook order by word");
	}
	else
	{
		query.exec("drop table t_power");
		ret = query.exec("create table t_power(progress integer primary key, result integer)");
		if (ret == false)
		{
			qDebug() << query.lastError().text();
			goto PROG_END;
		}

		for (i = 0; i < 21; i++)
		{
			script = QString("insert into t_power values (%1, %2)").arg(i).arg((int)(g_param1 * pow(g_param2, (i - 1))));
			logStream << QString("progress = %1, Familiarity(progress) = %2").arg(i).arg((int)(g_param1 * pow(g_param2, (i - 1)))) << endl;
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}
		}

		query.exec("drop table t_time");
		ret = query.exec("create table t_time(day integer primary key, result integer)");
		if (ret == false)
		{
			qDebug() << query.lastError().text();
			goto PROG_END;
		}
		for (i = 0; i < 5; i++)
		{
			script = QString("insert into t_time values (%1, %2)").arg(i).arg((int)(g_param1 - g_param1 * pow(g_param2, (4 - i))));
			logStream << QString("day = %1, Familiarity(day) = %2").arg(i).arg((int)(-(g_param1 - g_param1 * pow(g_param2, (4 - i))))) << endl;
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}
		}

		query.exec("drop view v_cal1");

#if 1
		ret = query.exec("create view v_cal1 as select  a.*, ifnull(b.result, 1 + round(julianday('now')) - round(julianday(a.lastdate))) * 1 as result_1 from YoudaoWordBook a left outer join t_time b on round(1 + julianday('now')) - round(julianday(a.lastdate)) = b.day");
#else
		ret = query.exec("create view v_cal1 as select  a.*, ifnull(b.result, round(julianday('now')) - round(julianday(a.lastdate))) * 1 as result_1 from YoudaoWordBook a left outer join t_time b on round(julianday('now')) - round(julianday(a.lastdate)) = b.day");
#endif

		if (ret == false)
		{
			qDebug() << query.lastError().text();
			goto PROG_END;
		} 

		if (desc)
		{
			ret = query.exec("select a.*, ifnull(b.result, 10000) * 1 - a.result_1 as result_2 from v_cal1 a left outer join t_power b on a.progress = b.progress order by result_2 DESC");
		}
		else
		{
			ret = query.exec("select a.*, ifnull(b.result, 10000) * 1 - a.result_1 as result_2 from v_cal1 a left outer join t_power b on a.progress = b.progress order by result_2");		
		}
		if (ret == false)
		{
			qDebug() << query.lastError().text();
			goto PROG_END;
		} 
	}

	//Let's export the words to XML files.
	while (query.next())
	{
		pWord[wordCount].m_word = query.value(0).toString();
		pWord[wordCount].m_trans = query.value(1).toString();
		pWord[wordCount].m_phonetic = query.value(2).toString();
		pWord[wordCount].m_tags = query.value(3).toString();
		pWord[wordCount].m_progress = query.value(4).toInt();
		pWord[wordCount].m_Date = query.value(5).toDate();
		if (pWord[wordCount].m_Date.year() <= 0)
		{
			pWord[wordCount].m_Date = QDate::currentDate();
		}
		f = query.value(6).toInt();
		priority = query.value(7).toInt();

		logStream << QString("%1 word = %2, progress = %3, date = %4, f(date) = %5, Familiarity = %6").arg(wordCount, 4).arg(pWord[wordCount].m_word, 20).arg(pWord[wordCount].m_progress, 1).arg(pWord[wordCount].m_Date.toString()).arg(-f).arg(priority) << endl;

		wordCount++;
#if 0
		if (wordCount >= maxRecordNum)
		{
			break;
		}
#endif
	}

	if (wordCount >= maxRecordNum)
	{
		wordCount = maxRecordNum;
	}

PROG_END:
	m_database.close();
	return (ret);
}

bool YoudaoDatabase::exportWords(QString fileName, QStringList wordList, YoudaoWord *pWord, int &wordCount)
{
	wordCount = 0;
	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;

	for (int i = 0; i < wordList.count(); i++)
	{
		QString word = wordList.at(i);
		word.replace("'", "''");
		if (!word.isEmpty())
		{
			QString script = QString("select * from YoudaoWordBook where lower(word) = lower('%1')").arg(word);
			if (query.exec(script) == true)
			{
				if (query.next())
				{
					pWord[wordCount].m_word = query.value(0).toString();
					pWord[wordCount].m_trans = query.value(1).toString();
					pWord[wordCount].m_phonetic = query.value(2).toString();
					pWord[wordCount].m_tags = query.value(3).toString();
					pWord[wordCount].m_progress = query.value(4).toInt();
/*
					if (pWord[wordCount].m_progress < 1)
					{
						pWord[wordCount].m_progress = 1;
					}
*/
					if (pWord[wordCount].m_progress > 20)
					{
						pWord[wordCount].m_progress = 20;
					}

					pWord[wordCount].m_Date = query.value(5).toDate();
					if (pWord[wordCount].m_Date.year() <= 0)
					{
						pWord[wordCount].m_Date = QDate::currentDate();		
					}

					wordCount++;
				}
			}
		}
	}

	m_database.close();
	return (true);

}


bool YoudaoDatabase::LoadFromDB(QString fileName, YoudaoWord *pWord, int &wordCount)
{
	wordCount = 0;
    m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	int	maxRecordNum = 1000000;

	bool ret = query.exec("select * from YoudaoWordBook");
	if (ret == false)
	{
		m_database.close();
		return (false);
	}

	while (query.next())
	{
		pWord[wordCount].m_word = query.value(0).toString();
		pWord[wordCount].m_trans = query.value(1).toString();
		pWord[wordCount].m_phonetic = query.value(2).toString();
		pWord[wordCount].m_tags = query.value(3).toString();
		pWord[wordCount].m_progress = query.value(4).toInt();
		pWord[wordCount].m_Date = query.value(5).toDate();
		if (pWord[wordCount].m_Date.year() <= 0)
		{
			pWord[wordCount].m_Date = QDate::currentDate();		
		}

		wordCount++;
		if (wordCount >= maxRecordNum)
		{
			break;
		}
	}
	m_database.close();
	return (true);
}

bool YoudaoDatabase::getRoteSetting(QString fileName, int &totalCount, bool &l1Flag, bool &l2Flag, int &l0Count, int &l1Count, int &l2Count, int &highestLevel)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return false;
	}

	QSqlQuery query;
	script = QString("select * from RoteSetting where dbVersion = '%1'").arg(CURRENT_ROTE_SETTING_VERSION);
	ret = query.exec(script);
	if (ret != true)
	{
		goto PROG_END;
	}

	if (query.next())
	{
		totalCount = query.value(1).toInt();
		l1Flag = query.value(2).toInt();
		l2Flag = query.value(3).toInt();
		l0Count = query.value(4).toInt();
		l1Count = query.value(5).toInt();
		l2Count = query.value(6).toInt();
		highestLevel = query.value(7).toInt();
	}
	else
	{
		ret = false;
	}

PROG_END:
	m_database.close();
	return (ret);
}

bool YoudaoDatabase::setRoteSetting(QString fileName, int totalCount, bool l1Flag, bool l2Flag, int l0Count, int l1Count, int l2Count, int highestLevel)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return false;
	}

	QSqlQuery query;
	script = QString("select * from RoteSetting where dbVersion = '%1'").arg(CURRENT_ROTE_SETTING_VERSION);
	ret = query.exec(script);
	if (ret == true)
	{
		if (query.next())
		{
			script = QString("update RoteSetting set totalCount='%1', l1Flag='%2', l2Flag='%3', l0Count='%4', l1Count='%5',l2Count='%6,l2Count='%7',highestLevel='%8' where dbVersion='%9'")
				.arg(totalCount).arg(l1Flag).arg(l2Flag).arg(l0Count).arg(l1Count).arg(l2Count).arg(highestLevel).arg(CURRENT_ROTE_SETTING_VERSION);
			qDebug() << script;
			ret = query.exec(script);
		}
		else
		{
			ret = false;
		}
	}

	/* if we failed to update the table RoteSetting, then we must recreate the table, because the table structure may changed. */
	if (ret != true)
	{
		query.exec("drop table RoteSetting");

		/* The database is not exist, we will create the database first */
		ret = query.exec("create table RoteSetting (dbVersion integer primary key, totalCount integer, l1Flag integer, l2Flag integer, l0Count integer, l1Count integer, l2Count integer, highestLevel integer)");
		if (ret != true)
		{
			goto PROG_END;
		}

		script = QString("insert into RoteSetting values('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8')")
			.arg(CURRENT_ROTE_SETTING_VERSION).arg(totalCount).arg(l1Flag).arg(l2Flag).arg(l0Count).arg(l1Count).arg(l2Count).arg(highestLevel);
		ret = query.exec(script);
		if (ret != true)
		{
			goto PROG_END;
		}
	}

PROG_END:
	m_database.close();
	return (ret);
}

bool YoudaoDatabase::getBrief(QString fileName, QString &brief)
{
	bool ret = false;
	QString script;

	brief = QString("");
	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return false;
	}

	QSqlQuery query;
	script = QString("select * from RoteBeiefSetting");
	ret = query.exec(script);
	if (ret != true)
	{
		goto PROG_END;
	}

	if (query.next())
	{
		brief = query.value(0).toString();
	}

PROG_END:
	m_database.close();
	return (ret);
}

bool YoudaoDatabase::setBrief(QString fileName, QString &brief)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return false;
	}

	QSqlQuery query;
	script = QString("select * from RoteBeiefSetting");
	ret = query.exec(script);
	if (ret == true)
	{
		ret = query.next();
		if (ret)
		{
			script = QString("update RoteBeiefSetting set brief='%1'").arg(brief);
			qDebug() << script;
			ret = query.exec(script);
		}
	}

	/* if we failed to update the table RoteSetting, then we must recreate the table, because the table structure may changed. */
	if (ret != true)
	{
		query.exec("drop table RoteBeiefSetting");

		/* The database is not exist, we will create the database first */
		ret = query.exec("create table RoteBeiefSetting (brief text)");
		if (ret != true)
		{
			goto PROG_END;
		}

		script = QString("insert into RoteBeiefSetting values('%1')").arg(brief);
		ret = query.exec(script);
		if (ret != true)
		{
			goto PROG_END;
		}
	}

PROG_END:
	m_database.close();
	return (ret);
}


bool YoudaoDatabase::newDB(QString fileName)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	ret = query.exec("create table YoudaoWordBook (word text primary key, trans text, phonetic text, tag text, progress integer, lastdate date);");
	if (ret != true)
	{
		goto PROG_END;
	}

	ret = query.exec("create table YoudaoWordPriority (recordindex integer primary key, param1 integer, param2 real, version integer)");
	if (ret != true)
	{
		goto PROG_END;
	}

	script = QString("insert into YoudaoWordPriority values('1', '25', '1.3', '%1')").arg(DB_VERSION);
	ret = query.exec(script);
	if (ret != true)
	{
		goto PROG_END;
	}

PROG_END:
	m_database.close();
	return (ret);
}

bool YoudaoDatabase::getPriority(QString fileName, int &a, qreal &b)
{
	bool ret = false;

	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	ret = query.exec("select * from YoudaoWordPriority where recordindex = '1'");
	if (ret != true)
	{
		goto PROG_END;
	}

	if (query.next())
	{
		a = query.value(1).toInt();
		b = query.value(2).toDouble();
	}

PROG_END:
	m_database.close();
	return (ret);
}

bool YoudaoDatabase::setPriority(QString fileName, int a, qreal b)
{
	bool ret = false;
	QString word, script;
	QDate date;

	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	script = QString("update YoudaoWordPriority set param1=%1, param2='%2' where recordindex='1'").arg(a).arg(b);
	ret = query.exec(script);

	m_database.close();
	return (ret);
}

bool YoudaoDatabase::isValidDB(QString fileName)
{
	bool ret = false;
	int version = 0;

	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	ret = query.exec("select version from YoudaoWordPriority where recordindex = '1'");
	if (ret != true)
	{
		goto PROG_END;
	}

	if (query.next())
	{
		version = query.value(0).toInt();
		if (version == DB_VERSION)
		{
			ret = true;
		}
		else
		{
			ret = false;
		}
	}

PROG_END:
	m_database.close();
	return (ret);
}

bool YoudaoDatabase::getDBSummary(QString fileName, int & nTotalCount, int &level0, int & level1, int & level2, int & level3, int & level4, int & level5, int & level6, int & above7, int &totalScore)
{
	nTotalCount = level0 = level1 = level2 = level3 = level4 = level5 = level6 = above7 = 0;
	int nLevel[100], levelIndex;

	for (levelIndex = 0; levelIndex < 100; levelIndex++)
	{
		nLevel[levelIndex] = 0;
	}

    m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	bool ret;
	ret = query.exec("Select progress, count(progress) as levelcount from YoudaoWordBook group by progress");
	if (ret != true)
	{
		goto PROG_END;
	}

	totalScore = 0;
	for (;;)
	{
		if (query.next())
		{
			levelIndex = query.value(0).toInt();
			if ((levelIndex >= 0) && (levelIndex < 100))
			{
				nLevel[levelIndex] = query.value(1).toInt();
				nTotalCount += nLevel[levelIndex];
				totalScore += levelIndex * nLevel[levelIndex];
			}
		}
		else
		{
			break;
		}
	}

	level0 = nLevel[0];
	level1 = nLevel[1];
	level2 = nLevel[2];
	level3 = nLevel[3];
	level4 = nLevel[4];
	level5 = nLevel[5];
	level6 = nLevel[6];
	for (int i = 7; i < 100; i++)
	{
		above7 += nLevel[i];
	}

PROG_END:
	m_database.close();
	return (ret);
}

/* Copy all the emphasis words to g_roteManager */
bool YoudaoDatabase::getEmphasisRecord(QString fileName)
{
	YoudaoWord word;
	bool ret = false;
	QString script;

	g_roteManager.clear();
	m_database.setDatabaseName(fileName);
    if (!m_database.open())
	{
		return false;
	}

	QSqlQuery query;
	script = QString("select YoudaoWordBook.* from Focus_table inner join YoudaoWordBook on Focus_table.word=YoudaoWordBook.word");
	ret = query.exec(script);
	if (ret == true)
	{
		//Let's export the words to XML files.
		while (query.next())
		{
			word.m_word = query.value(0).toString();
			word.m_trans = query.value(1).toString();
			word.m_phonetic = query.value(2).toString();
			word.m_tags = query.value(3).toString();
			word.m_progress = query.value(4).toInt();
			word.m_Date = query.value(5).toDate();

			g_roteManager.addWord(&word);
		}
	}
	m_database.close();
	return (ret);
}