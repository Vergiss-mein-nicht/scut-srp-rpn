#include "transition.h"


const QColor Transition::defalut_brushColor = Qt::white;
const QColor Transition::defalut_penColor = Qt::black;

Transition::Transition(const QString &id)
{
    this->id = id;
    name = id;
    rotation = 0;

    createTransition();

    label->setPos(30, 10);
}

/* Transition from XML data */
Transition::Transition(const TRANSITION_ATTR &transition)
{
  id = transition.id;
  m_comment = transition.comment;
  self_function = transition.self_function;
  m_brushColor = transition.brushColor;
  m_penColor = transition.penColor;
  show = transition.show;
  if(transition.name.isEmpty())
     name = id;
  else
     name = transition.name;

  createTransition();

  label->setPos(transition.offsetx, transition.offsety);
  if(show)setLabel_2();
}

Transition::Transition(const QString &id, const QPointF &position)
{
    this->id=id;
    label->setPos(position.x(),position.y());
}

Transition::Transition()
{

}


void Transition::setShow(bool flag)
{
    show = flag;
}

QColor Transition::getBrushColor() const
{
    return m_brushColor;
}

QColor Transition::getPenColor() const
{
    return m_penColor;
}

void Transition::setBrushColor(QColor color)
{
    m_brushColor = color;
    update();
}

void Transition::setPenColor(QColor color)
{
    m_penColor = color;
    update();
}

bool Transition::isInComponent()
{
    return this->InComponent;
}

void Transition::setIncomponent(bool isInComponent)
{
    this->InComponent=isInComponent;
}

bool Transition::IsNormalPort()
{
    return this->isNormalPort;
}

bool Transition::IsCompoundPort()
{
    return this->isCompoundPort;
}

QList<Arcus *> Transition::getinput()
{
    return this->input;
}

QList<Arcus *> Transition::getoutput()
{
    return  this->output;
}

void Transition::pushInput(Arcus *a)
{
    this->input.push_back(a);
}

void Transition::pushOutput(Arcus *a)
{
    this->output.push_back(a);
}

void Transition::setNormalPort(bool flag)
{
    this->isNormalPort=flag;
}

void Transition::setCompoundPort(bool flag)
{
    this->isCompoundPort=flag;
}

QString Transition::getName(QString ComID)
{
    Q_ASSERT_X(this->getComponentID().size()<=2,"transition ID","the number of transition(in compoundPort) exceeds the limit");
    if(this->getComponentID()[0]==ComID)
    {
        return this->getId().split("+")[0].split("&")[2];
    }
    else if(this->getComponentID()[1]==ComID)
    {
        return this->getId().split("+")[1].split("&")[2];
    }
}

QStringList Transition::getFileName()
{
    QStringList id;
    id.push_back(this->getComponentID()[0].split("&")[0]);
    if(this->getComponentID().size()==2)
    {
        id.push_back(this->getComponentID()[1].split("&")[0]);
    }
    return id;
}

QStringList Transition::getComponentID()
{
    QStringList ID=this->getId().split("+");
    QStringList cID;
    cID.push_back(ID[0].split("&")[0]+"&"+ID[0].split("&")[1]);
    if(ID.size()==2)
    {
        cID.push_back(ID[1].split("&")[0]+"&"+ID[1].split("&")[1]);
    }
    return cID;
}

bool Transition::getShow()
{
    return show;
}

QString Transition::getComment()
{
    return m_comment;
}

void Transition::createTransition()
{

   setRect(0, 0, transition_width, transition_heigh);
   setZValue(1000.0);
   setFlag(QGraphicsItem::ItemIsMovable, true);
   setFlag(QGraphicsItem::ItemIsSelectable, true);
   setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

   label = new QGraphicsSimpleTextItem(this);
   label->setText(name);
   label->setFlag(QGraphicsItem::ItemIsMovable, true);
   label->setFlag(QGraphicsItem::ItemIsSelectable, true);

   mode = normalMode;
}

//todo
//XML

TRANSITION_ATTR Transition::toXml() const
{
  TRANSITION_ATTR transition;

  transition.id = id;
  transition.name = name;
  transition.x = mapToScene(QPointF(0,0)).x();
  transition.y = mapToScene(QPointF(0,0)).y();
  transition.rotation = rotation;
  transition.offsetx = label->x();
  transition.offsety = label->y();
  transition.self_function=self_function;
  //transition.MassAction=MassAction;
  transition.comment = m_comment;
  transition.show = show;
  transition.brushColor = m_brushColor;
  transition.penColor = m_penColor;

  return transition;
}

/* to generate coverability tree */


//todo
//库所-弧-变迁的关系
//TRANS_RELATION Transition::getTrans_Relations() const
//{
//  TRANS_RELATION relation;
//  relation.name = name;

//  foreach(Arc * arc, input)
//   {
//        Place * place = qgraphicsitem_cast<Place *>(arc->getSourceItem());
//        relation.preSet[place->getId()] = arc->getWeight();
//   }

//  foreach(Arc * arc, output)
//   {
//        Place * place = qgraphicsitem_cast<Place *>(arc->getTargetItem());
//        relation.postSet[place->getId()] = arc->getWeight();
//   }

//  return relation;
//}

Transition::~Transition ()
{

}

/* get id */
QString Transition::getId () const
{
    return id;
}
QString Transition::getFunction() const
{
    return self_function;
}
void Transition::setFunction(const QString& fc)
{
    self_function=fc;
}

/* get name */
QString Transition::getName () const
{
    return name;
}

void Transition::setName (const QString& nm)
{
    name = nm;
    label->setText(name);
}

/* set mode */
void Transition::setMode (int mod)
{
    mode = mod;
}

void Transition::setID(QString ID)
{
    this->id=ID;
}

QString Transition::f_getComment() const
{
    return m_comment;
}

void Transition::f_setComment(QString str)
{
    m_comment = str;
}

void Transition::f_setFunction(QString str)
{
    m_function=str;
}

QString Transition::f_getFunction() const
{
    return m_function;
}



//todo
//需要弧与库所的定义
/* is firable */
bool Transition::isFirable()
{
    double toks;

    // check input
    foreach(Arcus * arc, input)
    {
        double w = arc->getWeight();
        QGraphicsItem * sourceItem = arc->getSourceItem();

        if(sourceItem->type() == Place::Type)
        {
            toks = qgraphicsitem_cast<Place *>(sourceItem)->getTokens();
            if(toks < w)
                return false;
        }
    }

    // check output
    foreach(Arcus * arc, output)
    {
        QGraphicsItem * targetItem = arc->getTargetItem();
        if(targetItem->type() == Place::Type)
        {
            if(qgraphicsitem_cast<Place *>(targetItem)->reachCapacity(arc))
                return false;
        }
    }

  return true;
}


//todo
//需要弧与库所的定义
/* fire transition */
void Transition::fire()
{

  // update input places
  foreach(Arcus * arc, input)
  {
      double w = arc->getWeight();
      QGraphicsItem * sourceItem = arc->getSourceItem ();

      if(sourceItem->type() == Place::Type)
       qgraphicsitem_cast<Place *>(sourceItem)->updateTokens(-w);
  }

  // update output places
  foreach(Arcus * arc, output)
  {
      double w = arc->getWeight();
      QGraphicsItem * targetItem = arc->getTargetItem();

      if(targetItem->type() == Place::Type)
        qgraphicsitem_cast<Place *>(targetItem)->updateTokens(w);
  }
}

void Transition::setLabel_1()
{
    label->setText(name);
}
void Transition::setLabel_2()
{
    QString lb;
    lb=QString (name+"\n"+f_getComment());

    label->setText(lb);
}



/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
/* @@@@@@ draw methods           @@@@@@ */
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

//todo
//need arc
/* add input arc */
void Transition::addInputArc(QGraphicsItem * arc)
{
   input << qgraphicsitem_cast<Arcus*>(arc);
}

/* add output arc */
void Transition::addOutputArc(QGraphicsItem * arc)
{
   output << qgraphicsitem_cast<Arcus*>(arc);
}

/* delete Arc */
void Transition::deleteArc(QString id)
{

  for(int i=0; i < input.size(); i++)
    if(input[i]->getId() == id)
    {
        input.removeAt(i);
        return;
    }

  for(int i=0; i < output.size(); i++)
    if(output[i]->getId() == id)
    {
        output.removeAt(i);
        return;
    }

}

int Transition::getRotation() const
{
  return rotation;
}

/* rotate */
void Transition::setRotation(int degree)
{

 QTransform newtransform;
 newtransform.translate(transition_width/2, transition_heigh/2);
 newtransform.rotate(degree);
 newtransform.translate(-(transition_width/2), -(transition_heigh/2));
 setTransform(newtransform, false);

  //label->setPos(transform().inverted().map(QPointF(30, 10)));
  newtransform.reset();
  newtransform.rotate(-degree);
  label->setTransform(newtransform, false);

  QVariant v(degree);

  rotation = degree;
}

//todo
//input and output is ARC
bool Transition::hasRelations ()
{
    if (input.isEmpty() && output.isEmpty())
        return false;
    else
        return true;
}

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
/* @@@@@ re-implemented functions @@@@@ */
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

/* type */
int Transition::type () const
{
 return Type;
}

/* bounding rect */
QRectF Transition::boundingRect() const
{
  return QRectF(0, 0, transition_width, transition_heigh)
     .normalized();
}

/* shape */
QPainterPath Transition::shape() const
{
     QPainterPath shape;
     shape.addRect(0, 0, transition_width , transition_heigh);
     return shape;
}

/* item change */
//todo
//need arc
QVariant Transition::itemChange(GraphicsItemChange change, const QVariant &value)
{

     if ((change == QGraphicsItem::ItemPositionChange) ||
     (change == QGraphicsItem::ItemPositionHasChanged) ||
         (change == QGraphicsItem::ItemTransformChange) ||
     (change == QGraphicsItem::ItemTransformHasChanged))
    {
        foreach (Arcus *inarc, input)
            inarc->updatePosition();

        foreach (Arcus *outarc, output)
            outarc->updatePosition();

        scene()->update();
    }

  return value;
}

/* paint */
void Transition::paint ( QPainter * painter,
    const QStyleOptionGraphicsItem * option,
        QWidget * widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //如果该变迁没有输入弧与输出弧，此时变迁的颜色为红黑色
    //todo
    if((input.isEmpty()) && (output.isEmpty()))
    {
       color = Qt::darkRed;
    }
    else
     {
       //如果鼠标点击了该变迁，则该变迁与输入弧输出弧均为选中状态且颜色变为蓝色
       if(isSelected())
        {
          color = QColor(0, 0, 150);

          foreach(Arcus * arc, input)
             arc->setSelected(true);
          foreach(Arcus * arc, output)
             arc->setSelected(true);
        }
       //如果未点击该变迁，该变迁的颜色为m_penColor
       else
          color = m_penColor;
    }
    if(mode == animationMode)
    {
      if(isFirable())
         color = QColor(0, 150, 0);
      else
         color = QColor(150, 0, 0);//150,0,0
     }
    //让标签的颜色与变迁的颜色相同
    label->setBrush(color);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(m_brushColor);
    painter->setPen(color);
    painter->setFont(QFont("Times", 15));
    painter->drawRect(0, 0, transition_width, transition_heigh);
}

bool Transition::collidesWithItem ( const QGraphicsItem * other, Qt::ItemSelectionMode mode ) const
{
    Q_UNUSED(mode);

    QPainterPath path1 = mapFromItem(other, other->shape());
    return path1.intersects(this->shape());

}

Transition::Transition(const TRANSITION_ATTR &transition,int count)
{
    //组件内变迁的id格式为 A&tn ,其中A为组件类型，n为该变迁在组件内的标号
    QString transitionId=transition.id.split('&')[0]+'&'+QString::number(count)+'&'+transition.id.split('&')[1];
    id = transitionId;
    m_comment = transition.comment;
    self_function = transition.self_function;
    m_brushColor = transition.brushColor;
    m_penColor = transition.penColor;
    show = transition.show;
    if(transition.name.isEmpty())
       name = id;
    else
       name = transition.name;

    createTransition();

    label->setPos(transition.offsetx, transition.offsety);
    if(show)setLabel_2();
}
