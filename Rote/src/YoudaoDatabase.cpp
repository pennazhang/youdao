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
YoudaoDatabase g_database;
RoteManager g_roteManager;
extern QString g_path;

RoteManager::RoteManager()
{
	m_wordCount = 0;
	m_nextRoteIndex = 0;
}

void RoteManager::clear()
{
	m_wordCount = 0;
	m_nextRoteIndex = 0;
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

int RoteManager::getTotalWordCount()
{
	return (m_wordCount);
}

int RoteManager::getNextRoteWord(YoudaoWord *pYoudaoWord)
{
	int i;

	for (i = m_nextRoteIndex; i < m_wordCount; i++)
	{
		if (m_roteWord[i].m_finishFlag == false)
		{
			*pYoudaoWord = m_roteWord[i].m_YoudaoWord;
			m_nextRoteIndex = i;
			return i;
		}
	}

	for (i = 0; i < m_nextRoteIndex; i++)
	{
		if (m_roteWord[i].m_finishFlag == false)
		{
			*pYoudaoWord = m_roteWord[i].m_YoudaoWord;
			m_nextRoteIndex = i;
			return i;
		}	
	}

	return (-1);
}

void RoteManager::clearUnrotedWord()
{
	int newWord;
	for (newWord = m_nextRoteIndex; newWord < m_wordCount; newWord++)
	{
		if (m_roteWord[newWord].m_roteCount == 0)
		{
			break;
		}
	}

	if (m_wordCount != newWord)
	{
		m_wordCount = newWord;
		m_nextRoteIndex = 0;
	}
}

int RoteManager::getRoteCount(int index)
{
	return (m_roteWord[index].m_roteCount);
}

bool RoteManager::isWordFinished(int index)
{
	return (m_roteWord[index].m_finishFlag);
}

/*
	resultFlag:
		TRUE: rote success.
		FALSE: rote failed.
*/
void RoteManager::setRoteResult(int index, bool resultFlag)
{
	if ((index < 0) || (index >= m_wordCount))
	{
		return;
	}

	if ((m_roteWord[index].m_roteCount == 0) && (resultFlag == true))
	{
		m_roteWord[index].m_roteCount = -1;
	}
	else if (resultFlag != true)
	{
		m_roteWord[index].m_roteCount++;
	}

	m_roteWord[index].m_finishFlag = resultFlag;

	m_nextRoteIndex++;
	if (m_nextRoteIndex >= m_wordCount)
	{
		m_nextRoteIndex = 0;
	}
}

int RoteManager::fillWord(YoudaoWord *pWord, QStringList &addList, QStringList &removeList)
{
	int i = 0;
	for (int index = 0; index < m_wordCount; index++)
	{
		pWord[i] = m_roteWord[index].m_YoudaoWord;
		if (m_roteWord[index].m_roteCount < 0)
		{
			pWord[i].m_progress++;
			removeList.append(pWord[i].m_word);
		}
		else if (m_roteWord[index].m_roteCount > 0)
		{
			addList.append(pWord[i].m_word);
			pWord[i].m_progress--;
		}
		if (pWord[i].m_progress < 1)
		{
			pWord[i].m_progress = 1;
		}

		if (m_roteWord[index].m_roteCount != 0)
		{
			pWord[i].m_Date = QDate::currentDate();
			i++;
		}
	}

	return (i);
}


void RoteManager::saveRoteResult()
{
	/* Add your code here */
	YoudaoWord word[TOTAL_ROTE_COUNT];
	QStringList addList, removeList;
	int wordCount = fillWord(word, addList, removeList);
	g_database.addtoDB(getDBName(), word, wordCount, true);
//	g_database.modifyBrowseWord(getDBName(), addList, removeList);
}

void RoteManager::setDBName(QString dbName)
{
	m_dbName = dbName;
}

YoudaoDatabase::YoudaoDatabase(void)
{
/*
      We can't call init() before the main QApplication class, or it will show the following error:
          QSqlDatabase: QSQLITE driver not loaded
          QSqlDatabase: available drivers:
          QSqlDatabase: an instance of QCoreApplication is required for loading driver plugins

      This error only happened at Android or Linux. For Windows, it work normally.

      Please see the following webpage for more information.
            https://forum.qt.io/topic/38486/solved-qt-sql-database-driver-not-loaded-in-android
*/

    //    init();
}

void YoudaoDatabase::init(void)
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

	m_database.setDatabaseName(g_path + fileName);
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


void YoudaoDatabase::modifyBrowseWord(QString fileName, QStringList &addWordList, QStringList &removeWordList)
{
	bool ret = false;
	QString script, word;
	int index;

	m_database.setDatabaseName(g_path + fileName);
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
		
		for (index = 0; index < addWordList.count(); index++)
		{
			word = addWordList.at(index);
			word.replace("'", "''");
			script = QString("insert into Focus_table values('%1')").arg(word);
			qDebug() << script;
			query.exec(script);
		}

		for (index = 0; index < removeWordList.count(); index++)
		{
			word = removeWordList.at(index);
			word.replace("'", "''");
			script = QString("delete from Focus_table where word = '%1'").arg(word);
			qDebug() << script;
			query.exec(script);
		}
	}

PROG_END:
	m_database.close();
}

void YoudaoDatabase::addBrowseWord(QString fileName, QString word)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(g_path + fileName);
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
			
		word.replace("'", "''");
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

	m_database.setDatabaseName(g_path + fileName);
	if (m_database.open())        
	{
		QSqlQuery query;
		strWord.replace("'", "''");
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

	m_database.setDatabaseName(g_path + fileName);
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
				pBrowserData[count].m_strExplain = query.value(1).toString().trimmed();
				pBrowserData[count].m_phonetic = query.value(2).toString().trimmed();
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

	m_database.setDatabaseName(g_path + fileName);
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

	m_database.setDatabaseName(g_path + fileName);

	if (m_database.open())        
	{  
		QSqlQuery query;
		ret = query.exec("select * from old_YoudaoWordBook");
		if (ret == true)
		{
			query.exec("drop table YoudaoWordBook");
			ret = query.exec("alter table old_YoudaoWordBook rename to YoudaoWordBook");
		}

		m_database.close();
	}

	return (ret);
}


bool YoudaoDatabase::setFamility(QString fileName, QString word, int famility)
{
    m_database.setDatabaseName(g_path + fileName);
    if (!m_database.open())
	{
		return (false);
	}

	QSqlQuery query;
	QDate today = QDate::currentDate();		
	word.replace("'", "''");
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

bool YoudaoDatabase::addtoDB(QString fileName, YoudaoWord *pWord, int &wordCount, bool replaceFlag)
{
	int i = 0;
	bool ret = false;
	QString script;

	if (wordCount == 0)
	{
		return true;
	}

	m_database.setDatabaseName(g_path + fileName);

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

			script = QString("insert into table_import_word values('%1', '%2', '%3', '%4', %5, '%6-%7-%8')").arg(pWord[i].m_word).arg(pWord[i].m_trans).arg(pWord[i].m_phonetic).arg(pWord[i].m_tags).arg(pWord[i].m_progress).arg(pWord[i].m_Date.year(), 4, 10, QChar('0')).arg(pWord[i].m_Date.month(), 2, 10, QChar('0')).arg(pWord[i].m_Date.day(), 2, 10, QChar('0'));
			qDebug() << "Execute: " << script;
			ret = query.exec(script);
			if (ret == false)
			{
				break;
			}
		}
		if ( i != wordCount)
		{
			goto PROG_END;
		}

		//替换掉以前的单词
		if (replaceFlag == true)
		{
			//找出所有YoudaoWordBook中有，但table_import_word中没有的单词
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
			//找出所有table_import_word中有，但YoudaoWordBook中没有的单词
			query.exec("drop view view_1");
			script = QString("create view view_1 as select a.* from table_import_word a left outer join YoudaoWordBook b on a.word = b.word where b.word is null"); 
			ret = query.exec(script);
			if (ret == false)
			{
				qDebug() << query.lastError().text();
				goto PROG_END;
			}

			/* 将view_1中的所有单词放到g_word中，这样可以导出这些单词 */
			ret = query.exec("select * from view_1");		
			if (ret == true)
			{
				wordCount = 0;
				//Let's export the words to XML files.
				while (query.next())
				{
					pWord[wordCount].m_word = query.value(0).toString();
					pWord[wordCount].m_trans = query.value(1).toString();
					pWord[wordCount].m_phonetic = query.value(2).toString();
					pWord[wordCount].m_tags = query.value(3).toString();
					pWord[wordCount].m_progress = query.value(4).toInt();
					pWord[wordCount].m_Date = query.value(5).toDate();
					wordCount++;
				}
			}

			//将table_import_word中的单词加入到YoudaoWordBook中，如果有重复，用table_import_word中的新数据覆盖掉YoudaoWordBook中的数据
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

bool YoudaoDatabase::removeWord(QString fileName, QStringList list)
{
    m_database.setDatabaseName(g_path + fileName);
	qDebug() << "DB Name: " << g_path + fileName;
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

    m_database.setDatabaseName(g_path + fileName);
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
	QFile logFile(g_path + fileName + ".txt");
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
	m_database.setDatabaseName(g_path + fileName);
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
					if (pWord[wordCount].m_progress < 1)
					{
						pWord[wordCount].m_progress = 1;
					}
					if (pWord[wordCount].m_progress > 20)
					{
						pWord[wordCount].m_progress = 20;
					}

					pWord[wordCount].m_Date = query.value(5).toDate();
					if (pWord[wordCount].m_Date.year() < 2014)
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
    m_database.setDatabaseName(g_path + fileName);
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
		if (pWord[wordCount].m_Date.year() < 2014)
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

bool YoudaoDatabase::getRoteSetting(QString fileName, int &totalCount, bool &l1Flag, bool &l2Flag, int &l0Count, int &l1Count, int &l2Count, int &highestLevel, int &methodIndex)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(g_path + fileName);
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
		methodIndex = query.value(8).toInt();
	}
	else
	{
		ret = false;
	}

PROG_END:
	m_database.close();
	return (ret);
}

bool YoudaoDatabase::setRoteSetting(QString fileName, int totalCount, bool l1Flag, bool l2Flag, int l0Count, int l1Count, int l2Count, int highestLevel, int methodIndex)
{
	bool ret = false;
	QString script;

	m_database.setDatabaseName(g_path + fileName);
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
			script = QString("update RoteSetting set totalCount='%1', l1Flag='%2', l2Flag='%3', l0Count='%4', l1Count='%5',l2Count='%6', highestLevel='%7', methodIndex='%8' where dbVersion='%9'")
				.arg(totalCount).arg(l1Flag).arg(l2Flag).arg(l0Count).arg(l1Count).arg(l2Count).arg(highestLevel).arg(methodIndex).arg(CURRENT_ROTE_SETTING_VERSION);
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
		ret = query.exec("create table RoteSetting (dbVersion integer primary key, totalCount integer, l1Flag integer, l2Flag integer, l0Count integer, l1Count integer, l2Count integer, highestLevel integer, methodIndex integer)");
		if (ret != true)
		{
			goto PROG_END;
		}

		script = QString("insert into RoteSetting values('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9')")
			.arg(CURRENT_ROTE_SETTING_VERSION).arg(totalCount).arg(l1Flag).arg(l2Flag).arg(l0Count).arg(l1Count).arg(l2Count).arg(highestLevel).arg(methodIndex);
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
	m_database.setDatabaseName(g_path + fileName);
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

	m_database.setDatabaseName(g_path + fileName);
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

	m_database.setDatabaseName(g_path + fileName);
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

	m_database.setDatabaseName(g_path + fileName);
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

	m_database.setDatabaseName(g_path + fileName);
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

	m_database.setDatabaseName(g_path + fileName);
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


//	ret = query.exec("create table YoudaoWordBook (word text primary key, trans text, phonetic text, tag text, progress integer, lastdate datetime, master integer)");

bool YoudaoDatabase::getDBSummary(QString fileName, int & nTotalCount, int &level0, int & level1, int & level2, int & level3, int & level4, int & level5, int & level6, int & above7, int &totalScore)
{
	nTotalCount = level0 = level1 = level2 = level3 = level4 = level5 = level6 = above7 = 0;
	int nLevel[100], levelIndex;

	for (levelIndex = 0; levelIndex < 100; levelIndex++)
	{
		nLevel[levelIndex] = 0;
	}

    m_database.setDatabaseName(g_path + fileName);
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
				totalScore += nLevel[levelIndex] * levelIndex;
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
