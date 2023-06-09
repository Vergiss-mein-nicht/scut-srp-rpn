#include "addoperation.h"

AddOperation::AddOperation(const QString& componentName)
    :newComponentName(componentName)
{

}

void AddOperation::execOperation(ComponentList* componentList) const
{
    componentList->addNewComponent(newComponentName);
}

AddOperation *AddOperation::clone() const
{
    return new AddOperation(newComponentName);
}

QList<QString*> AddOperation::getArguments()
{
    return QList<QString*>{&newComponentName};
}

OPERATION_ATTR AddOperation::toXML() const
{
    OPERATION_ATTR operation;
    operation.type=type;
    operation.arguments.push_back(newComponentName);
    return operation;
}
