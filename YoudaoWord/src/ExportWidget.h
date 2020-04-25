#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>


class ExportWidget : public QWidget
{
    Q_OBJECT

public:
	explicit ExportWidget(QWidget *parent = 0);
	~ExportWidget() {};
	void setButtonEnable(bool flag);
	void onEnterWidget();
	void saveExportSetting();
	void refresh();
	void setLevelCount(int count, int level[10]);

protected slots:
	void onHighestLevelChanged(int value);
	void onWordCountChanged(int);
	void onLevel0CountChanged(int);
	void onLevel1CountChanged(int);
	void onLevel2CountChanged(int);
	void onLevel1SelectChanged(int);
	void onLevel2SelectChanged(int);

protected:
	void resizeEvent(QResizeEvent *);

protected:
	QLabel *m_pLevelLabel[10], *m_pCountLabel;
	QLabel *m_pTotalCountLabel, *m_pLevel0Label, *m_pHighestLevelLabel;
	QSpinBox *m_pWordCount, *m_pLevel0Count, *m_pLevel1Count, *m_pLevel2Count, *m_pHighestLevel;
	QPushButton *m_pExportButton, *m_pSettingButton, *m_BrowseButton, *m_pCheckButton;
	QCheckBox *m_pLevel1SelectBox, *m_pLevel2SelectBox;
//	bool m_refreshFlag;

	int m_totalCount, m_l0Count, m_l1Count, m_l2Count, m_highestLevel;
	bool m_l1Flag, m_l2Flag;
};
