/* $OpenXM: OpenXM/src/kan96xx/Kan/Kclass/tree.c,v 1.4 2003/11/24 08:16:13 takayama Exp $ */
#include <stdio.h>
#include "../datatype.h"
#include "../stackm.h"
#include "../extern.h"
#include "../gradedset.h"
#include "../extern2.h"
#include "../kclass.h"



/* Data conversion function : see KclassDataConversion*/
struct object KpoTree(struct object ob) {
  struct object rob;
  struct object ob1,ob2,ob3;
  struct object *newobp;
  rob.tag = Sclass;
  rob.lc.ival = CLASSNAME_tree;
  newobp = (struct object *) sGC_malloc(sizeof(struct object));
  if (newobp == NULL) errorKan1("%s\n","Kclass/indeterminate.c, no more memory.");
  if (ob.tag != Sarray) {
    errorKan1("%s\n","Kclass/indeterminate.c, only properly formatted list object can be transformed into tree. [name, attr-list, arglist].");
  }
  if (getoaSize(ob) < 3) {
    errorKan1("%s\n","Kclass/indeterminate.c, the length must 3 or more than 3. [name, attr-list, arglist].");
  }
  ob1 = getoa(ob,0); ob2 = getoa(ob,1); ob3 = getoa(ob,2);
  if (ob1.tag != Sdollar || ob2.tag != Sarray || ob3.tag != Sarray) {
    errorKan1("%s\n","Kclass/indeterminate.c, [string name, list attributes, list arglist].");
  }
  *newobp = ob;   /* Add class-tag to the array */
  rob.rc.voidp = newobp;
  return(rob);
}


/* Printing function : see fprintClass */
void fprintTree(FILE *fp,struct object op)
{
  printObject(KopTree(op),0,fp);
}

int isTreeAdd(struct object ob) {
  struct object name;
  if (ob.tag != Sclass) {
    return(0);
  }
  if (ectag(ob) != CLASSNAME_tree) {
    return(0);
  }
  ob = KopTree(ob);
  if (ob.tag != Sarray) {
    errorKan1("%s\n","CLASSNAME_tree is broken. Should be array.");
  }
  name = getoa(ob,0);
  if (name.tag != Sdollar) {
    errorKan1("%s\n","CLASSNAME_tree is broken. Should be string.");
  }
  if (strcmp(KopString(name),"add") == 0) {
    return(1);
  }else{
    return(0);
  }
}

struct object addTree(struct object ob1, struct object ob2)
{
  struct object rob,aob;
  struct object ob3,ob4;
  struct object attr;
  struct object keyValue;
  int i;
  if (isTreeAdd(ob1) && !isTreeAdd(ob2)) {
    ob1 = KopTree(ob1);
    ob3 = getoa(ob1,2);
    aob = newObjectArray(getoaSize(ob3)+1);
    for (i=0; i<getoaSize(ob3); i++) {
      putoa(aob,i,getoa(ob3,i));
    }
    putoa(aob,getoaSize(ob3),ob2);
  }else if (!isTreeAdd(ob1) && isTreeAdd(ob2)) {
    ob2 = KopTree(ob2);
    ob3 = getoa(ob2,2);
    aob = newObjectArray(getoaSize(ob3)+1);
    putoa(aob,0,ob1);
    for (i=0; i<getoaSize(ob3); i++) {
      putoa(aob,1+i,getoa(ob3,i));
    }
  }else if (isTreeAdd(ob1) && isTreeAdd(ob2)) {
    ob1 = KopTree(ob1);
    ob2 = KopTree(ob2);
    ob3 = getoa(ob1,2);
    ob4 = getoa(ob2,2);
    aob = newObjectArray(getoaSize(ob3)+getoaSize(ob4));
    for (i=0; i<getoaSize(ob3); i++) {
      putoa(aob,i,getoa(ob3,i));
    }
    for (i=0; i<getoaSize(ob4); i++) {
      putoa(aob,getoaSize(ob3)+i,getoa(ob4,i));
    }
  }else{
    aob = newObjectArray(2);
    putoa(aob,0,ob1);
    putoa(aob,1,ob2);
  }
  attr = newObjectArray(1);
  keyValue = newObjectArray(2);
  putoa(keyValue,0,KpoString("arith1"));
  putoa(keyValue,1,KpoString("plus"));
  putoa(attr,0,keyValue);
  rob = newObjectArray(3);
  putoa(rob,0,KpoString("add"));
  putoa(rob,1,attr);
  putoa(rob,2,aob);
  return(KpoTree(rob));
}

/* XML DOM-like interfaces */
struct object KtreeGetDocumentElement(struct object to) {
  struct object rob;
  struct object ob;
  if (to.tag != Sclass) errorKan1("%s\n","KtreeGetDocumentElement");
  if (ectag(to) != CLASSNAME_tree) errorKan1("%s\n","KtreeGetDocumentElement");
  ob = KopTree(to);
  if (ob.tag != Sarray) errorKan1("%s\n","KtreeGetDocumentElement: not an array.");
  if (getoaSize(ob) != 3) errorKan1("%s\n","KtreeGetDocumentElement: array is broken.");
  ob=getoa(ob,0);
  if (ob.tag != Sdollar) errorKan1("%s\n","KtreeGetDocumentElement: element must be a string");
  return ob;
}

struct object KtreeGetAttributes(struct object to) {
  struct object rob;
  struct object ob;
  if (to.tag != Sclass) errorKan1("%s\n","KtreeGetAttributes:");
  if (ectag(to) != CLASSNAME_tree) errorKan1("%s\n","KtreeGetAttributes:");
  ob = KopTree(to);
  if (ob.tag != Sarray) errorKan1("%s\n","KtreeGetAttributes: not an array.");
  if (getoaSize(ob) != 3) errorKan1("%s\n","KtreeGetAttributes: array is broken.");
  return getoa(ob,1);
}

struct object KtreeGetChildNodes(struct object to) {
  struct object rob;
  struct object ob;
  if (to.tag != Sclass) errorKan1("%s\n","KtreeGetChildNodes:");
  if (ectag(to) != CLASSNAME_tree) errorKan1("%s\n","KtreeGetChildNodes:");
  ob = KopTree(to);
  if (ob.tag != Sarray) errorKan1("%s\n","KtreeGetChildNodes: not an array.");
  if (getoaSize(ob) != 3) errorKan1("%s\n","KtreeGetChildNodes: array is broken.");
  return getoa(ob,2);
}

struct object KtreeCreateElement(struct object ostr) {
  struct object ob;
  if (ostr.tag != Sdollar) errorKan1("%s\n","KtreeCreateElement: not a string.");
  ob = newObjectArray(3);
  getoa(ob,0)=ostr;
  getoa(ob,1) = newObjectArray(0);
  getoa(ob,2) = newObjectArray(0);
  return KpoTree(ob);
}

