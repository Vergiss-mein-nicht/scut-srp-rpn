#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H
#include <QThread>
#include "plot.h"
#include"event.h"
#include"mineventheap.h"
#include"rulemanager.h"
class SimulationController: public QThread
{
    Q_OBJECT

public:
    /**
     * @brief SimulationController 构造函数
     * @param m_gui
     * @param start
     * @param end
     * @param step
     */
    SimulationController(ComponentList*,Plot *m_gui,double start,double end,double step);
    /**
     * @brief run 重写虚函数，在线程内进行仿真操作
     * 主要行为包括：组件优先级排序，构造最小事件堆，仿真计算与调度，数据可视化
     */
    void  run();
    /**
     * @brief sort 对组件进行优先级排序，主要使用Trajan算法和拓扑排序
     * @return bool true为没有代数环
     */
    bool sort();
    /**
     * @brief tarjan 使用tarjan方法求解组件拓扑图中的强连通分量
     */
    void tarjan(int);
signals:
    /**
     * @brief updateui 更新仿真进度条
     * @param x        进度
     * @param y        仿真运行时间
     */
    void updateui(double,QString);
    /**
     * @brief addgraph 在图中增加一条曲线
     * @param string   曲线名字
     */
    void addgraph(string);
    /**
     * @brief adddata 在曲线上增加数据
     * @param i       曲线编号
     * @param t1      x坐标集合
     * @param t2      y坐标集合
     */
    void adddata(unsigned,QVector<double>,QVector<double>);
    /**
     * @brief slotUpdateUi 更新仿真进度条
     * @param x            进度
     * @param y            仿真运行时间
     */
public slots:
    void slotUpdateUi(double,QString);
    /**
     * @brief slotAddGraph 在图中增加一条曲线
     * @param s            曲线名字
     */
    void slotAddGraph(string);
    /**
     * @brief slotAddData      向曲线增加数据
     * @param i                曲线编号
     * @param t1               x坐标集合
     * @param t2               y坐标集合
     */
    void slotAddData(unsigned,QVector<double>,QVector<double>);
private:
    Plot *m_gui;               /** 主线程对象指针*/
    ComponentList*m_compList;  /** 组件管理*/
    RuleManager*m_ruleManager; /** 规则管理*/
    double m_start;            /** 仿真开始时间*/
    double m_end;              /** 仿真结束时间*/
    double m_step;             /** 规则判断间隔*/
};

#endif // SIMULATIONCONTROLLER_H
