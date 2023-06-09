#include "petritabwidget.h"
//#include "xmlwriter.h"

PetriTabWidget::PetriTabWidget(const QString &id, QWidget * parent)
    : QWidget(parent),ruleManager(RuleManager())
{
    this->id = id;
    name = id;
    createTab ();
    //初始化组件列表类
    componentlist=new ComponentList(scene,view);
}

PetriTabWidget::PetriTabWidget(const PTNET_ATTR &ptnet, const QString& file,QMap<QString,int>Comp2Count)
{
    id = ptnet.id;
    name = ptnet.name;
    filename = file;

    createTab ();

    if(!ptnet.pages.empty())
        ruleManager=(ptnet.pages[0].rules);
    else
        ruleManager=RuleManager();

    //初始化组件列表类
    componentlist=new ComponentList(scene,view,Comp2Count);
    // xml(待处理)
    scene->from_Xml (ptnet.pages);
    view->centerOn(scene->itemsBoundingRect().center());
    foreach(COMPONENT_ATTR component,ptnet.pages[0].componentList)
    {
        componentlist->addComponent(new Component(component));
    }
}
PetriTabWidget::PetriTabWidget(const QString& ID,const QString& Name, const QString& file)
{
    id =ID;
    name=Name;
    filename = file;
    createTab ();
    componentlist=new ComponentList(scene,view);
}
//添加组件
void PetriTabWidget::addComponent(const COMPONENT_ATTR &componentAttr)
{
//    id = ptnet.id;
//    name = ptnet.name;

    componentlist->addComponent(componentAttr);
    // xml （待处理
//    scene->from_Xml_Component (ptnet.pages);

    // 为视图设置场景 (待处理
//    view->centerOn(scene->itemsBoundingRect().center());

}

//返回组件列表中下表为i的组件属性
QString PetriTabWidget::getComponentType(int i)
{
//    if(i<this->component_vector.size())
//    {
//        return this->component_vector[i]->Component_type;
//    }
//    else
//    {
//        return this->component_vector[0]->Component_type;
//    }

}

int PetriTabWidget::getComponentSize()
{
//    return this->component_vector.size();
    return componentlist->getComponentList().size();
}
void PetriTabWidget::unbindComponent()
{
    scene->unbindComponent();
}
void PetriTabWidget::bindComponent()
{
    scene->bindComponent();
}
void PetriTabWidget::createTab ()
{
    mode = normalMode;
    setWindowTitle(name);
    scaleVal = 100;

    view = new QGraphicsView(this);
    view->setAlignment(Qt::AlignCenter);
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setRenderHints(QPainter::SmoothPixmapTransform);
    //    layout = new QHBoxLayout;  这三行是未知的神秘代码
    //    layout->addWidget(view);
    //    setLayout(layout);
    scene = new PTNscene(this);
    view->setScene (scene);
    /*init the view on left-top position*/
    view->centerOn(-INT_MAX, -INT_MAX);
    this->mynet=new PTNet();
    //undostack = new UndoStack(mynet,this);// 专用于控件
    undostack = new UndoStack(this);
    //graphVis = new GraphVisualizer;    //跟graphvisualizer相关的都不知道是啥

    arcEditDialog = new ArcEditDialog(this);
    placeEditDialog = new PlaceEditDialog(this);
    transEditDialog = new TransitionEditDialog(this);

    //这里面把主页面点击工具栏按钮的信号全都连接上了
    connect_sigs_slots ();
}

void PetriTabWidget::nodesInserted (const QStringList& names)
{
    nodes_names << names;
}

void PetriTabWidget::undo ()
{
    undostack->undo();
}

void PetriTabWidget::redo ()
{
    undostack->redo();
}

bool PetriTabWidget::canRedo ()
{
    return undostack->canRedo();
}

bool PetriTabWidget::canUndo ()
{
    return undostack->canUndo();
}

/* add new node to the current scene */
void PetriTabWidget::nodeInserted(const QPointF &itemPos,
                                  const QString &id)
{
    if(mode == addPlaceMode)
    {
        undostack->push(new AddPlaceCommand(itemPos, id, scene));
        nodes_names << id;
        //bug
        //mynet->AddPlace(new Place(id,itemPos));
    }else
    {
        undostack->push(new AddTransitionCommand(itemPos, id, scene));
        nodes_names << id;
        //bug
        //mynet->AddTransition(new Transition(id,itemPos));
    }
}

/* set Mode */
void PetriTabWidget::setMode (int mode)
{
    scene->setMode(mode);
    this->mode = mode;
    setCursorShape ();
}

/* get mode */
int PetriTabWidget::getMode () const
{
    return mode;
}

/* change the cursor shape */
void PetriTabWidget::setCursorShape ()
{
    QCursor cursor;

    if ((mode == normalMode)||(mode == animationMode))
        cursor.setShape(Qt::ArrowCursor);
    else
        cursor.setShape(Qt::CrossCursor);

    view->setCursor(cursor);
}

/* is scene empty */
bool PetriTabWidget::isNetEmpty()
{
    return scene->items().isEmpty();
}

bool PetriTabWidget::isSaved ()
{
    //qDebug()<<(undostack->isClean()&&arcnoclicked);
    return undostack->isClean();//&&arcnoclicked;
}

void PetriTabWidget::scaleView (qreal val)
{
    view->resetMatrix();
    view->scale(val/100, val/100);
    scene->update(scene->sceneRect());
    scaleVal = val;
}

qreal PetriTabWidget::scaleValue ()
{
    return scaleVal;
}

/* call this function when the delete Toolbar button is trigered */
void PetriTabWidget::removeItems ()
{
    //同步删除PTNet中的node
    //    foreach(QGraphicsItem * item, scene->selectedItems())
    //    {
    //        if(item->type()==Place::Type)
    //        {
    //            mynet->deletePlace(qgraphicsitem_cast<Place*>(item));
    //        }
    //        else if(item->type()==Transition::Type)
    //        {
    //            mynet->deleteTransition(qgraphicsitem_cast<Transition*>(item));
    //        }
    //    }
    scene->removeItems ();

}

const QString& PetriTabWidget::getFilename ()
{
    return filename;
}

void PetriTabWidget::setFilename (const QString& filenm)
{
    filename = filenm;
    QFileInfo fileInfo(filename);
    name = fileInfo.baseName();
}

void PetriTabWidget::cleanUndostack()
{
    undostack->setClean();
    // here clean all invisible items
}

void PetriTabWidget::setName(QString name)
{
    this->name = name;
}

void PetriTabWidget::setId(QString id)
{
    this->id = id;

}

PTNET_ATTR PetriTabWidget::toXml() const
{
    PTNET_ATTR net;
    net.id = id;
    net.name = name;
    PAGE_ATTR page;
    page.id = "page0";
    page.name = name;

    foreach(QGraphicsItem * item, scene->items())
    {
        if(item->type()==QGraphicsItemGroup::Type)
        {
              COMPONENT_ATTR l_component;
              QList<QGraphicsItem*>items=item->childItems();
              int l_length=items.size();
              QStringList l_itemName;
              if(items[0]->type() == Place::Type)
              {
                  l_itemName=qgraphicsitem_cast<Place*>(items[0])->getId().split('&');
                  l_component.name=l_itemName[0];
                  l_component.id=l_itemName[0]+'&'+l_itemName[1];
              }
              else if(items[0]->type() == Transition::Type)
              {
                  l_itemName=qgraphicsitem_cast<Transition*>(items[0])->getId().split('&');
                  l_component.name=l_itemName[0];
                  l_component.id=l_itemName[0]+'&'+l_itemName[1];
              }
              else if(items[0]->type() == Arcus::Type)
              {
                  l_itemName=qgraphicsitem_cast<Arcus*>(items[0])->getId().split('&');
                  l_component.name=l_itemName[0];
                  l_component.id=l_itemName[0]+'&'+l_itemName[1];
              }
              for(int i=0;i<l_length;i++)
              {
                  if(items[i]->type() == Place::Type)
                  {
                      l_component.placeNodes << qgraphicsitem_cast<Place*>(items[i])->toXml();
                  }
                  else if(items[i]->type() == Transition::Type)
                  {
                      l_component.transitionNodes<<qgraphicsitem_cast<Transition*>(items[i])->toXml();
                  }
                  else if(items[i]->type() == Arcus::Type)
                  {
                      l_component.arcs<<qgraphicsitem_cast<Arcus*>(items[i])->toXml();
                  }
              }
              page.componentList<<l_component;
//            foreach(QGraphicsItem*i,item->childItems())
//            {
//                if(i->type() == Place::Type)
//                {
//                    page.placeNodes << qgraphicsitem_cast<Place*>(i)->toXml();
//                    continue;
//                }
//                if(i->type() == Transition::Type)
//                {
//                    page.transitionNodes << qgraphicsitem_cast<Transition*>(i)->toXml();
//                    continue;
//                }
//                if(i->type() == Arcus::Type)
//                {
//                    page.arcs << qgraphicsitem_cast<Arcus*>(i)->toXml();
//                    continue;
//                }
////                if(i->type()==Connector::Type)
////                {
////                    page.connector << qgraphicsitem_cast<Connector*>(i)->toXml();
////                    continue;
////                }
//            }
        }
        else
        {
//            if(item->type() == Place::Type)
//            {
//                page.placeNodes << qgraphicsitem_cast<Place*>(item)->toXml();
//                continue;
//            }
//            if(item->type() == Transition::Type)
//            {
//                page.transitionNodes << qgraphicsitem_cast<Transition*>(item)->toXml();
//                continue;
//            }
//            if(item->type() == Arcus::Type)
//            {
//                page.arcs << qgraphicsitem_cast<Arcus*>(item)->toXml();
//                continue;
//            }
            if(item->type()==Connector::Type)
            {
                page.connector << qgraphicsitem_cast<Connector*>(item)->toXml();
                continue;
            }
        }
    }

    page.rules<<ruleManager.toXML();

    net.pages << page;


    return net;
}
//与toXML()函数相比，这个函数仅针对于选中的部分进行保存，并且没有保存规则库（2023.2.28）
PTNET_ATTR PetriTabWidget::componentToXml() const
{
    PTNET_ATTR net;
    net.id = id;
    net.name = name;
    PAGE_ATTR page;
    COMPONENT_ATTR component;
    component.id=id;
    component.name=name;
    component.step=default_step;
    page.id = "page0";
    page.name = name;
    int arc_count=0;
    int place_count=0;
    int transition_count=0;
    QVector<QGraphicsItem*>l_vArcItem;

    QMap<QString,QString>l_mOldID2NewID;
    QString l_newID;
    Place*l_place;
    Transition*l_transition;
    Arcus*l_arc;
    foreach(QGraphicsItem * item, scene->selectedItems())
    {

        if(item->type()==QGraphicsItemGroup::Type)
        {
            foreach(QGraphicsItem*i,item->childItems())
            {
                if(i->type() == Place::Type)
                {
                    l_newID=component.id+"&p"+QString::number(place_count);
                    l_place=qgraphicsitem_cast<Place*>(i);
                    l_mOldID2NewID[l_place->getId()]=l_newID;
                    l_place->setPlaceID(l_newID);
                    place_count++;
                    component.placeNodes << l_place->toXml();
                    continue;
                }
                if(i->type() == Transition::Type )
                {
                    l_newID=component.id+"&t"+QString::number(transition_count);
                    l_transition= qgraphicsitem_cast<Transition*>(i);
                    l_mOldID2NewID[l_transition->getId()]=l_newID;
                    l_transition->setID(l_newID);
                    transition_count++;
                    component.transitionNodes << l_transition->toXml();
                    continue;
                }
                if(i->type() == Arcus::Type)
                {
//                    l_arc=qgraphicsitem_cast<Arcus*>(i);
//                    l_arc->setID(component.id+"&a"+QString::number(arc_count));
//                    arc_count++;
//                    l_arc->setsourceId(l_mOldID2NewID[l_arc->getSourceId()]);
//                    l_arc->setTargetId(l_mOldID2NewID[l_arc->getTargetId()]);
//                    component.arcs << l_arc->toXml();
                    l_vArcItem.push_back(i);
                    continue;
                }
            }
        }
        else
        {
            if(item->type() == Place::Type)
            {
                l_newID=component.id+"&p"+QString::number(place_count);
                l_place=qgraphicsitem_cast<Place*>(item);
                l_mOldID2NewID[l_place->getId()]=l_newID;
                l_place->setPlaceID(l_newID);
                place_count++;
                component.placeNodes << l_place->toXml();
                continue;
            }
            if(item->type() == Transition::Type )
            {
                l_newID=component.id+"&t"+QString::number(transition_count);
                l_transition= qgraphicsitem_cast<Transition*>(item);
                l_mOldID2NewID[l_transition->getId()]=l_newID;
                l_transition->setID(l_newID);
                transition_count++;
                component.transitionNodes << l_transition->toXml();
                continue;
            }
            if(item->type() == Arcus::Type)
            {
//                l_arc=qgraphicsitem_cast<Arcus*>(item);
//                l_arc->setID(component.id+"&a"+QString::number(arc_count));
//                arc_count++;

//                l_arc->setsourceId(l_mOldID2NewID[l_arc->getSourceId()]);
//                l_arc->setTargetId(l_mOldID2NewID[l_arc->getTargetId()]);
//                component.arcs << l_arc->toXml();
                l_vArcItem.push_back(item);
                continue;
            }
        }
    }
    foreach( QGraphicsItem * i,l_vArcItem)
    {
         l_arc=qgraphicsitem_cast<Arcus*>(i);
         l_arc->setID(component.id+"&a"+QString::number(arc_count));
         arc_count++;
         l_arc->setsourceId(l_mOldID2NewID[l_arc->getSourceId()]);
         l_arc->setTargetId(l_mOldID2NewID[l_arc->getTargetId()]);
         component.arcs << l_arc->toXml();
    }
    page.componentList<<component;
    net.pages << page;
    return net;
}
void PetriTabWidget::exportNet (const QString &imagefile)
{
    QRectF rect = scene->itemsBoundingRect();
    QImage image(QSize(rect.width(), rect.height()), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
    QPainter painter;

    painter.begin(&image);
    scene->render(&painter , QRectF(), rect.adjusted(-10,-10,10,10).normalized(),
                  Qt::IgnoreAspectRatio);
    painter.end();

    image.save(imagefile);
}

bool PetriTabWidget::checkNet ()
{
    bool ok = false;

    emit errorMessage ("  <strong style=\"color:blue;\">Starting " \
                       " simulation of "+name+" ... </strong>");

    ok = checkMarking();

    if(!ok)
        emit errorMessage ("  <span style=\"color:red;\">Error:"\
                           "No firable transition found ...</span>");

    ok = checkNodesConnections();

    checkNodesNames();

    return ok;
}

//![0] check marking before animation
bool PetriTabWidget::checkMarking()
{
    bool ok = false;

    foreach(QGraphicsItem * item ,scene->items())
    {
        if(item->type()==QGraphicsItemGroup::Type)
        {
            foreach(QGraphicsItem*i,item->childItems())
            {
                if(i->type() == Transition::Type)
                {
                    if(qgraphicsitem_cast<Transition*>(i)->isFirable ())
                    {
                        ok = true;
                        break;
                    }
                }
            }
        }
        else
        {
            if(item->type() == Transition::Type)
            {
                if(qgraphicsitem_cast<Transition*>(item)->isFirable ())
                {
                    ok = true;
                    break;
                }
            }
        }

    }

    return ok;
}

//![1] check nodes connections!
bool PetriTabWidget::checkNodesConnections()
{
    bool ok = true;

    foreach(QGraphicsItem * item , scene->items())
    {
        if(item->type()==QGraphicsItemGroup::Type)
        {
            foreach(QGraphicsItem*i,item->childItems())
            {
                if(i->type() == Place::Type)
                {
                    Place * place = qgraphicsitem_cast<Place*>(i);
                    if(!place->hasRelations())
                    {
                        emit errorMessage ("  <span style=\"color:red;\">Error: Place "+
                                           place->getName()+" is not connected ...</span>");
                        ok = false;
                    }
                }
                if(i->type() == Transition::Type)
                {
                    Transition * transition = qgraphicsitem_cast<Transition*>(i);
                    if(!transition->hasRelations())
                    {
                        emit errorMessage ("  <span style=\"color:red;\">Error: Transition "+
                                           transition->getName()+" is not connected ...</span>");
                        ok = false;
                    }
                }
            }
        }
        else
        {
            if(item->type() == Place::Type)
            {
                Place * place = qgraphicsitem_cast<Place*>(item);
                if(!place->hasRelations())
                {
                    emit errorMessage ("  <span style=\"color:red;\">Error: Place "+
                                       place->getName()+" is not connected ...</span>");
                    ok = false;
                }
            }
            if(item->type() == Transition::Type)
            {
                Transition * transition = qgraphicsitem_cast<Transition*>(item);
                if(!transition->hasRelations())
                {
                    emit errorMessage ("  <span style=\"color:red;\">Error: Transition "+
                                       transition->getName()+" is not connected ...</span>");
                    ok = false;
                }
            }
        }

    }

    return ok;
}

//![2] check nodes names uniqueness!
void PetriTabWidget::checkNodesNames()
{

    QStringList plnames;
    QStringList trnames;

    foreach(QGraphicsItem * item , scene->items())
    {
        if(item->type()==QGraphicsItemGroup::Type)
        {
            foreach(QGraphicsItem*i,item->childItems())
            {
                if(i->type() == Place::Type)
                    plnames << (qgraphicsitem_cast<Place*>(i))->getName();

                if(i->type() == Transition::Type)
                    trnames << (qgraphicsitem_cast<Transition*>(i))->getName();
            }
        }
        else
        {
            if(item->type() == Place::Type)
                plnames << (qgraphicsitem_cast<Place*>(item))->getName();

            if(item->type() == Transition::Type)
                trnames << (qgraphicsitem_cast<Transition*>(item))->getName();
        }

    }

    if(plnames.removeDuplicates())
        emit errorMessage ("  <span style=\"color:orange;\">Warning: Places names are not unique!</span>");


    if(trnames.removeDuplicates())
        emit errorMessage ("  <span style=\"color:orange;\">Warning: Transitions names are not unique!</span>");

}

PTNscene* PetriTabWidget::getSCene()
{
    return this->scene;
}

QString PetriTabWidget::getName()
{
    return this->name;
}

RuleManager &PetriTabWidget::getRuleManager()
{
    return ruleManager;
}
void PetriTabWidget::setRuleManager(RuleManager manager)
{
    ruleManager=manager;
}
/* error message */
void PetriTabWidget::showErrorMessage (const QString &title, const QString &errorMsg)
{
    QMessageBox::critical(this, title, errorMsg);
}

//GraphVisualizer * PTNtab::createGraphVis ()
//{
//	Marking initial_marking = scene->getInitialMarking ();
//	QMap<QString, int> places_capacities = scene->getPlacesCapacities ();
//	QList<TRANS_RELATION> pt_relations = scene->getRelations ();
//	QMap<QString, QString> places_names = scene->getPlacesNames ();

//	graphVis->visualize_graph(initial_marking, places_capacities, places_names, pt_relations);

//	return graphVis;
//}

//GraphVisualizer * PTNtab::getGraphVis ()
//{
//	return graphVis;
//}

void PetriTabWidget::connect_sigs_slots ()
{
    connect (scene, &PTNscene::nodeInserted, this,  &PetriTabWidget::nodeInserted);

    connect (scene, &PTNscene::arcInserted, undostack,  &UndoStack::arcInserted);

    connect (scene, &PTNscene::connectorInserted, undostack,  &UndoStack::connectorInserted);

    connect (scene, &PTNscene::itemMoved, undostack,  &UndoStack::itemMoved);

    connect (scene, &PTNscene::nodeRemoved, undostack, &UndoStack::nodeRemoved);

    connect (scene, &PTNscene::arcRemoved, undostack,  &UndoStack::arcRemoved);

    connect (scene, &PTNscene::nodesInserted, this,  &PetriTabWidget::nodesInserted);

    connect (undostack, &QUndoStack::canRedoChanged, this, &PetriTabWidget::canRedoChange);
    connect (undostack, &QUndoStack::canUndoChanged, this, &PetriTabWidget::canUndoChange);
    connect (undostack, &QUndoStack::cleanChanged, this, &PetriTabWidget::netChanged);

    connect (scene, &PTNscene::itemDoubleClicked, this, &PetriTabWidget::itemDoubleClicked );
}

void PetriTabWidget::itemDoubleClicked (QGraphicsItem* item)
{
    if(item->type() == Place::Type)
        placeDoubleClicked (item);
    if(item->type() == Transition::Type)
        transitionDoubleClicked (item);
    if(item->type() == Arcus::Type)
        arcDoubleClicked (item);

    item = 0;
}

void PetriTabWidget::save()
{
//    Component*com=new Component(this->toXml(),scene);
//    component_vector.push_back(com);
}

void PetriTabWidget::componentSave()
{
//    Component*com=new Component(this->componentToXml(),scene);
//    component_vector.push_back(com);
}

void PetriTabWidget::placeDoubleClicked (QGraphicsItem* item)
{
    Place * place = qgraphicsitem_cast<Place*>(item);
    QString old_name = place->getName();
    placeEditDialog->inputLabel->setText(old_name);
    placeEditDialog->inputTokens->setText(QString::number(place->getTokens()));
    placeEditDialog->m_placeID->setText(place->getId());

    placeEditDialog->m_plainTextEdit->setText(place->getComment());
    placeEditDialog->m_showComment->setChecked(place->getShow());
    placeEditDialog->m_inputPort->setChecked(place->isInputPort());
    placeEditDialog->m_outputPort->setChecked(place->isOutputPort());
    placeEditDialog->m_notPort->setChecked(!place->isInputPort()&&!place->isOutputPort());

    //set Color
    placeEditDialog->m_graphicsEditTab->setBrushColor(place->getBrushColor());
    placeEditDialog->m_graphicsEditTab->setPenColor(place->getPenColor());
    placeEditDialog->m_graphicsEditTab->setBrushColorOrin(place->getBrushColor());
    placeEditDialog->m_graphicsEditTab->setPenColorOrin(place->getPenColor());

    placeEditDialog->exec();

    if(placeEditDialog->result() == QDialog::Rejected)
        return;

    QString new_name = placeEditDialog->inputLabel->text();
    place->setTokens(placeEditDialog->inputTokens->text().toDouble());

    //set color
    place->setPenColor(placeEditDialog->m_graphicsEditTab->getPenColor());
    place->setBrushColor(placeEditDialog->m_graphicsEditTab->getBrushColor());
    place->setName(new_name);
    int myflag=0;
    foreach(QGraphicsItem * item1, scene->items())
    {
        if(item->type()==QGraphicsItemGroup::Type)
        {
            foreach(QGraphicsItem*i,item->childItems())
            {
                if(i->type() == Place::Type)
                {
                    if(qgraphicsitem_cast<Place*>(i)->getName()==new_name&&qgraphicsitem_cast<Place*>(i)->getId()!=place->getId())
                    {
                        myflag=1;
                        qgraphicsitem_cast<Place*>(i)->setTokens(place->getTokens());
                    }
                }
            }
        }
        else
        {
            if(item1->type() == Place::Type)
            {
                if(qgraphicsitem_cast<Place*>(item1)->getName()==new_name&&qgraphicsitem_cast<Place*>(item1)->getId()!=place->getId())
                {
                    myflag=1;
                    qgraphicsitem_cast<Place*>(item1)->setTokens(place->getTokens());
                }
            }
        }

    }
    if(myflag==1)
    {
        QMessageBox::about(this,tr("Same Name Exists"),tr("Nodes will be synchronized."));
    }
    place->f_setComment(placeEditDialog->m_plainTextEdit->toPlainText());

    if(!placeEditDialog->m_showComment->isChecked())
    {
        place->setShow(false);
        place->setLabel_1();
    }
    else
    {
        place->setShow(true);
        place->setLabel_2();
    }
    //端口识别和判断选择
    if(placeEditDialog->m_inputPort->isChecked()&&!placeEditDialog->m_outputPort->isChecked())
    {
        place->setOutputPort(false);
        place->setInputPort(true);
    }
    else if(placeEditDialog->m_outputPort->isChecked()&&!placeEditDialog->m_inputPort->isChecked())
    {
        place->setInputPort(false);
        place->setOutputPort(true);

    }
    if(placeEditDialog->m_inputPort->isChecked()&&placeEditDialog->m_outputPort->isChecked())
    {
        place->setOutputPort(true);
        place->setInputPort(true);
        place->setCompoundPort(true);
    }
    else if(placeEditDialog->m_notPort->isChecked())
    {
        place->setInputPort(false);
        place->setOutputPort(false);
    }
}

void PetriTabWidget::transitionDoubleClicked (QGraphicsItem* item)
{
    Transition * trans = qgraphicsitem_cast<Transition*>(item);
    QString old_name = trans->getName();
    QString old_function=trans->getFunction();
    //QString old_MassAction=trans->getAction();
    transEditDialog->inputLabel->setText(old_name);
    transEditDialog->inputLabel1->setText(old_function);
    //transEditDialog->inputLabel1->setText(old_MassAction);
    transEditDialog->m_plainTextEdit->setText(trans->getComment());
    transEditDialog->slider->setValue(trans->getRotation());
    transEditDialog->m_transitionID->setText(trans->getId());
    /////transEditDialog->m_transitionID->setText(QString::number(nodes_names.indexOf(old_name)));
    transEditDialog->m_showComment->setChecked(trans->getShow());
    //set color
    transEditDialog->m_graphicsEditTab->setPenColor(trans->getPenColor());
    transEditDialog->m_graphicsEditTab->setBrushColor(trans->getBrushColor());
    transEditDialog->m_graphicsEditTab->setPenColorOrin(trans->getPenColor());
    transEditDialog->m_graphicsEditTab->setBrushColorOrin(trans->getBrushColor());

    transEditDialog->exec();

    if(transEditDialog->result() == QDialog::Rejected)
        return;

    QString new_name = transEditDialog->inputLabel->text();
    QString new_function=transEditDialog->inputLabel1->text();
    //QString new_MassAction=transEditDialog->inputLabel2->text();
    trans->setRotation (transEditDialog->slider->value());
    trans->setFunction(new_function);
    //trans->setAction(new_MassAction);

    //set color
    trans->setPenColor(transEditDialog->m_graphicsEditTab->getPenColor());
    trans->setBrushColor(transEditDialog->m_graphicsEditTab->getBrushColor());
    trans->setName(new_name);
    int myflag=0;
    foreach(QGraphicsItem * item1, scene->items())
    {
        if(item->type()==QGraphicsItemGroup::Type)
        {
            foreach(QGraphicsItem*i,item->childItems())
            {
                if(i->type() == Transition::Type)
                {
                    if(qgraphicsitem_cast<Transition*>(i)->getName()==new_name&&qgraphicsitem_cast<Transition*>(i)->getId()!=trans->getId())
                    {
                        myflag=1;
                        qgraphicsitem_cast<Transition*>(i)->setFunction(trans->getFunction());
                    }
                }
            }
        }
        else
        {
            if(item1->type() == Transition::Type)
            {
                if(qgraphicsitem_cast<Transition*>(item1)->getName()==new_name&&qgraphicsitem_cast<Transition*>(item1)->getId()!=trans->getId())
                {
                    myflag=1;
                    qgraphicsitem_cast<Transition*>(item1)->setFunction(trans->getFunction());
                }
            }
        }

    }
    if(myflag==1)
    {
        QMessageBox::about(this,tr("Same Name Exists"),tr("Nodes will be synchronized."));
    }
    /*
    int index = nodes_names.indexOf (old_name);

    if (new_name != old_name)
    {
        if(!nodes_names.contains(new_name))
        {
            nodes_names.replace (index, new_name);
            trans->setName(new_name);
        }
        else
        {
            QMessageBox::StandardButton button = QMessageBox::warning(this, "Not a unique name", "The transition name <strong>"+new_name+"</strong> is already being used. Use it anyway?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
            if(button == QMessageBox::Yes)
            {
                nodes_names.replace (index, new_name);
                trans->setName(new_name);
            }
            foreach(QGraphicsItem * item, scene->items())
            {
                if(item->type() == Transition::Type)
                {
                    if(qgraphicsitem_cast<Transition*>(item)->getName()==new_name&&qgraphicsitem_cast<Transition*>(item)->getId()!=trans->getId())
                    {
                        qgraphicsitem_cast<Transition*>(item)->setFunction(trans->getFunction());
                    }
                }
            }
        }
    }
    else
    {
        foreach(QGraphicsItem * item, scene->items())
        {
            if(item->type() == Transition::Type)
            {
                if(qgraphicsitem_cast<Transition*>(item)->getName()==new_name&&qgraphicsitem_cast<Transition*>(item)->getId()!=trans->getId())
                {
                    qgraphicsitem_cast<Transition*>(item)->setFunction(trans->getFunction());
                }
            }
        }
    }
    */
    //trans->f_setFunction(transEditDialog->m_functionEdit->toPlainText());
    trans->f_setComment(transEditDialog->m_plainTextEdit->toPlainText());
    if(!transEditDialog->m_showComment->isChecked())
    {
        trans->setShow(false);
        trans->setLabel_1();
    }
    else
    {
        trans->setShow(true);
        trans->setLabel_2();
    }
}

void PetriTabWidget::arcDoubleClicked (QGraphicsItem* item)
{
    undostack->arcClicked(item, scene);
    //arcnoclicked=false;
    Arcus* arc = qgraphicsitem_cast<Arcus*>(item);
    //	/* set the SpinBox with the arc's current weight */
    //	//arcEditDialog->inputWeight->setText(QString::number(arc->getWeight(),'f',10));
    arcEditDialog->inputWeight->setText(QString("%1").arg(arc->getWeight()));
    //	//arcEditDialog->m_experssionEdit->setText(arc->f_getExperssion());
    //	//arcEditDialog->exec();
    //	//FL
    QVector<QString> l_vPlaces;



    Transition *l_transition;

    if(arc->getSourceItem()->type()==Transition::Type)
    {
        l_transition=qgraphicsitem_cast<Transition*>(arc->getSourceItem());
    }
    else
    {
        l_transition=qgraphicsitem_cast<Transition*>(arc->getTargetItem());
    }//find transition
    foreach(QGraphicsItem * item, scene->items())
    {
        if(item->type()==QGraphicsItemGroup::Type)
        {
            foreach(QGraphicsItem*i,item->childItems())
            {
                if(i->type()==Arcus::Type)
                {
                    Arcus *l_arc=qgraphicsitem_cast<Arcus*>(i);
                    if(l_arc->getSourceItem()->type()==Transition::Type)
                    {
                        if((qgraphicsitem_cast<Transition*>(l_arc->getSourceItem()))->getName()==l_transition->getName())
                        {
                            l_vPlaces.push_back(qgraphicsitem_cast<Place*>(l_arc->getTargetItem())->getName());
                        }
                    }
                    else
                    {
                        if((qgraphicsitem_cast<Transition*>(l_arc->getTargetItem()))->getName()==l_transition->getName())
                        {
                            l_vPlaces.push_back(qgraphicsitem_cast<Place*>(l_arc->getSourceItem())->getName());
                        }
                    }
                }
            }
        }
        else
        {
            if(item->type()==Arcus::Type)
            {
                Arcus *l_arc=qgraphicsitem_cast<Arcus*>(item);
                if(l_arc->getSourceItem()->type()==Transition::Type)
                {
                    if((qgraphicsitem_cast<Transition*>(l_arc->getSourceItem()))->getName()==l_transition->getName())
                    {
                        l_vPlaces.push_back(qgraphicsitem_cast<Place*>(l_arc->getTargetItem())->getName());
                    }
                }
                else
                {
                    if((qgraphicsitem_cast<Transition*>(l_arc->getTargetItem()))->getName()==l_transition->getName())
                    {
                        l_vPlaces.push_back(qgraphicsitem_cast<Place*>(l_arc->getSourceItem())->getName());
                    }
                }
            }
        }

    }





    //	//zyc
    //	/*
    //	l_vPlaces.push_back(QString("X1"));
    //	l_vPlaces.push_back(QString("X2"));
    //	l_vPlaces.push_back(QString("X3"));
    //	l_vPlaces.push_back(QString("X4"));
    //	l_vPlaces.push_back(QString("X5"));*/

    arcEditDialog->setlPlaces(l_vPlaces);
    //	//zyc
    //    arcEditDialog->setFIS_a(arc->getFISStruct());//非&,m_FIS_a,
    //    arcEditDialog->SetFISStruct(arc->GetFISStruct());//将获得的m_fis传输,&,m_pFIS
    //	//arcEditDialog->setRuleSet(arc->GetRuleSet());

    //set color
    arcEditDialog->m_graphicsEditTab->setPenColor(arc->getPenColor());
    arcEditDialog->m_graphicsEditTab->setBrushColor(arc->getBrushColor());
    arcEditDialog->m_graphicsEditTab->setPenColorOrin(arc->getPenColor());
    arcEditDialog->m_graphicsEditTab->setBrushColorOrin(arc->getBrushColor());

    arcEditDialog->exec();
    if (arcEditDialog->result() == QDialog::Rejected) return;


    //	/* update the arc's weight */
    if(arcEditDialog->result() == QDialog::Accepted)
        //		//arc->setWeight(arcEditDialog->inputWeight->text().toDouble());
    {
        arc->setWeight(arcEditDialog->inputWeight->text().toDouble());
        //		//zyc
        //		arc->createRuleSet();
        //        if(arc->getFISStruct().m_sFISName.size())
        //            arc->setExpression(QString::fromStdString(arc->getFISStruct().m_sFISName));
    }
    //	//lf
    //	arcEditDialog->SetFISStruct(arc->GetFISStruct());
    //	//arc->f_setExperssion(arcEditDialog->m_experssionEdit->text());

    //	/*if(!arcEditDialog->m_showExperssion->isChecked())
    //	{
    //		arc->f_setLabel_1();
    //	}
    //	else
    //	{
    //		arc->f_setLabel_2();
    //	}*/

    //	//set color
    arc->setPenColor(arcEditDialog->m_graphicsEditTab->getPenColor());
    arc->setBrushColor(arcEditDialog->m_graphicsEditTab->getBrushColor());
}

PetriTabWidget::~PetriTabWidget ()
{
    //delete placeEditDialog;
    //delete transEditDialog;
    //delete arcEditDialog;
    delete scene;
    //delete graphVis;
    delete view;
    //delete layout;
    delete undostack;
}

QVector<Component*> PetriTabWidget::getCompVector()
{
    return componentlist->getComponentList();
}

ComponentList*PetriTabWidget::getComponentList()
{
    return componentlist;
}

void PetriTabWidget::setComponentController(ComponentController*compController)
{
    componentlist->setComponentController(compController);
}

void PetriTabWidget::setConnector_AttrList(QList<Connector*>l)
{
    componentlist->initConnector_list(l);
}

void PetriTabWidget::editComponentStep(QString componentName,double componentStep)
{
    componentlist->setComponentStep(componentName,componentStep);
}

void PetriTabWidget::editComponentID(QString oldName,QString newName)
{
    componentlist->editComponentID(oldName,newName);
}

bool PetriTabWidget::containComponent(QString componentName)
{
    return componentlist->containComponent(componentName);
}

void PetriTabWidget::addConnector(QList<CONNECTOR_ATTR>l_connectorAttr)
{
    scene->addXML_connectors(l_connectorAttr);
}
void PetriTabWidget::updateCompAttrInComList(QList<COMPONENT_ATTR>compAttrList)
{
    componentlist->getComponentAttr(compAttrList);
}
