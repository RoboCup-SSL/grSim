//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    VarSelection.h
  \brief   C++ Interface: VarSelection
  \author  Stefan Zickler, (C) 2008
*/

#ifndef VAR_SELECTION_H_
#define VAR_SELECTION_H_
#include "primitives/VarType.h"
#include "primitives/VarBool.h"
#include <vector>
#include <QListWidget>
using namespace std;
namespace VarTypes {
  /*!
    \class  VarSelection
    \brief  This is the multi-selection VarType of the VarTypes system
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    The VarSelection class allows to define an ordered set of strings from which the user can select an arbitrary combination. This effectively models a popup / listbox allowing multi-item selection.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarSelection : public VarType
  {
    Q_OBJECT
  protected:
    vector<VarType *> list;
  public:
    VarSelection(string _name="", int num_items=0, bool default_value=false) : VarType(_name) {
      setSize(num_items,default_value);
      //addFlags( VARTYPE_FLAG_PERSISTENT );
    }

    virtual ~VarSelection() {
      int n = list.size();
      for (int i=n-1;i>=0;i--) {
        delete list[(unsigned int)i];
      }
    }
  
    virtual string getString() const { 
      string result;
      lock();
      //TODO:
      result="";
      //for (unsigned int i =0; i < list.size(); i++) {
      //  list[i]->getName()
      //}
      unlock();
      return result;
    }
    virtual void printdebug() const { printf("VarSelection named %s containing %zu element(s)\n",getName().c_str(), list.size()); }
  
    unsigned int getCount() const {
      lock();
      int n = list.size();
      unlock();
      return n;
    }
  
    /// check whether an item is currently selected
    bool isSelected(unsigned int i) const {
      bool result;
      lock();
      if (i >= list.size()) {
        result=false;
      } else {
        result=((VarBool *)list[i])->getBool();
      }
      unlock();
      return result;
    }
  
    /// select an item by using its index
    bool setSelected(unsigned int i, bool val) {
      bool result=false;
      lock();
      if (i < list.size()) {
        result=(((VarBool *)list[i])->setBool(val));
      }
      unlock();
      if (result) changed();
      return result;
    }
  
    /// get the string of item at a given index
    string getLabel(unsigned int index) const {
    string result="";
    lock();
    if (index >= list.size()) {
      result="";
    } else {
      result=((VarBool *)list[index])->getName();
    }
    unlock();
    return result;
    }
  
    /// trim or extend the list to a certain total number of items
    void setSize(unsigned int size, bool default_val=false) {
      lock();
      if (list.size() < size) {
        for (unsigned int i=list.size();i<size;i++) {
          char tmp[64];
          sprintf(tmp,"%zu",list.size());
          list.push_back(new VarBool(tmp ,default_val));
          list[list.size()-1]->addFlags(VARTYPE_FLAG_HIDDEN);
        }
      } else if (list.size() > size) {
        for (unsigned int i=list.size();i>size;i--) {
          delete list[i-1];
          list.pop_back();
        }
      }
      unlock();
      changed();
      return;
    }
  
    /// set the string of item at a given index
    void setLabel(unsigned int index, const string & label) {
    string result="";
    lock();
    if (index < list.size()) {
      list[index]->setName(label);
    }
    unlock();
    changed();
    return;
    }
  
    /// add an item to the end of the enumeration
    int addItem(bool value=false, string label="") {
      lock();
      if (label.compare("")==0) {
        char tmp[64];
        sprintf(tmp,"%zu",list.size());
        list.push_back(new VarBool(tmp , value));
      } else {
        list.push_back(new VarBool(label , value));
      }
      list[list.size()-1]->addFlags(VARTYPE_FLAG_HIDDEN);
      unlock();
      changed();
      return (list.size()-1);
    }
  
    virtual VarTypeId getType() const { return VARTYPE_ID_SELECTION; } ;
  
    virtual vector<VarType *> getChildren() const
    {
      lock();
      vector<VarType *> l;
      unsigned int n_children=list.size();
      l.reserve(n_children);
      for (unsigned int i=0; i < n_children; i++) { 
        l.push_back(list[i]);
      }
      unlock();
      return l;
    }
  
  #ifndef VDATA_NO_XML
  protected:
    virtual void readChildren(XMLNode & us)
    {
      if (areFlagsSet(VARTYPE_FLAG_NOLOAD_ENUM_CHILDREN)==false) {
        list=readChildrenHelper(us, list, false, false);
        for (unsigned int i = 0; i < list.size();i++) {
          list[i]->addFlags(VARTYPE_FLAG_HIDDEN);
        }
      }
      changed();
    }
  #endif
  
  //Qt model/view gui stuff:
  public:
    virtual QWidget * createEditor(const VarItemDelegate * delegate, QWidget *parent, const QStyleOptionViewItem &option) {
      (void)option;
      QListWidget * w=new QListWidget(parent);
      connect((const QObject *)w,SIGNAL(itemChanged( QListWidgetItem * )),(const QObject *)delegate,SLOT(editorChangeEvent()));
      w->setSelectionMode(QAbstractItemView::SingleSelection);
      return w;
    }
    virtual void setEditorData(const VarItemDelegate * delegate, QWidget *editor) const {
      (void)delegate;
      QListWidget * listwidget=(QListWidget*)editor;
      int n = getCount();
      QString tmp;
      listwidget->clear();
      for (int i=0;i<n;i++) {
        tmp=QString::fromStdString(getLabel(i));
        QListWidgetItem * item = new QListWidgetItem(tmp);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
        item->setCheckState(isSelected(i) ? Qt::Checked : Qt::Unchecked);
        //item->setSelected();
        listwidget->addItem(item);
      }
  
    }
  
    virtual void setModelData(const VarItemDelegate * delegate, QWidget *editor) {
      (void)delegate;
      QListWidget * listwidget=(QListWidget *)editor;
      bool changed=false;
      int in=listwidget->count();
      for (int i=0;i<in;i++) {
        QListWidgetItem * item = listwidget->item(i);
        if (item!=0) {
          changed = changed | (setSelected(i,item->checkState()==Qt::Checked));
        }
      }
      if (changed) mvcEditCompleted();
    }
  
    virtual QSize sizeHint(const VarItemDelegate * delegate, const QStyleOptionViewItem & option, const QModelIndex & index) const {
      (void)delegate;
      (void)option;
      (void)index;
      QSize s(20,100);
      return s;
    }
  
  };
};
#endif
