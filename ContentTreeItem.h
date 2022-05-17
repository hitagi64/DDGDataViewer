#ifndef CONTENTTREEITEM_H
#define CONTENTTREEITEM_H

#include <QTreeWidgetItem>
#include "DDG/DDGContent.h"

class ContentTreeItem : public QTreeWidgetItem
{
public:
    ContentTreeItem(DDGContent *data);

    DDGContent *content;
};

#endif // CONTENTTREEITEM_H
