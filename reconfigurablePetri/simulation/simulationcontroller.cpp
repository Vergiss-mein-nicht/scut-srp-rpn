#include "simulationcontroller.h"
//允许的组件最大数量
#define COMP_NUM_MAX 100

QString findCompId(QString const s)
{
    QStringList l_stringList=s.split("&");
    return l_stringList[0]+"&"+l_stringList[1];
}

SimulationController::SimulationController(ComponentList*list,Plot*gui,double start,double end,double step)
{

    qRegisterMetaType<QVector<double>>("QVector<double>");
    qRegisterMetaType<string>("string");
    m_compList=new ComponentList(list);
    m_gui=gui;
    m_step = step;
    m_start = start;
    m_end = end;
    connect(this, &SimulationController::updateui, this, &SimulationController::slotUpdateUi);
    connect(this, &SimulationController::addgraph,this,&SimulationController::slotAddGraph);
    connect(this, &SimulationController::adddata,this,&SimulationController::slotAddData);
}

void SimulationController::run()
{
//    bool suc = true;
//    qDebug()<<"simulation is in thread: "<<QThread::currentThreadId();

//    /***
//     * *测试获取的com_list信息
//     */
//    if(m_compList!=nullptr)
//        qDebug()<<"get not null ptr : com_list";
//    Component*l_Component=nullptr;
//    QList<PLACE_ATTR> l_placeList;
//    QVector<Component*>l_vComponent = m_compList->getComponentList();
//    for( int i = 0; i<l_vComponent.size();i++)
//    {
//        l_Component=l_vComponent[i];
//        ///目前无法获得步长数据！！！
//        l_vComponent[i]->setStep(0.5);
//        qDebug()<<"this is the "<<i<<"component :"<<l_vComponent[i]->getID()<<", and step is "<<l_vComponent[i]->getStep();
//        l_placeList=l_vComponent[i]->getPlace_ATTRList();
//        for( int i = 0; i<l_placeList.size();i++)
//        {
//             qDebug()<<"place name: "<<l_placeList[i].name<<"and token : "<<l_placeList[i].initmark;
//        }
//        //测试：速率函数的生成
//        l_vComponent[i]->makeFunction();
//        //测试：单步仿真
//        l_vComponent[i]->tick(1,true);
//        //获取更新后的数据并打印
//        l_vComponent = m_compList->getComponentList();
//        l_placeList=l_vComponent[i]->getPlace_ATTRList();
//        for( int i = 0; i<l_placeList.size();i++)
//        {
//             qDebug()<<"place name: "<<l_placeList[i].name<<"and token after one step: "<<l_placeList[i].initmark;
//        }
//    }
//    //测试获取的connector信息
//    QList<CONNECTOR_ATTR>l_connectorList=m_compList->getConnectorAttrList();
//    for( int i = 0; i < l_connectorList.size();i++)
//    {
//        qDebug()<<"connector "<<i<<" is from "<<l_connectorList[i].source<<" to "<<l_connectorList[i].source;
//    }
//    //测试数据可视化
//    datavv[0].clear();
//    double l_x=0,l_y1=0,l_y2=0;
//    string l_s1 = "test1";
//    string l_s2 = "test2";
//    emit addgraph(l_s1);
//    emit addgraph(l_s2);
//    for(int i = 0; i<10;i++)
//    {
//        l_x+=1;
//        l_y1+=0.5;
//        l_y2+=1;

//        QVector<double>temp1,temp2,temp3,temp;
//        temp1.push_back(l_x);
//        temp2.push_back(l_y1);
//        temp3.push_back(l_y2);
//        temp.push_back(l_y1);
//        temp.push_back(l_y2);
//        datavv[0].push_back(temp);
//        emit adddata(0,temp1,temp2);
//        emit adddata(1,temp1,temp3);
//    }

    //1.对组件进行优先级排序
    if(!sort())
        qDebug()<<"components have been sorted";
    //2.生成事件树
    //3.规则库初始化
    //4.进行仿真
    //5.仿真调度
    //6.连接器数据处理
    //7.数据导出

}

void SimulationController::slotAddGraph(string s)
{
    m_gui->addGraph(s);
}
void SimulationController::slotAddData(unsigned i,QVector<double>t1,QVector<double>t2)
{
    m_gui->addData(i,t1,t2);
}
void SimulationController::slotUpdateUi(double x,QString y)
{
    m_gui->updateUi(x,y);
}

//------用于tarjan算法-------
static int DFN[COMP_NUM_MAX];
static int low[COMP_NUM_MAX];
static int counts=1;
static int stack[COMP_NUM_MAX];
static int top=1;
static int flag[COMP_NUM_MAX];
static int number=0;
static int j;
static int matrix[COMP_NUM_MAX][COMP_NUM_MAX];
static int length;
//--------------------------

void SimulationController::tarjan(int u)
{
    DFN[u]=low[u]=counts++;
    stack[++top]=u;
    flag[u]=1;

    for(int v=0;v<length;v++)
    {
        if(matrix[u][v])
        {
            if(!DFN[v])
            {
                tarjan(v);
                if(low[v]<low[u])
                    low[u]=low[v];
            }
            else{
                if(DFN[v]<low[u]&&flag[v])
                    low[u]=DFN[v];
            }
        }
    }

    if(DFN[u]==low[u])
    {
        number++;
        do{
            j=stack[top--];
            qDebug()<<j<<" ";
            flag[j]=0;
        }while(j!=u);
    }
}

bool SimulationController::sort()
{


    QList<CONNECTOR_ATTR> l_ConnectorAttrList=m_compList->getConnectorAttrList();
    QVector<Component*> l_vComponent=m_compList->getComponentList();
    QMap<QString,int>l_mCompId2Order;
    length=l_vComponent.size();
    if(length>COMP_NUM_MAX)
    {
        qDebug()<<"warning : the number of component is out of range !";
        return false;
    }
    //数组初始化
    memset(DFN,0,sizeof (DFN));
    memset(low,0,sizeof (low));
    memset(flag,0,sizeof (flag));
    for(int i =0;i<length;i++)
    {
        for(int j =0;j<length;j++)
            matrix[i][j]=0;
    }
    //用数字标记数组
    for(int i=0;i<length;i++)
    {
        l_mCompId2Order[l_vComponent[i]->getID()]=i;
    }
    QString source,target;
    //将有向图转化为矩阵
    for(int i=0;i<l_ConnectorAttrList.size();i++)
    {
        source=findCompId(l_ConnectorAttrList[i].source);
        target=findCompId(l_ConnectorAttrList[i].target);
        if(!l_mCompId2Order.contains(source))
        {
            qDebug()<<l_ConnectorAttrList[i].id<<" has invalid source"<<l_ConnectorAttrList[i].source;
        }
        if(!l_mCompId2Order.contains(target))
        {
            qDebug()<<l_ConnectorAttrList[i].id<<" has invalid source"<<l_ConnectorAttrList[i].target;
        }
        qDebug()<<"the "<<i<<" connector is from "<<source<<" to "<<target;
        matrix[l_mCompId2Order[source]][l_mCompId2Order[target]]=1;
    }
    tarjan(0);
    return true;
}

