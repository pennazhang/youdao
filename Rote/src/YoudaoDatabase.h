#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QDate>
#include "typedef.h"

#define DB_VERSION 0x101
#define CURRENT_ROTE_SETTING_VERSION	3


extern int calcMaster(int progress, QDate studyDay);

typedef struct 
{
	QString m_strWord;
	QString m_strExplain;
	QString m_phonetic;
} BrowseData;

typedef struct 
{
	QString m_word, m_trans, m_phonetic, m_tags;
	int m_progress;
	QDate m_Date;
} YoudaoWord;

typedef struct
{
	YoudaoWord m_YoudaoWord;
/* 
	0: The word has not been remembered yet.
	-1: The word has been remembered at the first time; 
	>= 1 The word has not been remembered at the first time
*/
	int	m_roteCount;	
	bool m_finishFlag; /* true: the word passed test successfully; false: the word failed to be roted. */
} RoteWord;

#define TOTAL_ROTE_COUNT	1000
class RoteManager
{
public:
	RoteManager();
	void clear();
	void addWord(YoudaoWord *pYoudaoWord);
	void clearUnrotedWord(); /* Delete all the unroted word ( RoteWord.m_roteCount = 0 ) */
	int getLeftWordCount();
	int getTotalWordCount();
	YoudaoWord *getWord(int i);

	/*
		return value:
			>= 0: index of the word in m_roteWord.
			-1: All words has been roted.
	*/
	int getNextRoteWord(YoudaoWord *pYoudaoWord);

	/*
		resultFlag:
			TRUE: rote success.
			FALSE: rote failed.
	*/
	void setRoteResult(int index, bool resultFlag);
	void saveRoteResult();
	void setDBName(QString dbName);
	QString getDBName() { return m_dbName; }
	int getRoteCount(int index);
	bool isWordFinished(int index);
	int fillWord(YoudaoWord *pWord, QStringList &addList, QStringList &removeList);
protected:
	RoteWord m_roteWord[TOTAL_ROTE_COUNT];
	int m_wordCount;
	int m_nextRoteIndex;
	QString m_dbName;
} ;

extern RoteManager g_roteManager;

class YoudaoDatabase
{
public:
	YoudaoDatabase(void);
    void init(void);
	~YoudaoDatabase(void);


	/*  
	replaceFlag:
			true: when import existing word, the old existing word will be replaced.
			false: when import existing word, the old existing word will be kept unchanged. 
	*/
	bool canRestoreDB(QString fileName);
	bool restoreDB(QString fileName);
	bool addtoDB(QString fileName, YoudaoWord *pWord, int &wordCount, bool replaceFlag);
	bool exportWords(QString fileName, QStringList wordList, YoudaoWord *pWord, int &wordCount);
	bool LoadFromDB(QString fileName, YoudaoWord *pWord, int &wordCount);
	bool LoadFromDBByPriority(QString fileName, YoudaoWord *pWord, int &wordCount, int maxWordCount, bool desc = false);
	bool setRoteSetting(QString fileName, int totalCount, bool l1Flag, bool l2Flag, int l0Count, int l1Count, int l2Count, int highestLevel, int methodIndex);
	bool getRoteSetting(QString fileName, int &totalCount, bool &l1Flag, bool &l2Flag, int &l0Count, int &l1Count, int &l2Count, int &highestLevel, int &methodIndex);
	bool newDB(QString fileName);
	bool getPriority(QString fileName, int &a, qreal &b);
	bool setPriority(QString fileName, int a, qreal b);
	bool isValidDB(QString fileName);
	bool getDBSummary(QString fileName, int & nTotalCount, int &level0, int & level1, int & level2, int & level3, int & level4, int & level5, int &level6, int &above7, int &totalScore);
	bool removeWord(QString fileName, QStringList list);
	bool setFamility(QString fileName, QString word, int famility);
	int getBrowseWordCount(QString fileName);
	int loadBrowseWord(QString fileName, BrowseData * pBrowserData, int maxWordCount);

	void modifyBrowseWord(QString fileName, QStringList &addWordList, QStringList &removeWordList);
	void addBrowseWord(QString fileName, QString word);
	void deleteBrowseWord(QString fileName, QString strWord);

	bool setBrief(QString fileName, QString &brief);
	bool getBrief(QString fileName, QString &brief);

protected:	
	QSqlDatabase m_database;
};

extern int g_param1;
extern double g_param2;

extern YoudaoDatabase g_database;

