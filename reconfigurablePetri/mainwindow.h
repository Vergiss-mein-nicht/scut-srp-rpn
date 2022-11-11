#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QButtonGroup>
#include <QStatusBar>
#include <QSlider>
#include<QFont>
#include <QToolTip>

#include <QCloseEvent>

#include "tabwidget.h"
#include "dockwidget.h"


//enum{normalMode=0, animationMode=1, addPlaceMode=2, \
//            addTransMode=3,drawArcMode=4, addToken=5, subToken=6};
/*
 * 主窗口
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
     MainWindow(QWidget *parent = 0);
    ~MainWindow();

     // 当窗口改变时及时更新窗口按钮是否可按
     void updateWidgets (int mode);

    //QT5的槽函数可以写到public/全局
    void buttonGroupClicked (int id);//这个函数处理主页面上所有按钮点击，如果涉及模拟，似乎需要一大堆处理
    void tabChanged (int index);
    void sliderValueChanged (int value);
    void open ();
    void about();
    void openRuleLibrary();

private:

    //各个菜单栏及其动作
    QMenu   * fileMenu;
    QAction * newMenuAct;
    QAction * openMenuAct;
    QAction * saveMenuAct;
    QAction * saveAsMenuAct;
    QAction * exportMenuAct;
    QAction * printMenuAct;
    QAction * quitMenuAct;

    QMenu   * editMenu;
    QAction * undoMenuAct;
    QAction * redoMenuAct;
    QAction * removeMenuAct;

    QMenu   * helpMenu;
    QAction * aboutMenuAct;

    //同等于工具栏的圆框线
    QMenu   * m_modelingMenu;
    QAction * m_placeMenuAct;
    QAction * m_transitionMenuAct;
    QAction * m_arcMenuAct;

    //组件库
    QMenu   * componentMenu;
    QAction * openComponentAction;
    QAction * saveComponentAction;

    //规则库
    QMenu* ruleMenu;
    QAction* openRuleLibraryAction;


    //状态栏
    QStatusBar * statusBar;

    //工具栏及按钮组
    QToolBar * toolBar;
    QButtonGroup * buttonGroup;

    QToolButton  * newToolButton;
    QToolButton  * openToolButton;
    QToolButton  * saveToolButton;
    QToolButton * saveAsToolButton;
    QToolButton * printToolButton;

    QToolButton  * undoToolButton;
    QToolButton  * redoToolButton;
    QToolButton  * removeToolButton;

    QToolButton  * cursorToolButton;
    QToolButton  * addPlaceToolButton;
    QToolButton  * addTransToolButton;
    QToolButton  * drawArcToolButton;

    //仿真按钮
    QToolButton  * animateToolButton;

    //选项卡式的窗口控件
    TabWidget  * tabWidget;

    //可拖拉界面的部件
    DockWidget * buttomDock;

    //用于缩放的滑动条
    QSlider  * slider;

    //初始化菜单，工具，状态栏，拖拉界面
    void createMenuBar ();
    void createToolBar ();
    void createStatusBar ();
    void createDocks ();

};

#endif // MAINWINDOW_H
