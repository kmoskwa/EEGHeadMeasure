#ifndef TEST006_H
#define TEST006_H

#include <QtGui/QMainWindow>
#include "ui_EEGHeadMeasure.h"
class EEGScalpMeasurePrivate;

class EEGHeadMeasure : public QMainWindow
{
    Q_OBJECT

public:
    EEGHeadMeasure(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~EEGHeadMeasure();

public slots:
    void resizeEvent(QResizeEvent *);
    void calcElectrodes();
    void readConfig();

private slots:
    void slotValueUpdated(double aValue);
    void slotUpdateValue(void);
    void slotElectrodeStateChanged(int aState);
private:
    Ui::EEGHeadMeasure ui;
    EEGScalpMeasurePrivate *m_Private;
};

#endif // TEST006_H
