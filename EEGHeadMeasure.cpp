#include <QPainter>
#include <QPointer>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QTimer>

#include "EEGHeadMeasure.h"

class Electrode
  {
  public:
  Electrode() : horizontal(false), point(false), enabled(false) {}
  ~Electrode()  {}

  QString name;
  QVariant posXRel;
  QVariant posYRel;
  QVariant percent;
  bool horizontal;
  QVariant measure;
  QString measureStr;
  QColor color;
  bool point;
  bool enabled;
  };

class EEGScalpMeasurePrivate
  {
  public:
  QPointer<QTimer> m_Timer;
  QList<Electrode> m_Electrodes;
  };



EEGHeadMeasure::EEGHeadMeasure(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
  m_Private = new EEGScalpMeasurePrivate;
  ui.setupUi(this);
  //QString styleSheet = QString("{text-color: rgba(100, 0, 0, 0);}");
  //ui.m_Longitude->setStyleSheet(styleSheet);
  ui.gElectrodes->hide();

  bool ret = false;
  ret = connect(ui.m_Longitude, SIGNAL(valueChanged(double)), this, SLOT(slotValueUpdated(double)));
  ret = connect(ui.m_Perimeter, SIGNAL(valueChanged(double)), this, SLOT(slotValueUpdated(double)));
  ret = connect(ui.FP1,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.FP2,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.Fz,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.F7,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.F3,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.F4,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.F8,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.T3,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.C3,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.C4,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.T4,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.T5,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.P3,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.P4,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.T8,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.O1,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.O2,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.Cz,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.Pz,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));
  ret = connect(ui.Fz,          SIGNAL(stateChanged(int)),    this, SLOT(slotElectrodeStateChanged(int)));

  /*
  QPalette palette;
  palette.setColor(QPalette::Text,Qt::red);
  ui.m_Longitude->setPalette(palette);
  palette.setColor(QPalette::Text, Qt::blue);
  ui.m_Perimeter->setPalette(palette);
  */

  ui.m_Longitude->setValue(38.0);
  ui.m_Perimeter->setValue(60.5);
  QIcon windowIcon(QPixmap(":/EEGHeadMeasure/ZUTIcon.png"));
  qApp->setWindowIcon(windowIcon);

  readConfig();
  slotUpdateValue();
}

EEGHeadMeasure::~EEGHeadMeasure()
{
  delete m_Private;
  m_Private = NULL;
}

void EEGHeadMeasure::calcElectrodes()
  {
  double horizontal = ui.m_Perimeter->value();
  double vertical   = ui.m_Longitude->value();
  for (QList<Electrode>::iterator it = m_Private->m_Electrodes.begin(); it < m_Private->m_Electrodes.end(); it++)
    {
    Electrode &electrode = *it;
    double val = 0.0f;
    if (true == electrode.horizontal)
      {
      val = horizontal * electrode.percent.toDouble() / 100.0f;
      }
    else
      {
      val = vertical * electrode.percent.toDouble() / 100.0f;
      }
    electrode.measureStr = QString::number(val, 'f', 1);
    electrode.measure = val;

    }
  }

void EEGHeadMeasure::readConfig()
  {
  QString fileName = "EEGHeadMeasure.xml";
  QDomDocument doc("EEGHeadMeasure");
  QFileInfo fileInfo(fileName);
  if (false == fileInfo.exists())
    {
    fileName = ":/EEGHeadMeasure/EEGHeadMeasure.xml";
    }
  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly))
    {
    return;
    }
  if (!doc.setContent(&file))
    {
    file.close();
    return;
    }
  file.close();

  QDomElement docElem = doc.documentElement();
  QString rootTag = docElem.tagName();
  if (rootTag != "EEGHeadMeasure")
    {
    return;
    }

  QDomNodeList electrodesElements = docElem.elementsByTagName("Electrodes");
  if (true == electrodesElements.isEmpty())
    {
    return;
    }
  QDomNode electrodeElement = electrodesElements.item(0);

  QDomNode n = electrodeElement.firstChild();
  while(false == n.isNull())
    {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(false == e.isNull())
      {
      Electrode electrode;
      electrode.name = e.attribute("Name");
      electrode.posXRel = e.attribute("X");
      electrode.posYRel = e.attribute("Y");
      electrode.percent = e.attribute("Percent");
      electrode.horizontal = QVariant(e.attribute("Horizontal")).toBool();
      electrode.color = QColor(e.attribute("Color"));
      QString type = e.attribute("Type");
      if ("Point" == type)
        {
        electrode.point = true;
        }
      m_Private->m_Electrodes.append(electrode);
      }
      n = n.nextSibling();
    }
  }

void EEGHeadMeasure::resizeEvent(QResizeEvent *)
{
  return;
  QPixmap pixmap(":/EEGHeadMeasure/EEGImage.png");
  pixmap = pixmap.scaled(ui.m_imgHead->size());
  ui.m_imgHead->setPixmap(pixmap);
}


void EEGHeadMeasure::slotValueUpdated(double aValue)
  {
  if (false == m_Private->m_Timer.isNull())
    {
    delete m_Private->m_Timer;
    }
  m_Private->m_Timer = new QTimer();
  bool ret = false;
  ret = connect(m_Private->m_Timer, SIGNAL(timeout()), this, SLOT(slotUpdateValue()));

  m_Private->m_Timer->setSingleShot(true);
  m_Private->m_Timer->start(500);
  }

void EEGHeadMeasure::slotUpdateValue()
  {
  calcElectrodes();

  QPixmap pixmap(":/EEGHeadMeasure/EEGImage.png");
  int width = pixmap.width();
  int height = pixmap.height();
  //pixmap = pixmap.scaled(width, width);
  QPainter painter(&pixmap);
  painter.setPen(Qt::red);

  int sizeX = width;
  int sizeY = height;
  QFont font = painter.font();
  font.setPointSize(12);
  font.setBold(true);
  painter.setFont(font);
  for (QList<Electrode>::iterator it = m_Private->m_Electrodes.begin(); it < m_Private->m_Electrodes.end(); it++)
    {
    Electrode &electrode = *it;
    int posX = sizeX * electrode.posXRel.toDouble();
    int posY = sizeY * electrode.posYRel.toDouble();
    QString measureStr = electrode.measureStr + " cm";
    painter.setPen(electrode.color);
    if (false == electrode.point)
      {
      painter.drawText( QRectF(QPointF(posX, posY), QPointF(posX + 70, posY + 20)), Qt::AlignLeft | Qt::AlignTop , measureStr);
      }
    else
      {
      if (true == electrode.enabled)
        {
        painter.setBrush(QBrush(electrode.color));
        painter.drawEllipse(QPoint(posX, posY), 4, 4);
        }
      }
    }
  ui.m_imgHead->setPixmap(pixmap);
  }

void EEGHeadMeasure::slotElectrodeStateChanged(int aState)
  {
  QPointer<QObject> sender = QObject::sender();
  if (false == sender.isNull())
    {
    QString objectName = sender->objectName();
    for (QList<Electrode>::iterator it = m_Private->m_Electrodes.begin(); it < m_Private->m_Electrodes.end(); it++)
      {
      Electrode &electrode = *it;
      if (electrode.name != objectName || false == electrode.point )
        {
        continue;
        }
      
      QColor color("yellow");
      bool enabled = true;
      if (0 == aState)
        {
        enabled = false;
        }
      if (2 == aState)
        {
        color = QColor("green");
        }
      electrode.color = color;
      electrode.enabled = enabled;
      }
    }
  slotUpdateValue();
  }
