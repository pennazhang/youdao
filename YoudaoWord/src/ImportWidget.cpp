#include "ImportWidget.h"
#include "YoudaoSetting.h"
#include "widget.h"

ImportWidget::ImportWidget(QWidget *parent) :
    QWidget(parent)
{
	m_pSettingLabel = new QLabel(tr("When Import the existing word:"), this);

	m_pRadioButton1 = new QRadioButton(tr("Replace the old existing word (update existing words)"), this);
	m_pRadioButton1->setChecked(true);
	m_pRadioButton2= new QRadioButton(tr("Keep the old existing word (add new words)"), this);

	m_pImportButton = new QPushButton(tr("Import"), this);
	connect(m_pImportButton, SIGNAL(clicked()), g_pMainWidget, SLOT(onImportXML()));

	m_pRestoreButton = new QPushButton(tr("Restore Database"), this);
	connect(m_pRestoreButton, SIGNAL(clicked()), g_pMainWidget, SLOT(onRestoreDB()));
}

void ImportWidget::resizeEvent(QResizeEvent *)
{
	m_pSettingLabel->setGeometry(10, 30, 191, 16);
	m_pRadioButton1->setGeometry(20, 70, 221, 17);
	m_pRadioButton2->setGeometry(20, 100, 221, 17);
	m_pRestoreButton->setGeometry(130, 150, 101, 23);
	m_pImportButton->setGeometry(250, 150, 81, 23);
}

bool ImportWidget::getReplaceFlag()
{
	return (m_pRadioButton1->isChecked());
}

void ImportWidget::setButtonEnable(bool flag)
{
	m_pSettingLabel->setEnabled(flag);
	m_pRadioButton1->setEnabled(flag);
	m_pRadioButton2->setEnabled(flag);
	m_pImportButton->setEnabled(flag);
}

void ImportWidget::setRestoreEnable(bool flag)
{
	m_pRestoreButton->setEnabled(flag);
}