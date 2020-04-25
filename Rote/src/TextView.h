#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QTextEdit>
#include <QLabel>
#include <QTextBrowser>
#include <QEvent>

class TextView : public QTextEdit
{
public:
    TextView(QWidget *parent);
    bool event(QEvent *event);
};

#endif // ROTEDLG_H
