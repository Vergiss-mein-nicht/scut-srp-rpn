#include "petritabwidgt.h"
//#include "xmlwriter.h"

PetriTabwidgt::PetriTabwidgt(const QString &id, QWidget * parent)
    : QWidget(parent)
{
    this->id = id;
    name = id;
    createTab ();
}

//PTNtab::PTNtab(const PTNET_ATTR &ptnet, const QString& file)
//{
//	id = ptnet.id;
//	name = ptnet.name;
//	filename = file;
//	createTab ();
//	// xml
//	scene->from_Xml (ptnet.pages);
//	view->centerOn(scene->itemsBoundingRect().center());
//}

void PetriTabwidgt::createTab ()
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
    undostack = new UndoStack(this);
    //graphVis = new GraphVisualizer;    //跟graphvisualizer相关的都不知道是啥

//    arcEditDialog = new ArcEditDialog(this);          // 编辑对话框相关，暂时先不搞了
//    placeEditDialog = new PlaceEditDialog(this);
//    transEditDialog = new TransitionEditDialog(this);

    connect_sigs_slots ();        //这里面把主页面点击工具栏按钮的信号全都连接上了
}

void PetriTabwidgt::nodesInserted (const QStringList& names)
{
    nodes_names << names;
}

void PetriTabwidgt::undo ()
{
    undostack->undo();
}

void PetriTabwidgt::redo ()
{
    undostack->redo();
}

bool PetriTabwidgt::canRedo ()
{
    return undostack->canRedo();
}

bool PetriTabwidgt::canUndo ()
{
    return undostack->canUndo();
}

/* add new node to the current scene */
void PetriTabwidgt::nodeInserted(const QPointF &itemPos,
            const QString &id)
{
    if(mode == addPlaceMode)
    {
       undostack->push(new AddPlaceCommand(itemPos, id, scene));
      nodes_names << id;
    }else
     {
       undostack->push(new AddTransitionCommand(itemPos, id, scene));
       nodes_names << id;
     }
}

/* set Mode */
void PetriTabwidgt::setMode (int mode)
{
    scene->setMode(mode);
    this->mode = mode;
    setCursorShape ();
}

/* get mode */
int PetriTabwidgt::getMode () const
{
    return mode;
}

/* change the cursor shape */
void PetriTabwidgt::setCursorShape ()
{
    QCursor cursor;

    if ((mode == normalMode)||(mode == animationMode))
        cursor.setShape(Qt::ArrowCursor);
    else
        cursor.setShape(Qt::CrossCursor);

    view->setCursor(cursor);
}

/* is scene empty */
bool PetriTabwidgt::isNetEmpty()
{
  return scene->items().isEmpty();
}

bool PetriTabwidgt::isSaved ()
{
    //qDebug()<<(undostack->isClean()&&arcnoclicked);
    return undostack->isClean();//&&arcnoclicked;
}

void PetriTabwidgt::scaleView (qreal val)
{
    view->resetMatrix();
    view->scale(val/100, val/100);
    scene->update(scene->sceneRect());
    scaleVal = val;
}

qreal PetriTabwidgt::scaleValue ()
{
    return scaleVal;
}

/* call this function when the delete Toolbar button is trigered */
void PetriTabwidgt::removeItems ()
{
    scene->removeItems ();
}

const QString& PetriTabwidgt::getFilename ()
{
    return filename;
}

void PetriTabwidgt::setFilename (const QString& filenm)
{
    filename = filenm;
    QFileInfo fileInfo(filename);
    name = fileInfo.baseName();
}

void PetriTabwidgt::cleanUndostack()
{
  undostack->setClean();
      // here clean all invisible items
}

void PetriTabwidgt::setName(QString name)
{
    this->name = name;
}

void PetriTabwidgt::setId(QString id)
{
    this->id = id;
}

//PTNET_ATTR PTNtab::toXml() const
//{
//	PTNET_ATTR net;
//	net.id = id;
//	net.name = name;
//	PAGE_ATTR page;
//	page.id = "page0";
//	page.name = name;

//	foreach(QGraphicsItem * item, scene->items())
//	{
//		if(item->type() == Place::Type)
//		{
//			page.placeNodes << qgraphicsitem_cast<Place*>(item)->toXml();
//			//Place * place = qgraphicsitem_cast<Place*>(item);
//			//m_PlaceVector->push(place);
//			continue;
//		}
//		if(item->type() == Transition::Type)
//		{
//			page.transitionNodes << qgraphicsitem_cast<Transition*>(item)->toXml();
//			//Transition * trans = qgraphicsitem_cast<Transition*>(item);
//			//m_TransitionVector->push(trans);
//			continue;
//		}
//		if(item->type() == Arc::Type)
//		{
//			page.arcs << qgraphicsitem_cast<Arc*>(item)->toXml();
//			continue;
//		}
//	}

//	net.pages << page;
//	return net;
//}

void PetriTabwidgt::exportNet (const QString &imagefile)
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

bool PetriTabwidgt::checkNet ()
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
bool PetriTabwidgt::checkMarking()
{
  bool ok = false;

    foreach(QGraphicsItem * item ,scene->items())
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

  return ok;
}

//![1] check nodes connections!
bool PetriTabwidgt::checkNodesConnections()
{
    bool ok = true;

    foreach(QGraphicsItem * item , scene->items())
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

  return ok;
}

//![2] check nodes names uniqueness!
void PetriTabwidgt::checkNodesNames()
{

    QStringList plnames;
    QStringList trnames;

    foreach(QGraphicsItem * item , scene->items())
    {
        if(item->type() == Place::Type)
            plnames << (qgraphicsitem_cast<Place*>(item))->getName();

        if(item->type() == Transition::Type)
            trnames << (qgraphicsitem_cast<Transition*>(item))->getName();
    }

    if(plnames.removeDuplicates())
                        emit errorMessage ("  <span style=\"color:orange;\">Warning: Places names are not unique!</span>");


    if(trnames.removeDuplicates())
                        emit errorMessage ("  <span style=\"color:orange;\">Warning: Transitions names are not unique!</span>");

}

/* error message */
void PetriTabwidgt::showErrorMessage (const QString &title, const QString &errorMsg)
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

void PetriTabwidgt::connect_sigs_slots ()
{
    connect (scene, SIGNAL (nodeInserted(const QPointF&, const QString&)),
             this,  SLOT (nodeInserted(const QPointF&, const QString&)));

    connect (scene, SIGNAL (arcInserted(QGraphicsItem *, QGraphicsItem *,
                    const QPainterPath &, const QString &, PTNscene *, int)),
             undostack,  SLOT (arcInserted(QGraphicsItem *, QGraphicsItem *,
                    const QPainterPath &, const QString &, PTNscene *, int)));

    connect (scene, SIGNAL (itemMoved(QGraphicsItem*, QPointF)),
             undostack,  SLOT (itemMoved(QGraphicsItem*, QPointF)));

    connect (scene, SIGNAL (nodeRemoved(QGraphicsItem*, PTNscene*)),
             undostack, SLOT (nodeRemoved(QGraphicsItem*, PTNscene*)));

    connect (scene, SIGNAL (arcRemoved(QGraphicsItem*, PTNscene*)),
             undostack,  SLOT (arcRemoved(QGraphicsItem*, PTNscene*)));

    connect (scene, SIGNAL (nodesInserted(const QStringList&)),
             this,  SLOT (nodesInserted(const QStringList&)));

    connect (undostack, SIGNAL(canRedoChanged (bool)),
                  this, SIGNAL(canRedoChange (bool)));
    connect (undostack, SIGNAL(canUndoChanged (bool)),
                  this, SIGNAL(canUndoChange (bool)));
    connect (undostack, SIGNAL(cleanChanged (bool)),
                  this, SIGNAL(netChanged (bool)));

    connect (scene, SIGNAL(itemDoubleClicked (QGraphicsItem*)),
                  this, SLOT(itemDoubleClicked (QGraphicsItem*)));
}

//void PetriTabwidgt::itemDoubleClicked (QGraphicsItem* item)
//{
//    if(item->type() == Place::Type)
//    placeDoubleClicked (item);
//    if(item->type() == Transition::Type)
//    transitionDoubleClicked (item);
//    if(item->type() == Arc::Type)
//    arcDoubleClicked (item);

//    item = 0;
//}

//void PTNtab::placeDoubleClicked (QGraphicsItem* item)
//{
//	Place * place = qgraphicsitem_cast<Place*>(item);
//	QString old_name = place->getName();
//	placeEditDialog->inputLabel->setText(old_name);
//	placeEditDialog->inputTokens->setText(QString::number(place->getTokens()));
//    placeEditDialog->m_placeID->setText(place->getId());
//    /////placeEditDialog->m_placeID->setText(QString::number(nodes_names.indexOf(old_name)));
//	placeEditDialog->m_plainTextEdit->setText(place->getComment());
//	placeEditDialog->m_showComment->setChecked(place->getShow());
//	//set Color
//	placeEditDialog->m_graphicsEditTab->setBrushColor(place->getBrushColor());
//	placeEditDialog->m_graphicsEditTab->setPenColor(place->getPenColor());
//	placeEditDialog->m_graphicsEditTab->setBrushColorOrin(place->getBrushColor());
//	placeEditDialog->m_graphicsEditTab->setPenColorOrin(place->getPenColor());
//    //qDebug() <<old_name<<endl<<endl;
//    //for(int i=0;i<nodes_names.size();++i)qDebug()<<nodes_names[i]<<endl;
//	/*int c = place->getCapacity();
//	if(c == omega)
//	c=0;

//	placeEditDialog->inputCapacity->setValue(c);*/
//	placeEditDialog->exec();

//	if(placeEditDialog->result() == QDialog::Rejected)
//	return;

//	QString new_name = placeEditDialog->inputLabel->text();
//	place->setTokens(placeEditDialog->inputTokens->text().toDouble());
//	//place->setCapacity(placeEditDialog->inputCapacity->value());

//	//set color
//	place->setPenColor(placeEditDialog->m_graphicsEditTab->getPenColor());
//	place->setBrushColor(placeEditDialog->m_graphicsEditTab->getBrushColor());
//    place->setName(new_name);
//    int myflag=0;
//    foreach(QGraphicsItem * item1, scene->items())
//    {
//        if(item1->type() == Place::Type)
//        {
//            if(qgraphicsitem_cast<Place*>(item1)->getName()==new_name&&qgraphicsitem_cast<Place*>(item1)->getId()!=place->getId())
//            {
//                myflag=1;
//                qgraphicsitem_cast<Place*>(item1)->setTokens(place->getTokens());
//            }
//        }
//    }
//    if(myflag==1)
//    {
//        QMessageBox::about(this,tr("Same Name Exists"),tr("Nodes will be synchronized."));
//    }

//    /*
//	int index = nodes_names.indexOf (old_name);

//	if (new_name != old_name)
//	{
//        if(!nodes_names.contains(new_name))
//        {
//            nodes_names.replace (index, new_name);
//            place->setName(new_name);
//        }
//        else
//        {
//            QMessageBox::StandardButton button = QMessageBox::warning(this, "Not a unique name", "The place name <strong>"+new_name+"</strong> is already being used. Use it anyway?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
//            if(button == QMessageBox::Yes)
//            {
//                nodes_names.replace (index, new_name);
//                place->setName(new_name);
//                foreach(QGraphicsItem * item, scene->items())
//                {
//                    if(item->type() == Place::Type)
//                    {
//                        if(qgraphicsitem_cast<Place*>(item)->getName()==new_name&&qgraphicsitem_cast<Place*>(item)->getId()!=place->getId())
//                        {
//                            qgraphicsitem_cast<Place*>(item)->setTokens(place->getTokens());
//                        }
//                    }
//                }
//            }
//        }
//	}
//    else
//    {
//        foreach(QGraphicsItem * item, scene->items())
//        {
//            if(item->type() == Place::Type)
//            {
//                if(qgraphicsitem_cast<Place*>(item)->getName()==new_name&&qgraphicsitem_cast<Place*>(item)->getId()!=place->getId())
//                {
//                    qgraphicsitem_cast<Place*>(item)->setTokens(place->getTokens());
//                }
//            }
//        }
//    }
//    */
//	place->f_setComment(placeEditDialog->m_plainTextEdit->toPlainText());

//	if(!placeEditDialog->m_showComment->isChecked())
//	{
//		place->setShow(false);
//		place->setLabel_1();
//	}
//	else
//	{
//		place->setShow(true);
//		place->setLabel_2();
//	}

//	/*if(!placeEditDialog->chckBox->isChecked ())
//	place->setLabel_1();
//	else
//	place->setLabel_2();*/
//}

//void PTNtab::transitionDoubleClicked (QGraphicsItem* item)
//{
//	Transition * trans = qgraphicsitem_cast<Transition*>(item);
//	QString old_name = trans->getName();
//	QString old_function=trans->getFunction();
//	//QString old_MassAction=trans->getAction();
//	transEditDialog->inputLabel->setText(old_name);
//	transEditDialog->inputLabel1->setText(old_function);
//	//transEditDialog->inputLabel1->setText(old_MassAction);
//	transEditDialog->m_plainTextEdit->setText(trans->getComment());
//	transEditDialog->slider->setValue(trans->getRotation());
//    transEditDialog->m_transitionID->setText(trans->getId());
//    /////transEditDialog->m_transitionID->setText(QString::number(nodes_names.indexOf(old_name)));
//	transEditDialog->m_showComment->setChecked(trans->getShow());
//	//set color
//	transEditDialog->m_graphicsEditTab->setPenColor(trans->getPenColor());
//	transEditDialog->m_graphicsEditTab->setBrushColor(trans->getBrushColor());
//	transEditDialog->m_graphicsEditTab->setPenColorOrin(trans->getPenColor());
//	transEditDialog->m_graphicsEditTab->setBrushColorOrin(trans->getBrushColor());

//	transEditDialog->exec();

//	if(transEditDialog->result() == QDialog::Rejected)
//	return;

//	QString new_name = transEditDialog->inputLabel->text();
//	QString new_function=transEditDialog->inputLabel1->text();
//	//QString new_MassAction=transEditDialog->inputLabel2->text();
//	trans->setRotation (transEditDialog->slider->value());
//	trans->setFunction(new_function);
//	//trans->setAction(new_MassAction);

//	//set color
//	trans->setPenColor(transEditDialog->m_graphicsEditTab->getPenColor());
//	trans->setBrushColor(transEditDialog->m_graphicsEditTab->getBrushColor());
//    trans->setName(new_name);
//    int myflag=0;
//    foreach(QGraphicsItem * item1, scene->items())
//    {
//        if(item1->type() == Transition::Type)
//        {
//            if(qgraphicsitem_cast<Transition*>(item1)->getName()==new_name&&qgraphicsitem_cast<Place*>(item1)->getId()!=trans->getId())
//            {
//                myflag=1;
//                qgraphicsitem_cast<Transition*>(item1)->setFunction(trans->getFunction());
//            }
//        }
//    }
//    if(myflag==1)
//    {
//        QMessageBox::about(this,tr("Same Name Exists"),tr("Nodes will be synchronized."));
//    }
//    /*
//	int index = nodes_names.indexOf (old_name);

//	if (new_name != old_name)
//	{
//        if(!nodes_names.contains(new_name))
//        {
//            nodes_names.replace (index, new_name);
//            trans->setName(new_name);
//        }
//        else
//        {
//            QMessageBox::StandardButton button = QMessageBox::warning(this, "Not a unique name", "The transition name <strong>"+new_name+"</strong> is already being used. Use it anyway?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
//            if(button == QMessageBox::Yes)
//            {
//                nodes_names.replace (index, new_name);
//                trans->setName(new_name);
//            }
//            foreach(QGraphicsItem * item, scene->items())
//            {
//                if(item->type() == Transition::Type)
//                {
//                    if(qgraphicsitem_cast<Transition*>(item)->getName()==new_name&&qgraphicsitem_cast<Transition*>(item)->getId()!=trans->getId())
//                    {
//                        qgraphicsitem_cast<Transition*>(item)->setFunction(trans->getFunction());
//                    }
//                }
//            }
//        }
//	}
//    else
//    {
//        foreach(QGraphicsItem * item, scene->items())
//        {
//            if(item->type() == Transition::Type)
//            {
//                if(qgraphicsitem_cast<Transition*>(item)->getName()==new_name&&qgraphicsitem_cast<Transition*>(item)->getId()!=trans->getId())
//                {
//                    qgraphicsitem_cast<Transition*>(item)->setFunction(trans->getFunction());
//                }
//            }
//        }
//    }
//    */
//	//trans->f_setFunction(transEditDialog->m_functionEdit->toPlainText());
//	trans->f_setComment(transEditDialog->m_plainTextEdit->toPlainText());
//	if(!transEditDialog->m_showComment->isChecked())
//	{
//		trans->setShow(false);
//		trans->setLabel_1();
//	}
//	else
//	{
//		trans->setShow(true);
//		trans->setLabel_2();
//	}
//}

//void PTNtab::arcDoubleClicked (QGraphicsItem* item)
//{
//	undostack->arcClicked(item, scene);
//	//arcnoclicked=false;
//	Arc* arc = qgraphicsitem_cast<Arc*>(item);
//	/* set the SpinBox with the arc's current weight */
//	//arcEditDialog->inputWeight->setText(QString::number(arc->getWeight(),'f',10));
//	arcEditDialog->inputExpression->setText(arc->getExpression());
//	//arcEditDialog->m_experssionEdit->setText(arc->f_getExperssion());
//	//arcEditDialog->exec();
//	//FL
//	QVector<QString> l_vPlaces;



//	Transition *l_transition;
//	//@武杰，请在此通过arc找到其连接的transition，然后查找变迁链接的所有位置，将其名字写入l_vPlaces
//	if(arc->getSourceItem()->type()==Transition::Type)
//	{
//		l_transition=qgraphicsitem_cast<Transition*>(arc->getSourceItem());
//	}
//	else
//	{
//		l_transition=qgraphicsitem_cast<Transition*>(arc->getTargetItem());
//	}//find transition
//	foreach(QGraphicsItem * item, scene->items())
//	{
//		if(item->type()==Arc::Type)
//		{
//			Arc *l_arc=qgraphicsitem_cast<Arc*>(item);
//			if(l_arc->getSourceItem()->type()==Transition::Type)
//			{
//				if((qgraphicsitem_cast<Transition*>(l_arc->getSourceItem()))->getName()==l_transition->getName())
//				{
//					l_vPlaces.push_back(qgraphicsitem_cast<Place*>(l_arc->getTargetItem())->getName());
//				}
//			}
//			else
//			{
//				if((qgraphicsitem_cast<Transition*>(l_arc->getTargetItem()))->getName()==l_transition->getName())
//				{
//					l_vPlaces.push_back(qgraphicsitem_cast<Place*>(l_arc->getSourceItem())->getName());
//				}
//			}
//		}
//	}





//	//zyc
//	/*
//	l_vPlaces.push_back(QString("X1"));
//	l_vPlaces.push_back(QString("X2"));
//	l_vPlaces.push_back(QString("X3"));
//	l_vPlaces.push_back(QString("X4"));
//	l_vPlaces.push_back(QString("X5"));*/

//	arcEditDialog->setlPlaces(l_vPlaces);
//	//zyc
//    arcEditDialog->setFIS_a(arc->getFISStruct());//非&,m_FIS_a,
//    arcEditDialog->SetFISStruct(arc->GetFISStruct());//将获得的m_fis传输,&,m_pFIS
//	//arcEditDialog->setRuleSet(arc->GetRuleSet());

//	//set color
//	arcEditDialog->m_graphicsEditTab->setPenColor(arc->getPenColor());
//	arcEditDialog->m_graphicsEditTab->setBrushColor(arc->getBrushColor());
//	arcEditDialog->m_graphicsEditTab->setPenColorOrin(arc->getPenColor());
//	arcEditDialog->m_graphicsEditTab->setBrushColorOrin(arc->getBrushColor());

//	arcEditDialog->exec();
//	if (arcEditDialog->result() == QDialog::Rejected) return;


//	/* update the arc's weight */
//	if(arcEditDialog->result() == QDialog::Accepted)
//		//arc->setWeight(arcEditDialog->inputWeight->text().toDouble());
//	{
//		arc->setExpression(arcEditDialog->inputExpression->text());
//		//zyc
//		arc->createRuleSet();
////        if(arc->getFISStruct().m_sFISName.size())
////            arc->setExpression(QString::fromStdString(arc->getFISStruct().m_sFISName));
//	}
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
//	arc->setPenColor(arcEditDialog->m_graphicsEditTab->getPenColor());
//	arc->setBrushColor(arcEditDialog->m_graphicsEditTab->getBrushColor());
//}

PetriTabwidgt::~PetriTabwidgt ()
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
