#ifndef COMPONENTCONTROLLER_H
#define COMPONENTCONTROLLER_H
#include <QTreeWidget>
#include <QDebug>
#include <QDebug>
#include <QFile>
#include<QMap>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include"component.h"
#include "xmlparser.h"
class ComponentController
{
public:
    ComponentController();
    void componentTreeInitial(QTreeWidget *newTree);
    void addComponentTreeNode(QTreeWidget *newTree,QString component_name,QString component_path);
    void removeComponentTreeNode(QTreeWidgetItem *item);

    double getToken(QString filename,QString ID);
    Component * getComponent(QString filename);
    QList<PAGE_ATTR> getXMLpages(QString filename);
    PTNET_ATTR getPTnet(QString filename);

    //通过读取文件，读取type数量
    void ReadListFile(QString str="./list.txt");
    void WriteListFile(QString str="./list.txt");
    QString newtype;
    QMap<QString,int>type_list;
    QMap<QString,QString> itemsFile;
    QMap<QTreeWidgetItem *,QString> TreeWiditems;
    //2023.03.04
    QList<COMPONENT_ATTR> getCompAttrList(QString filename);


private slots:
    void on_treeWidget_Dev_itemChanged(QTreeWidgetItem *item); //通过右键菜单添加槽函数
    void slotCustomContextMenu(const QPoint &);
private:
    //2023.03.13
    QMap<QString,COMPONENT_ATTR>m_mCompName2Attr;

};



#endif // COMPONENTCONTROLLER_H
