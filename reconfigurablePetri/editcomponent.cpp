#include "editcomponent.h"
#include "ui_editcomponent.h"
#include"QLineEdit"
#include"QDebug"
editComponent::editComponent(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editComponent)
{
    ui->setupUi(this);

}
void editComponent::on_buttonBox_accepted()
{
    QString componentName=ui->ComponentName->text();
    QString Step=ui->ComponentStep->text();
    double componentStep=Step.toDouble();
    emit editComponentInfo(componentName,componentStep);

}

editComponent::~editComponent()
{
    delete ui;
}


