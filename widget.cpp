#include "widget.h"
#include <QtWidgets>
#include <QStringListModel>
#define WHEEL_SCROLL_OFFSET 50000.0

AbstractWheelWidget::AbstractWheelWidget(bool touch, QWidget *parent)
    :QWidget(parent)
    ,m_currentItem(0)
    ,m_itemOffset(0)
{
    touch=false;
}
AbstractWheelWidget::~AbstractWheelWidget()
{

}
int AbstractWheelWidget::currentIndex()const
{
    return m_currentItem;
}
void AbstractWheelWidget::setCurrentIndex(int index)
{
    if(index>=0&&index<itemCount())
    {
        m_currentItem=index;
        m_itemOffset=0;
        update();
    }
}
bool AbstractWheelWidget::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::ScrollPrepare:
    {
        QScroller *scroller=QScroller::scroller(this);
        scroller->setSnapPositionsY(WHEEL_SCROLL_OFFSET,itemHeight());

        QScrollPrepareEvent *se=static_cast<QScrollPrepareEvent *>(e);
        se->setViewportSize(QSizeF(size()));
        se->setContentPosRange(QRect(0.0,0.0,0.0,WHEEL_SCROLL_OFFSET*2));
        se->setContentPos(QPointF(0.0,WHEEL_SCROLL_OFFSET+m_currentItem*itemHeight()+m_itemOffset));
        se->accept();
        return true;
    }
    case QEvent::Scroll:
    {
        QScrollEvent *se=static_cast<QScrollEvent *>(e);

        qreal y=se->contentPos().y();
        int iy=y-WHEEL_SCROLL_OFFSET;
        int ih=itemHeight();

        int ic=itemCount();
        if(ic>0)
        {
            m_currentItem=iy/ih%ic;
            m_itemOffset=iy%ih;
            if(m_currentItem<0)
            {
                m_itemOffset+=ih;
                m_currentItem--;
            }
            if(m_currentItem<0)
                m_currentItem+=ic;
        }
        update();
        se->accept();
        return true;
    }
    default:
        return QWidget::event(e);
    }
    return true;
}
void AbstractWheelWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    int w=width();
    int h=height();

    QPainter painter(this);
    QPalette palette=QApplication::palette();
    QPalette::ColorGroup colorGroup=isEnabled()?QPalette::Active:QPalette::Disabled;

    QLinearGradient grad(0.5,0,0.5,1.0);
    grad.setColorAt(0,palette.color(colorGroup,QPalette::ButtonText));
    grad.setColorAt(0.2,palette.color(colorGroup,QPalette::Button));
    grad.setColorAt(0.8,palette.color(colorGroup,QPalette::Button));
    grad.setCoordinateMode(QGradient::ObjectBoundingMode);
    QBrush gBrush(grad);

    painter.setPen(palette.color(colorGroup,QPalette::ButtonText));
    painter.setBrush(gBrush);
    painter.drawRect(0,0,w-1,h-1);

    painter.setPen(palette.color(colorGroup,QPalette::Button));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(1,1,w-3,h-3);

    painter.setClipRect(QRect(3,3,w-6,h-6));
    painter.setPen(palette.color(colorGroup,QPalette::ButtonText));

    int iH=itemHeight();
    int iC=itemCount();
    if(iC>0)
    {
        m_itemOffset=m_itemOffset%iH;

            for(int i=-h/2/iH;i<=h/2/iH+1;i++)
            {
                int itemNum=m_currentItem+i;
                while(itemNum<0)
                    itemNum+=iC;
                while(itemNum>=iC)
                    itemNum-=iC;
                paintItem(&painter,itemNum,QRect(6,h/2+i*iH-m_itemOffset-iH/2,w-6,iH));
            }
    }

    QColor highlight=palette.color(colorGroup,QPalette::Highlight);
    highlight.setAlpha(150);

    QLinearGradient grad2(0.5,0,0.5,1.0);
    grad2.setColorAt(0,highlight);
    grad2.setColorAt(1.0,highlight.lighter());
    grad2.setCoordinateMode(QGradient::ObjectBoundingMode);
    QBrush gBrush2(grad2);

    QLinearGradient grad3(0.5,0,0.5,1.0);
    grad3.setColorAt(0,highlight);
    grad3.setColorAt(1.0,highlight.darker());
    grad3.setCoordinateMode(QGradient::ObjectBoundingMode);
    QBrush gBrush3(grad3);

    painter.fillRect(QRect(0,h/2-iH/2,w,iH/2),gBrush2);
    painter.fillRect(QRect(0,h/2,w,iH/2),gBrush3);
}
void AbstractWheelWidget::scrollTo(int index)
{
    QScroller *scroller=QScroller::scroller(this);
    scroller->scrollTo(QPointF(0,WHEEL_SCROLL_OFFSET+index*itemHeight()),5000);
    qDebug()<<"456";
}
StringWheelWidget::StringWheelWidget(bool touch)
    :AbstractWheelWidget(touch)
{

}
QStringList StringWheelWidget::items() const
{
    return m_items;
}
void StringWheelWidget::setItems(const QStringList &items)
{
    m_items=items;
    if(m_currentItem>=items.count())
        m_currentItem=items.count()-1;
    update();
}
QSize StringWheelWidget::sizeHint() const
{
    QFontMetrics fm(font());

    return QSize(fm.width("m")*10+6,fm.height()*7+6);

}
QSize StringWheelWidget::minimumSizeHint()const
{
    QFontMetrics fm(font());

    return QSize(fm.width("m")*5+6,fm.height()*3+6);
}
void StringWheelWidget::paintItem(QPainter *painter, int index, const QRect &rect)
{
    painter->drawText(rect,Qt::AlignCenter,m_items.at(index));
}
int StringWheelWidget::itemHeight()const
{
    QFontMetrics fm(font());
    return fm.height();
}
int StringWheelWidget::itemCount()const
{
    return m_items.count();
}

Mylistview::Mylistview(QStringList data)
{
    model=new QStringListModel(this);
    model->setStringList(data);
    listview=new QListView(this);
    listview->setModel(model);

    QHBoxLayout *btnlayout=new QHBoxLayout;
    QPushButton *insertbtn=new QPushButton(tr("添加"),this);
    QPushButton *delbtn=new QPushButton(tr("删除"),this);
    QPushButton *donebt=new QPushButton(tr("完成"));
    btnlayout->addWidget(insertbtn);
    btnlayout->addWidget(delbtn);
    btnlayout->addWidget(donebt);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(listview);
    mainLayout->addLayout(btnlayout);
    this->setLayout(mainLayout);

    connect(insertbtn,SIGNAL(clicked()),this,SLOT(insertData()));
    connect(delbtn,SIGNAL(clicked()),this,SLOT(deleteData()));
    connect(donebt,SIGNAL(clicked()),this,SLOT(doneData()));
}
void Mylistview::insertData()
{
        bool isOK;
        QString text = QInputDialog::getText(NULL, "添加", "难以选择的是:",
                                                                                 QLineEdit::Normal, "这真的难以选择", &isOK);
        if(isOK) {
                int row = listview->currentIndex().row();
                model->insertRows(row, 1);
                QModelIndex index = model->index(row);
                model->setData(index, text);
                listview->setCurrentIndex(index);
                listview->edit(index);
        }
}

void Mylistview::deleteData()
{
        if(model->rowCount() > 1) {
                model->removeRows(listview->currentIndex().row(), 1);
        }
}
void Mylistview::doneData()
{
    qDebug()<<"done";
    qDebug()<<model->stringList();
    QFile file("./mainmenu.txt");
    qDebug()<<"1";
    if(!file.open(QIODevice::WriteOnly))
    {
      QMessageBox message(QMessageBox::NoIcon, "错误", "无法读取信息", QMessageBox::Ok, NULL);
      message.exec();
      return;
    }
    QTextStream out(&file);
    qDebug()<<"2";
    QString str=model->stringList().join("#");
    qDebug()<<str;
    out<<str;
    file.close();
    close();
}
