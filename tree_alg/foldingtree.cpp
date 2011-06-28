#include "foldingtree.h"
#include "qdebug.h"

int INF = 1000;

FoldingNode::FoldingNode(int pos, int typ, FoldingNode *par) :
    position(pos),
    parent(par),
    type(typ)
{
}

FoldingNode::~FoldingNode()
{
  m_startChildren.clear();
  m_endChildren.clear();
}

FoldingNode* FoldingNode::matchingNode()
{
  if (m_endChildren.size() > 0)
    return m_endChildren[0];
  return NULL;
}

void FoldingNode::addChild(FoldingNode *node)
{
  if (node->type > 0)
    add(node,m_startChildren);
  else
    add(node,m_endChildren);
}

void FoldingNode::removeChild(FoldingNode *node)
{
  if (node->type > 0)
    remove(node,m_startChildren);
  else
    remove(node,m_endChildren);
}

// All the children must be kept sorted (using their position)
// This method inserts the new child in a children list
void FoldingNode::add(FoldingNode *node, QVector<FoldingNode*> &m_childred)
{
  int i;
  for (i = 0 ; i < m_childred.size() ; ++i) {
    if (m_childred[i]->position > node->position)
      break;
  }
  m_childred.insert(i,node);
}

void FoldingNode::remove(FoldingNode *node, QVector<FoldingNode *> &m_childred)
{
  int i;
  for (i = 0 ; i < m_childred.size() ; ++i) {
    if (m_childred[i]->position == node->position)
      break;
  }
  m_childred.remove(i);
}

// Merges two QVectors of children.
// The result is placed in list1. list2 it is not modified.
void FoldingNode::mergeChildren(QVector <FoldingNode*> &list1, QVector <FoldingNode*> &list2)
{
  QVector <FoldingNode*> mergedList;
  int index1 = 0, index2 = 0;
  while (index1 < list1.size() && index2 < list2.size()) {
    if (list1[index1]->position < list2[index2]->position) {
      mergedList.append(list1[index1]);
      ++index1;
    }
    else {
      mergedList.append(list2[index2]);
      ++index2;
    }
  }

  for (; index1 < list1.size() ; ++index1)
    mergedList.append(list1[index1]);
  for (; index2 < list2.size() ; ++index2)
    mergedList.append(list2[index2]);

  list1 = mergedList;
}

// This method recalirates the folding tree after a modification occurs.
void FoldingNode::updateSelf()
{
  if (m_endChildren.size() > 0) {                       // this node doesn't have shortage
    for (int i = 0 ; i < m_startChildren.size() ; ++i) {
      FoldingNode* child = m_startChildren[i];
      if (child->position > matchingNode()->position) { // if child is lower than parent's pair
        remove(child, m_startChildren);                 // this node is not it's child anymore
        i --;                                           // go one step behind (because we removed 1 element)
        parent->addChild(child);                        // the node selected becomes it's broter (same parent)
      }
    }
  }
  else {                                                // this node has a shortage
    for (int i = 0 ; i < parent->m_startChildren.size() ; ++i) {
      FoldingNode* child = parent->m_startChildren[i];  // it's brother is selected
      if (child->position > position) {                 // and if this brother is above the current node, then
        remove(child, parent->m_startChildren);         // this node is not it's brother anymore, but it's child
        i --;                                           // go one step behind (because we removed 1 element)
        addChild(child);                                // the node selected becomes it's child
      }
    }
  }

  m_endChildren.clear();                                // we rebuild end_children list
  foreach (FoldingNode *child, m_startChildren) {
    if (child->shortage > 0) {                          // the only child this node has
      shortage = child->shortage + 1;
    }
    else {
      QVector <FoldingNode*> tempList (child->m_endChildren);
      tempList.pop_front();                             // we take the excess of it's children
      mergeChildren(m_endChildren,tempList);            // and merge to it's endChildren list
    }
  }
  setParent();
}

// This method helps recalibrating the tree
void FoldingNode::updateParent(QVector <FoldingNode *> newExcess, int newShortage)
{
  mergeChildren(m_endChildren,newExcess);               // parent's endChildren list is updated
  if (newShortage > 0)
    shortage = newShortage + 1;                         // parent's shortage is updated
  if (type)                                             // if this node is not the root node
    updateSelf();                                       // then recalibration continues
}

// This method will ensure that all children have their parent set ok
void FoldingNode::setParent()
{
  foreach(FoldingNode *child, m_startChildren) {
    child->parent = this;
  }
  foreach(FoldingNode *child, m_endChildren) {
    child->parent = this;
  }
}

QString FoldingNode::toString(int level)
{
  QString string = "\n";
  for (int i = 0 ; i < level-1 ; ++ i)
    string.append(QString("   "));

  //qDebug()<<QString("list size = %1").arg(m_startChildren.size());
  if (type > 0)
  {
    string.append(QString("{ (position=%1, parentNo=%2)").arg(position).arg(parent->position));
    //qDebug()<<QString("!!!!!!!!!parent = %1").arg(parent->position);
  }
  else if (type < 0)
  {
    string.append(QString("} (position=%1, parentNo=%2)").arg(position).arg(parent->position));
  }
  //qDebug()<<QString("\n------\nnew string = %1\n--------\n").arg(string);
  int i1,i2;
  for (i1 = 0, i2 = 0 ; i1 < m_startChildren.size() && i2 < m_endChildren.size() ;) {
    if (m_startChildren[i1]->position < m_endChildren[i2]->position) {
      string.append(m_startChildren[i1]->toString(level + 1));
      ++i1;
    }
    else {
      string.append(m_endChildren[i2]->toString(level));
      ++i2;
    }
  }

  for (; i1 < m_startChildren.size() && i1 < m_startChildren.size() ; ++ i1) {
    string.append(m_startChildren[i1]->toString(level + 1));
  }
  for (; i2 < m_endChildren.size() && i2 < m_endChildren.size() ; ++ i2) {
    string.append(m_endChildren[i2]->toString(level + 1));
  }
  return string;
}

FoldingTree::FoldingTree()
{
  root = new FoldingNode(0,0,NULL);
  nodeMap.clear();
  nodeMap.insert(0,root);
}

FoldingTree::~FoldingTree()
{
}

// Search for the parent of the new node that will be created at position startingPos + 1
FoldingNode* FoldingTree::findParent(int startingPos) // Node placed at position startingPos is the first candidate
{
  int i;
  for (i = startingPos ; i > 0 ; -- i) {
    if (nodeMap[i]->type > 0)                         // The parent node has to be a "start node"
      return nodeMap[i];
  }
  return root;                                        // The root node is the default parent
}

void FoldingTree::insertStartNode(int pos)
{
  // step 0 - set newNode's parameters
  FoldingNode *parentNode = findParent(pos);                                  // find the parent of the new node
  FoldingNode *newNode = new
                FoldingNode(parentNode->position + 1,1,parentNode);       // create the new node
  nodeMap.insert(newNode->position,newNode);                              // insert it in the map
  increasePosition(newNode->position + 1);                                // update the others nodes position (+ 1)
  parentNode->addChild(newNode);                                              // add the node to it's parent

  // step 1 - devide parent's startChildrenList
  QVector <FoldingNode*> tempList(parentNode->m_startChildren);
  sublist(parentNode->m_startChildren,tempList,0,newNode->position);
  sublist(newNode->m_startChildren,tempList,newNode->position,INF);

  // step 2 - devide parent's endChildrenList (or inherit shortage)
  if (parentNode->shortage > 0) {
    newNode->shortage = parentNode->shortage;
  }
  else {
    tempList = parentNode->m_endChildren;
    sublist(parentNode->m_endChildren,tempList,0,newNode->position);
    sublist(newNode->m_endChildren,tempList,newNode->position,INF);
  }

  // step 3 - set the new parent for all the children
  newNode->setParent();
  parentNode->setParent();

  // step 4 - call updateParent te recalibrate tree
  tempList = newNode->m_startChildren;
  tempList.pop_front();
  newNode->updateParent(tempList,newNode->shortage);

}

void FoldingTree::insertEndNode(int pos)
{
  // step 0 - set newNode's parameters
  FoldingNode *parentNode = findParent(pos);                                  // find the parent of the new node
  FoldingNode *newNode = new
               FoldingNode(parentNode->position + 1,-1,parentNode);       // create the new node
  nodeMap.insert(newNode->position,newNode);                              // insert it in the map
  increasePosition(newNode->position + 1);                                // update the others nodes position (+ 1)
  parentNode->addChild(newNode);                                              // add the node to it's parent

  // step 1 - call updateSelf() for parent node to recalibrate tree
  parentNode->updateSelf();
}

void FoldingTree::deleteStartNode(int pos)
{
  // step 0 - find the node that will be deleted
  if (pos >= nodeMap.size())
    return;
  if (pos <= 0)
    return;
  FoldingNode *deletedNode = findNodeAt(pos);

  // step 1 - all its startChildren are inherited by its parent
  deletedNode->parent->mergeChildren(deletedNode->parent->m_startChildren,deletedNode->m_startChildren);

  // step 2 - this node is removed from the tree
  deletedNode->parent->removeChild(deletedNode);
  decreasePosition(deletedNode->position);

  // step 3 - parent inherits shortage and endChildren too
  deletedNode->parent->updateParent(deletedNode->m_endChildren,deletedNode->shortage - 1);

  // step 4 - node is deleted
  delete deletedNode;
}

void FoldingTree::deleteEndNode(int pos)
{
  // step 0 - find the node that will be deleted
  if (pos >= nodeMap.size())
    return;
  if (pos <= 0)
    return;
  FoldingNode *deletedNode = findNodeAt(pos);

  // step 1 - this node is removed from the tree
  deletedNode->parent->removeChild(deletedNode);
  decreasePosition(deletedNode->position);

  // step 2 - recalibrate folding tree starting from parent
  deletedNode->parent->updateSelf();

  // step 3 - node is deleted
  delete deletedNode;
}

void FoldingTree::deleteNode(int pos)
{
  // step 0 - find the node that will be deleted
  if (pos >= nodeMap.size())
    return;
  if (pos <= 0)
    return;
  FoldingNode *deletedNode = findNodeAt(pos);
  if (deletedNode->type > 0) {
    deleteStartNode(pos);
  }
  else {
    deleteEndNode(pos);
  }
}

// Increase position for each node placed after the inserted node
void FoldingTree::increasePosition(int startingPos)
{
  for (int i = startingPos ; i < nodeMap.size(); ++i) {
    nodeMap[i]->position ++;
  }
}

// Decrease position for each node placed after the inserted node
void FoldingTree::decreasePosition(int startingPos)
{
  for (int i = startingPos ; i < nodeMap.size(); ++i) {
    nodeMap[i]->position --;
  }
}

FoldingNode* FoldingTree::findNodeAt(int position) {
  foreach (FoldingNode *node,nodeMap) {
    if (node->position == position)
      return node;
  }
  return NULL;
}

// puts in dest Vector a sublist of source Vector
void FoldingTree::sublist(QVector<FoldingNode *> &dest, QVector<FoldingNode *> source, int begin, int end)
{
  foreach (FoldingNode *node, source) {
    if (node->position >= begin) {
      dest.append(node);
    }
    if (node->position >= end)
      break;
  }
}

QString FoldingTree::toString()
{
  /*qDebug()<<QString("\n********\nThe node's map:\n");
  for (int i = 0 ; i < nodeMap.size() ; i++) {
    qDebug()<<QString("(%1 ; %2)").arg(nodeMap[i]->position).arg(nodeMap[i]->type);
  }*/
  return "(0)" + root->toString();
}
