/******************************************************************************
* \filename    RangeList.cpp
* \author      Jalin Brown
* \email       jalinbrownworks@gmail.com

Brief Description:
  1) convert previous rangelist into C++ class RangeList
  2) implement Proxy design pattern to support index operator
  3) implement scalar multiplication and vector addition

******************************************************************************/

/* cases:
 * NON-zero value - insert
 * insert value is NEXT to a range of same values      => just MODIFY the range (extend)
 * insert value is NEXT to a range of diff values      => insert as a NEW NODE with a range of size 1
 * insert value is NOT next to any range               => insert as a NEW NODE with a range of size 1
 * insert value is in the MIDDLE of the existing range =>
 *              case 1) value of the range is the same => do nothing
 *              case 2) value is different             => SPLIT range into 3 ranges (before, position, after)
 *                                                        the before and/or after ranges may be empty
 * SPECIAL CASE:
 * after expanding (cases when insert position is NEXT to an existing range, 2 ranges may need to be merged
 * if there are 2 ranges [a,b) and [b,c) they should be merged into a single [a,c)
 *
 * ZERO value (delete)
 * delete a position that is FIRST OR LAST in a range => just MODIFY the range
 * delete a position that is the MIDDLE of a range    => SPLIT range in two (before and after)
 * delete a position that us NOT in a range           => do nothing
 */

 /*-----------------------------------------------------------------------------
  Include Files:
 -----------------------------------------------------------------------------*/

#include "RangeList.h"
#include <cstdlib>    /* new/delete */
#include <cstddef>    /* NULL */
#include <cstdio>     /* printf */
#include <algorithm>  /* std::swap */
#include <iostream>

#define xDEBUG

 //-----------------------------------------------------------------------------
 // Private Constants:
 //-----------------------------------------------------------------------------

 //-----------------------------------------------------------------------------
 // Private Variables:
 //-----------------------------------------------------------------------------

 //-----------------------------------------------------------------------------
 // Private Structures:
 //-----------------------------------------------------------------------------

 /*
  * @brief RangeNode CTOR
  *
  * Create and return a dynamically allocated node with the requested
  * initilizing values from the parameter.
  *
  * @param b - begin
  * @param e - end
  * @param l - value
  *
  * @return Rangenode
  */
RangeList::RangeNode::RangeNode(int b, int e, int v) :
  begin(b),
  end(e),
  value(v),
  next(NULL)
{
}

//-----------------------------------------------------------------------------
// Private Function Declarations:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Structures:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Variables:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Functions:
//-----------------------------------------------------------------------------

/*
 * @brief RANGELIST -  DEFAULT CTOR
 *
 * Create a RangList, with head pointer set to Null
 *
 * @param
 *
 * @return a single ranglist
 */
RangeList::RangeList() : pHead(NULL)
{
}

//-----------------------------------------------------------------------------

/*
 * @brief RANGELIST -  COPY CTOR
 *
 * Create a RangList copy of another rangelist, with the new head pointer set to Null
 *
 * @param - other
 *
 * @return a single ranglist
 */
RangeList::RangeList(const RangeList& other) : pHead(NULL)
{
  /* Iterate through the other rangelist and copy each node */
  RangeNode* otherNode = other.pHead;
  RangeNode* tail = NULL;

  while (otherNode != NULL)
  {
    /* Create the copy nodes to list for the other range's nodes */
    RangeNode* newNode = new RangeNode(otherNode->begin, otherNode->end, otherNode->value);

    // If it's the first node, set it as the head of the new list
    if (tail == NULL)
    {
      pHead = newNode;
    }
    else
    {
      // Link the new node to the previous node
      tail->next = newNode;
    }

    // Update the tail to the new node
    tail = newNode;

    otherNode = otherNode->next; // Move to the next node in the other list
  }

  // Make sure to set the tail of the new list to NULL
  if (tail != NULL)
  {
    tail->next = NULL;
  }
}

//-----------------------------------------------------------------------------

/*
 * @brief RANGELIST -  ASSIGNMENT CTOR
 *
 * Create a RangList DEEP copy of another rangelist.
 * ***The new head pointer will point to the same location
 * in memory as the original.***
 *
 * @param - other
 *
 * @return a single ranglist
 */
RangeList& RangeList::operator=(const RangeList& other) {
  if (this != &other)
  {
    RangeList temp(other);

    // Swap the contents of 'this' and 'temp'
    std::swap(pHead, temp.pHead);
  }

  return *this;
}

//-----------------------------------------------------------------------------

/*
 * @brief RANGELIST - DTOR
 *
 * Destroy a RangList using the delete_ranges method.
 * This will iterate through the list and delete all RangeNodes
 * and the RangeList itselg properly.
 *
 * @param
 *
 * @return
 */
RangeList::~RangeList()
{
  delete_ranges(pHead);
}

//-----------------------------------------------------------------------------

/*
 * @brief: get
 *
 * Get and return a the value of a RangeNode at the requested position.
 *
 * @param:  position - The position of the RangeList requested to get value of.
 *
 * @return: The value of a RangeNode at the requested position.
 */
int RangeList::get(int position) const
{
  RangeNode* current = pHead;

  /* Traverse list until at target location */
  while ((current) && (current->begin != position))
  {
    current = current->next;
  }

  /* Once at target location */
  if ((current != NULL) && current->begin == position)
  {
    return current->value;
  }

  return 0;
}

//-----------------------------------------------------------------------------

/*
 * @breif   insert_value
 *
 * Create and insert a single RangeNode within the RangeList correctly.
 *
 * @param:  position - Position within the RangeList the RangeNode shoulg go
 * @param:  value    - The integer value of the created RangeNode
 *
 * @return  1 The RangeNode was created and inserted successfully
 *          -1 A memory error has occured during creation of the RangeNode
 */
int RangeList::insert_value(int position, int value)
{
  /* Step 1: Create and initialize a new node to be inserted                 */
  RangeNode* newNode = new RangeNode(position, position + 1, value);

  /* Step 2: Check if the given list is empty.
     If so, insert node at the head of the list                              */
  if (!pHead || position < pHead->begin)
  {
    newNode->next = pHead;
    pHead = newNode;

    mergeNodes(&pHead);

    return 1;
  }

  /* Step 3: Create pointers for the nodes that will
     help seek and insert the node                                           */
  RangeNode* indexNode = pHead;
  RangeNode* prevNode = NULL;
  RangeNode* tempNode = NULL;

  /* Step 4: Traverse until the end of the list is reached,
     or a condition is met along the way                                     */
  while (indexNode != NULL)
  {
    /*S4a:  The new node should be placed before the current node            */
    if (newNode->end <= indexNode->begin)
    {
      // Link the new node's next to the current node if it exists
      newNode->next = indexNode;
      if (prevNode != NULL)
      {
        prevNode->next = newNode;
      }
      // Update the head pointer of the list if the new node is now the head
      else
      {
        pHead = newNode;
      }

      // Remove nodes with zero values
      if (newNode->value == 0)
      {
        if (prevNode != NULL)
        {
          prevNode->next = newNode->next;
        }
        else
        {
          pHead = newNode->next;
        }
        tempNode = newNode->next;
        delete newNode;
        newNode = tempNode; // Move to the next node in the list
      }

      // Merge consecutive nodes with the same value
      mergeNodes(&pHead);

      return 1;
    }
    /*S4b: The new node should be placed after the current node but not here */
    else if (newNode->begin >= indexNode->end)
    {
      prevNode = indexNode;
      indexNode = indexNode->next;
    }
    else
    {
      /*S4c: There is an overlap between the created series of numbers
         and the current index's series of numbers. Merge or split nodes
         accordingly.                                                        */
      if (newNode->begin <= indexNode->begin)
      {
        // The new node starts before or at the same position as the indexNode
        if (newNode->end >= indexNode->end)
        {
          // The new node completely overlaps the indexNode. 
          // Update the indexNode with the new values. 
          indexNode->begin = newNode->begin;
          indexNode->end = newNode->end;
          indexNode->value = value;

          if (indexNode->value == 0)
          {
            if (prevNode != NULL)
            {
              prevNode->next = indexNode->next;
            }
            else
            {
              pHead = indexNode->next;
            }
            tempNode = indexNode->next;
            delete indexNode;
            indexNode = tempNode;
          }
        }
        /*S4d: The new node partially overlaps the indexNode from the beginning. */
        else if (newNode->end <= indexNode->end)
        {
          /* The new node does not overlap the head of the list */
          if (indexNode != pHead)
          {

            // Split the indexNode and correct the begin, end, next values
            RangeNode* splitNode = new RangeNode(newNode->end, indexNode->end, indexNode->value);
            splitNode->next = indexNode->next;

            indexNode->end = newNode->begin;
            indexNode->next = newNode;
            newNode->next = splitNode;

            if (splitNode->value == 0)
            {
              if (prevNode != NULL)
              {
                prevNode->next = splitNode->next;
              }
              else
              {
                pHead = splitNode->next;
              }
              tempNode = splitNode->next;
              delete splitNode;
              splitNode = tempNode;
            }
          }
          /* The new node partially overlaps at the head of the list */
          else
          {
            // Re-set the pointer to the head of the list as the new node
            newNode->next = pHead;
            pHead = newNode;

            // Link the new node's end to the head nodes beginning
            indexNode->begin = newNode->end;
          }
        }
      }
      else
      {
        /* S4e: The new node completely overlaps the indexNode.              */
        if (newNode->end > indexNode->end)
        {
          // Update the indexNode with the new values.
          indexNode->end = newNode->end;
          indexNode->value = value;

          if (indexNode->value == 0)
          {
            if (prevNode != NULL)
            {
              prevNode->next = indexNode->next;
            }
            else
            {
              pHead = indexNode->next;
            }
            tempNode = indexNode->next;
            delete indexNode;
            indexNode = tempNode;
          }
        }
        /* S4f: The new node partially overlaps the indexNode from the end.  */
        else
        {
          // Split the indexNode and insert the new node in between.
          RangeNode* splitNode = new RangeNode(newNode->end, indexNode->end, indexNode->value);
          splitNode->next = indexNode->next;

          // Update the node's begin, end, and next values
          indexNode->end = newNode->begin;
          indexNode->next = newNode;
          newNode->next = splitNode;

          if (splitNode->value == 0)
          {
            if (prevNode != NULL)
            {
              prevNode->next = splitNode->next;
            }
            else
            {
              pHead = splitNode->next;
            }
            tempNode = splitNode->next;
            delete splitNode;
            splitNode = tempNode;
          }
        }
      }

      mergeNodes(&pHead);

      return 1;
    }
  }

  /* Step 5: If the code has reached this point, then the index node has reached the
     end of the list without meeting any insert conditions. Therefore insert
     at the end of the list here.                                            */
  if (prevNode != NULL)
  {
    prevNode->next = newNode;
  }

  if (newNode->value == 0)
  {
    if (prevNode != NULL)
    {
      prevNode->next = newNode->next;
    }
    else
    {
      pHead = newNode->next;
    }
    tempNode = newNode->next;
    delete newNode;
    newNode = tempNode;
  }

  mergeNodes(&pHead);

  return 1;
}

//-----------------------------------------------------------------------------

/*
 * @brief: operator*
 *
 * Multiply the contents of two int ranglists, and return the value of the product.
 *
 * @param: rl2 - the other RangeList to multpiply with.
 *
 * @returns: Integer value of the product of both RangeLists.
 *
 */
int RangeList::operator*(RangeList const& rl2)
{
  /* Step1:  Create pointers to the head of both RangeLists                  */
  RangeNode* current = pHead;
  RangeNode* rhs = rl2.pHead;

  int product = 0;

  /* Step2: Traverse through both list's series of RangeNode values properly */
  while (current && rhs)
  {
    /* S2a: Check if both indexing range nodes are in the same position (==) */
    if (current->begin == rhs->begin)
    {
      product += (current->value * rhs->value);
      current = current->next;
      rhs = rhs->next;
    }
    /* S2b: Check if the current list's indexing range node
      is currently placed before the other list's indexing node (<)          */
    else if (current->begin < rhs->begin)
    {
      //  Check if the placement is overlapping, and take into account for total product
      if (rhs->end <= current->end)
      {
        product += (rhs->end - rhs->begin) * current->value;
        current = current->next;
      }
      // Check if there is a difference of placement to take into account for total product
      else if ((current->end - rhs->begin) > 0)
      {
        product += (current->end - rhs->begin) * current->value;
        current = current->next;
      }
      else
      {
        current = current->next;
      }
    }
    /* S2c: Check if the other list's indexing range node
       is currently placed before the current list's indexing node (>)       */
    else if (current->begin > rhs->begin) // >
    {
      //  Check if the placement is overlapping, and take into account for total product
      if (rhs->end >= current->end)
      {
        product += (current->end - rhs->begin) * current->value;
        rhs = rhs->next;
      }
      // Otherwise shift only the other list's indexing node forward.
      else
      {
        rhs = rhs->next;
      }
    }
    /* S2d: If the code has reached this point, shift both list's indexes forward */
    else
    {
      current = current->next;
      rhs = rhs->next;
    }
  }

  return product;
}

//-----------------------------------------------------------------------------

/*
 * @brief: operator[]
 *
 * Subscript operator functionality for a Node within a RangeList
 *
 * @param: position - The position within the RangeList to access
 *
 * @returns: NodeProxy of the requestion position of the RangeList
 *
 */
NodeProxy RangeList::operator[](const int position)
{
  return NodeProxy(*this, position);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
 // Private Functions:
 //----------------------------------------------------------------------------

/*
 * @brief: mergeNodes
 *
 * If an overlap has occured, and the values are the same,
 * then merge the values of the two node links properly
 *
 * @param: ppHead - handle to the pointer to the head of the linked list
 *
 * @returns:
 *
 */
void RangeList::mergeNodes(struct RangeNode** ppHead)
{
  RangeNode* current = *ppHead;
  while (current != NULL && current->next != NULL)
  {
    if ((current->value == current->next->value) && (current->end == current->next->begin))
    {
      /* Merge current and current->next */
      RangeNode* nextNode = current->next;
      current->end = nextNode->end;
      current->next = nextNode->next;

      // If the merged node is the head, update ppHead
      if (*ppHead == nextNode)
      {
        *ppHead = current;
      }

      delete nextNode;
    }
    else
    {
      current = current->next;
    }
  }
}

//-----------------------------------------------------------------------------

/*
 * @brief: splitNodes
 *
 * If an overlap has occured, split the beginning node
 *  into two seperate nodes for further handling and implementation
 *
 * @param: pphead    - pointer to the head of the linked list
 *         position  - position for the node to be created and inserted
 *         value     - value of the node to be created and inserted
 *
 * @returns:
 *
 */
void RangeList::splitNodes(struct RangeNode* node, int splitPosition)
{
  // ptr check for safety
  if (node == NULL)
  {
    return;
  }

  /* Create a new node to represent the second part after splitting */
  RangeNode* splitNode = new RangeNode(splitPosition, node->end, node->value);
  if (splitNode == NULL)
  {
    return;
  }
  splitNode->next = node->next;

  /* Update the original node to represent the first part after splitting */
  node->end = splitPosition;
  node->next = splitNode;

}

//-----------------------------------------------------------------------------

/*
 * @brief: delete_ranges
 *
 * Traverse a given RangeList, and delete all nodes within it.
 *
 * @param: node - pointer to the head of the RangeList
 *
 * @returns:
 *
 */
void RangeList::delete_ranges(RangeNode* node)
{
  if (!node) return;
  RangeNode* current = node;
  while (current) {
    RangeNode* temp = current;
    current = current->next;
    delete temp;
  }
}

//-----------------------------------------------------------------------------

/*
 * @brief: delete_node
 *
 * Given the position within the list of a target node,
 * delete the target node, and link the neighboring nodes
 * to reconnect the RangeList at that position.
 *
 * @param: position - The position of the RangeList to perform the operation
 *
 * @returns:
 *
 */
void RangeList::delete_node(int position)
{
  RangeNode* current = pHead;
  RangeNode* prev = NULL;
  while (current->next)
  {
    if (current->begin == position)
    {
      if (prev == NULL)
      {
        pHead = current->next;
      }
      else
      {
        prev->next = current->next;
      }
      delete current;
      return;
    }
    prev = current;
    current = current->next;
  }
}

//-----------------------------------------------------------------------------