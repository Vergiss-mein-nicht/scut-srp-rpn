#include "deleteoperation.h"

DeleteOperation::DeleteOperation(const QString& componentId)
    :ComponentInstanceToDelete(componentId)
{

}

void DeleteOperation::execOperation(ComponentList* componentList) const
{
    componentList->deleteComponent(ComponentInstanceToDelete);
}

DeleteOperation *DeleteOperation::clone() const
{
    return new DeleteOperation(ComponentInstanceToDelete);
}

QList<QString*> DeleteOperation::getArguments()
{
    return QList<QString*>{&ComponentInstanceToDelete};
}

OPERATION_ATTR DeleteOperation::toXML() const
{
    OPERATION_ATTR operation;
    operation.type=type;
    operation.arguments.push_back(ComponentInstanceToDelete);
    return operation;
}
