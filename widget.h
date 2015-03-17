#ifndef WIDGET_H
#define WIDGET_H
#include <qmath.h>
#include <QWidget>
#include <QStringList>
#include <QtWidgets>
#include <QDebug>
class QPainter;
class QRect;

class AbstractWheelWidget: public QWidget
{
 Q_OBJECT

public:
    AbstractWheelWidget(bool touch,QWidget *parent=0);
    virtual ~AbstractWheelWidget();

    int currentIndex()const;
    void setCurrentIndex(int index);

    bool event(QEvent *e);
    void paintEvent(QPaintEvent *e);
    virtual void paintItem(QPainter *painter,int index,const QRect &rect)=0;

    virtual int itemHeight() const=0;
    virtual int itemCount() const=0;

public slots:
    void scrollTo(int index);
signals:
    void stopped(int index);
protected:
    int m_currentItem;
    int m_itemOffset;
    qreal m_lastY;
};
class StringWheelWidget:public AbstractWheelWidget
{
   Q_OBJECT
public:
    StringWheelWidget(bool touch);

    QStringList items()const;
    void setItems(const QStringList &items);

    QSize sizeHint() const;
    QSize minimumSizeHint()const;

    void paintItem(QPainter *painter,int index,const QRect &rect);

    int itemHeight()const;
    int itemCount()const;
private:
    QStringList m_items;
};
class Mylistview :public QWidget
{
    Q_OBJECT
public:
    Mylistview(QStringList data);
private:
    QStringListModel *model;
    QListView *listview;
private slots:
    void insertData();
    void deleteData();
    void doneData();

};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(bool touch)
        : QMainWindow()
    {
        makeSlotMachine(touch);
        setCentralWidget(m_slotMachine);
    }
    QString readmenu()
    {
        QFile file("./mainmenu.txt");
        qDebug()<<"3";
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QMessageBox message(QMessageBox::NoIcon, "错误", "无法读取信息", QMessageBox::Ok, NULL);
            message.exec();
         }
        QTextStream in(&file);
        qDebug()<<"4";
        QString str(in.readAll());
        return str;
    }

    void makeSlotMachine(bool touch)
    {
        if (QApplication::desktop()->width() > 1000) {
            QFont f = font();
            f.setPointSize(f.pointSize() * 2);
            setFont(f);
        }

        m_slotMachine = new QWidget(this);
        QVBoxLayout *grid = new QVBoxLayout(m_slotMachine);
        grid->setSpacing(20);


        QString str=readmenu();
        colors=str.split("#");
        m_wheel = new StringWheelWidget(touch);
        m_wheel->setItems( colors );
        grid->addWidget(m_wheel);

        QHBoxLayout *btlayout=new QHBoxLayout;
        qDebug()<<str.size();
        shakeButton = new QPushButton(tr("摇一摇"));if(!str.size())shakeButton->setEnabled(false);
        connect(shakeButton, SIGNAL(clicked()), this, SLOT(rotateRandom()));
        addbt=new QPushButton(tr("添加"));
        connect(addbt,SIGNAL(clicked()),this,SLOT(addmenu()));
        mainbt=new QPushButton(tr("退出"));
        connect(mainbt,SIGNAL(clicked()),qApp,SLOT(quit()));
        btlayout->addWidget(addbt);
        btlayout->addWidget(shakeButton);
        btlayout->addWidget(mainbt);
        grid->addLayout(btlayout);
        srand(QTime::currentTime().msec());
    }

private slots:
    void rotateRandom()
    {
        qDebug()<<"123";
        shakeButton->setEnabled(false);
        m_wheel->scrollTo(m_wheel->currentIndex() + (qrand() % 200));
        qDebug()<<"321";
    }
    void addmenu()
    {
        qDebug()<<"yes";
        shakeButton->setEnabled(false);
        Mylistview *newlist=new Mylistview(colors);
        newlist->show();
        if(hasFocus())
            update();
    }

private:
    QWidget *m_slotMachine;
    QStringList colors;
    StringWheelWidget *m_wheel;
    QPushButton *shakeButton;
    QPushButton *addbt;
    QPushButton *mainbt;
};





#endif // WIDGET_H
