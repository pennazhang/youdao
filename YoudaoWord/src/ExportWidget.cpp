#include "ExportWidget.h"
#include "YoudaoSetting.h"
#include "widget.h"

ExportWidget::ExportWidget(QWidget *parent) :
    QWidget(parent)
{
	/* Total Count */
	m_pTotalCountLabel = new QLabel(tr("Maximun export word count:"), this);
	m_pWordCount = new QSpinBox(this);
	m_pWordCount->setRange(0, 10000);
	m_pWordCount->setValue(0);
	connect(m_pWordCount, SIGNAL(valueChanged(int)), this, SLOT(onWordCountChanged(int)));

	/* Level 0 */
	m_pLevel0Label = new QLabel(tr("Word Count of Level 0:"), this);
	m_pLevel0Count = new QSpinBox(this);
	m_pLevel0Count->setRange(0, 10000);
	m_pLevel0Count->setValue(0);
	connect(m_pLevel0Count, SIGNAL(valueChanged(int)), this, SLOT(onLevel0CountChanged(int)));

	/* Level 1 */
	m_pLevel1SelectBox = new QCheckBox(tr("Word Count of Level 1:"), this);
	m_pLevel1SelectBox->setChecked(0);
	connect(m_pLevel1SelectBox, SIGNAL(stateChanged(int)), this, SLOT(onLevel1SelectChanged(int)));
	m_pLevel1Count = new QSpinBox(this);
	m_pLevel1Count->setRange(0, 10000);
	m_pLevel1Count->setValue(0);
	connect(m_pLevel1Count, SIGNAL(valueChanged(int)), this, SLOT(onLevel1CountChanged(int)));

	/* Level 2 */
	m_pLevel2SelectBox = new QCheckBox(tr("Word Count of Level 2:"), this);
	m_pLevel2SelectBox->setChecked(0);
	connect(m_pLevel2SelectBox, SIGNAL(stateChanged(int)), this, SLOT(onLevel2SelectChanged(int)));
	m_pLevel2Count = new QSpinBox(this);
	m_pLevel2Count->setRange(0, 10000);
	m_pLevel2Count->setValue(0);
	connect(m_pLevel2Count, SIGNAL(valueChanged(int)), this, SLOT(onLevel2CountChanged(int)));

	/* Highest Level */
	m_pHighestLevelLabel = new QLabel(tr("Highest Level:"), this);
	m_pHighestLevel = new QSpinBox(this);
	m_pHighestLevel->setRange(0, 20);
	m_pHighestLevel->setValue(7);
	connect(m_pHighestLevel, SIGNAL(valueChanged(int)), this, SLOT(onHighestLevelChanged(int)));

	/* Button 1 */
	m_pSettingButton = new QPushButton(tr("Familiarity Setting"), this);
	connect(m_pSettingButton, SIGNAL(clicked()), g_pMainWidget, SLOT(onPrioritySetting()));

	/* Button 2 */
	m_pExportButton = new QPushButton(tr("Export"), this);
	connect(m_pExportButton, SIGNAL(clicked()), g_pMainWidget, SLOT(onExportXML()));

	m_BrowseButton = new QPushButton(tr("Export preview"), this);
	connect(m_BrowseButton, SIGNAL(clicked()), g_pMainWidget, SLOT(onBrowse()));

	m_pCheckButton = new QPushButton(tr("=>"), this);
	connect(m_pCheckButton, SIGNAL(clicked()), g_pMainWidget, SLOT(onCheck()));

	m_pCountLabel = new QLabel(QString(tr("count:")), this);
	m_pCountLabel->setEnabled(false);
	for (int i = 0; i < 9; i++)
	{
		m_pLevelLabel[i] = new QLabel(QString("%1: ").arg(i), this);
		m_pLevelLabel[i]->setEnabled(false);
	}
	m_pLevelLabel[9] = new QLabel(QString("9+: "), this);
	m_pLevelLabel[9]->setEnabled(false);

	m_totalCount = 60;
	m_l0Count = 5;
	m_l1Count = 10;
	m_l2Count = 10;
	m_l1Flag = true;
	m_l2Flag = true;
	m_highestLevel = 7;
}

void ExportWidget::resizeEvent(QResizeEvent *)
{
	m_pTotalCountLabel->setGeometry(10, 20, 161, 16);
	m_pWordCount->setGeometry(115, 20, 61, 18);

//	m_pFamilitySelectBox->setGeometry(20, 70, 161, 16);
	m_pLevel0Label->setGeometry(20, 50, 161, 16);
	m_pLevel0Count->setGeometry(115, 50, 61, 18);

	m_pLevel1SelectBox->setGeometry(20, 75, 161, 16);
	m_pLevel1Count->setGeometry(115, 75, 61, 18);

	m_pLevel2SelectBox->setGeometry(20, 100, 161, 16);
	m_pLevel2Count->setGeometry(115, 100, 61, 18);

	m_pHighestLevelLabel->setGeometry(20, 125, 161, 18);
	m_pHighestLevel->setGeometry(115, 125, 61, 18);

	m_pExportButton->setGeometry(250, 160, 81, 23);
	m_BrowseButton->setGeometry(150, 160, 81, 23);
	m_pSettingButton->setGeometry(30, 160, 101, 23);

	m_pCountLabel->setGeometry(260, 5, 81, 18);
	m_pLevelLabel[0]->setGeometry(240, 30, 81, 18);
	m_pLevelLabel[2]->setGeometry(240, 55, 81, 18);
	m_pLevelLabel[4]->setGeometry(240, 80, 81, 18);
	m_pLevelLabel[6]->setGeometry(240, 105, 81, 18);
	m_pLevelLabel[8]->setGeometry(240, 130, 81, 18);

	m_pLevelLabel[1]->setGeometry(300, 30, 81, 18);
	m_pLevelLabel[3]->setGeometry(300, 55, 81, 18);
	m_pLevelLabel[5]->setGeometry(300, 80, 81, 18);
	m_pLevelLabel[7]->setGeometry(300, 105, 81, 18);
	m_pLevelLabel[9]->setGeometry(300, 130, 81, 18);
	m_pCheckButton->setGeometry(190, 80, 30, 20);
}

void ExportWidget::setLevelCount(int count, int level[10])
{
	m_pCountLabel->setText(QString(tr("count: %1")).arg(count));

	for (int i = 0; i < 9; i++)
	{
		m_pLevelLabel[i]->setText(QString("%1: %2").arg(i).arg(level[i]));
	}
	m_pLevelLabel[9]->setText(QString("9+: %1").arg(level[9]));
}

void ExportWidget::onHighestLevelChanged(int value)
{
	m_highestLevel = value;
	refresh();
}

void ExportWidget::onWordCountChanged(int value)
{
	m_totalCount = value;
	refresh();
}

void ExportWidget::onLevel0CountChanged(int value)
{
	m_l0Count = value;
	refresh();
}

void ExportWidget::onLevel1CountChanged(int value)
{
	m_l1Count = value;
	refresh();
}

void ExportWidget::onLevel2CountChanged(int value)
{
	m_l2Count = value;
	refresh();
}

void ExportWidget::onLevel1SelectChanged(int state)
{
	m_l1Flag = state;
	refresh();
}

void ExportWidget::onLevel2SelectChanged(int state)
{
	m_l2Flag = state;
	refresh();
}

void ExportWidget::refresh()
{
}

void ExportWidget::setButtonEnable(bool flag)
{
	m_pTotalCountLabel->setEnabled(flag);
	m_pHighestLevelLabel->setEnabled(flag);
	m_pLevel0Label->setEnabled(flag);

	m_pWordCount->setEnabled(flag);
	m_pLevel0Count->setEnabled(flag);
	m_pLevel1Count->setEnabled(flag);
	m_pLevel2Count->setEnabled(flag);
	m_pHighestLevel->setEnabled(flag);

	m_pLevel1SelectBox->setEnabled(flag);
	m_pLevel2SelectBox->setEnabled(flag);

	m_pExportButton->setEnabled(flag);
	m_pSettingButton->setEnabled(flag);

	if (flag == true)
	{
		refresh();
	}
}

void ExportWidget::onEnterWidget()
{
	if (!g_pMainWidget->getExportSetting(m_totalCount, m_l1Flag, m_l2Flag, m_l0Count, m_l1Count, m_l2Count, m_highestLevel))
	{
		m_totalCount = 60;
		m_l0Count = 5;
		m_l1Count = 10;
		m_l2Count = 10;
		m_l1Flag = true;
		m_l2Flag = true;
		m_highestLevel = 7;
	}

	m_pWordCount->setValue(m_totalCount);
	m_pLevel0Count->setValue(m_l0Count);
	m_pLevel1Count->setValue(m_l1Count);
	m_pLevel2Count->setValue(m_l2Count);
	m_pHighestLevel->setValue(m_highestLevel);
	m_pLevel1SelectBox->setChecked(m_l1Flag);
	m_pLevel2SelectBox->setChecked(m_l2Flag);
}

void ExportWidget::saveExportSetting()
{
	g_pMainWidget->setExportSetting(m_totalCount, m_l1Flag, m_l2Flag, m_l0Count, m_l1Count, m_l2Count, m_highestLevel);
}