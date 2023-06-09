#ifndef PLACEEDITDIALOG_H
#define PLACEEDITDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QTabWidget>
#include <QPlainTextEdit>
#include<QButtonGroup>

#include "graphicsedittab.h"
#include "graphicsedittab.h"

class PlaceEditDialog : public QDialog
{
    Q_OBJECT

 friend class PetriTabWidget;

 public:

    PlaceEditDialog (QWidget * parent = 0);
    ~PlaceEditDialog ();

    //void show (const QString& nm, int t, int c);
    //QString getName () const;
    //int getTokens() const;
    //int getCapacity() const;
    //bool showAttributes();
    void inputStateChanged(int state);
    void outputStateChanged(int state);
    void notStateChanged(int state);

 private:
     GraphicsEditTab *m_graphicsEditTab;
    QTabWidget  * m_tabwidget;//tabwidget,make dialog tidy
    QHBoxLayout * hlayout;
    QHBoxLayout * hlayout2;
    QVBoxLayout * vlayout;
    QVBoxLayout * vlayout2;
    QFormLayout * flayout;
    QLineEdit   * inputLabel;
    QLineEdit   * inputTokens;
    QSpinBox   * inputCapacity;
    QCheckBox   * chckBox;
    QLabel      * label;
    QPushButton * validateButton;
    QPushButton * cancelButton;
    QGroupBox   * groupBox;
    QLabel      * m_placeID;
    QTextEdit * m_plainTextEdit;
    QCheckBox   * m_showComment;
    QCheckBox   * m_inputPort;
    QCheckBox   * m_outputPort;
    QCheckBox   * m_notPort;
    QLabel      * m_commentLabel;
    QHBoxLayout * m_commentHlayout;
    QVBoxLayout * m_portlayout;

    QButtonGroup *ButtonGroup;
    //QGroupBox   * m_groupBox_1;
    //QFormLayout * m_flayout_1;
    //QSpinBox    * m_radiusSpinBox;
};

#endif // PLACEEDITDIALOG_H

