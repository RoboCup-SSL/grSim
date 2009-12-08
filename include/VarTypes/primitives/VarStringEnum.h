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
  \file    VarStringEnum.h
  \brief   C++ Interface: VarStringEnum
  \author  Stefan Zickler, (C) 2008
*/

#ifndef STRINGENUM_H_
#define STRINGENUM_H_
#include "primitives/VarType.h"
#include "primitives/VarString.h"
#include <vector>
#include <QComboBox>
using namespace std;

namespace VarTypes {
  /*!
    \class  VarStringEnum
    \brief  This is the string enumeration VarType of the VarTypes system
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    The VarStringEnum class allows to define an ordered set of strings from which the user can chose one. This effectively models a popup / listbox allowing
    a single-item selection.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarStringEnum : public VarType
  {
    Q_OBJECT
  protected:
    vector<VarType *> list;
    string selected;
    string default_string;
  public:
    VarStringEnum(string _name="", string _default="") : VarType(_name) {
      default_string=_default;
      selected=_default;
    }

    virtual ~VarStringEnum() {
      int n = list.size();
      for (int i=n-1;i>=0;i--) {
        delete list[(unsigned int)i];
      }
    }

    virtual string getString() const { return selected; }
    virtual void resetToDefault() { list.clear(); selected=default_string; };
    virtual void printdebug() const { printf("StringEnum named %s containing %zu element(s)\n",getName().c_str(), list.size()); }
  
    unsigned int getCount() const {
      lock();
      int n = list.size();
      unlock();
      return n;
    }
  
    /// set the currently selected string
    virtual bool setString(const string & val) {
      return select(val);
    }
  
    /// get the index of the currently selected string item
    int getIndex() const {
      lock();
      int res=-1;
      unsigned int n = list.size();
      for (unsigned int i=0;i<n;i++) {
        if (((VarString *)(list[i]))->getString().compare(selected)==0) {
          res= i;
          break;
        }
      }
      unlock();
      return res;
    }
  
    /// select an item by using its index
    bool selectIndex(unsigned int i) {
      string result="";
      lock();
      if (i > (list.size() - 1)) {
        result=default_string;
      } else {
        result=((VarString *)(list[i]))->getString();
      }
      if (result!=selected) {
        selected=result;
        unlock(); changed(); return true;
      } else {
        unlock(); return false;
      }
    }
  
    /// select a particular string
    bool select(const string & val) {
      //TODO: add a flag so only selection of existing items is possible.
      lock(); if (val!=selected) {selected=val; unlock(); changed(); return true;} else { unlock(); return false;}
    }
  
    /// return the currently selected string
    string getSelection() const {
      return selected;
    }
  
    /// get the string of item at a given index
    string getLabel(unsigned int index) const {
    string result="";
    lock();
    if (index > (list.size() - 1)) {
      result="";
    } else {
      result=((VarString *)(list[index]))->getString();
    }
    unlock();
    return result;
    }
  
    /// trim or extend the list to a certain total number of items
    void setSize(unsigned int size, const string default_label="") {
      lock();
      if (list.size() < size) {
        for (unsigned int i=list.size();i<size;i++) {
          char tmp[64];
          sprintf(tmp,"%zu",list.size());
          list.push_back(new VarString(tmp ,default_label));
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
    }
  
    /// set the string of item at a given index
    void setLabel(unsigned int index, const string & label) {
    string result="";
    lock();
    if (index < list.size()) {
      if (((VarString *)(list[index]))->getString().compare(selected)==0) {
        selected=label;
      }
      ((VarString *)(list[index]))->setString(label);
      
    }
    
    unlock();
    changed();
    return;
    }
  
    /// add an item to the end of the enumeration
    int addItem(const string & label) {
      lock();
      char tmp[64];
      sprintf(tmp,"%zu",list.size());
      list.push_back(new VarString(tmp ,label));
      list[list.size()-1]->addFlags(VARTYPE_FLAG_HIDDEN);
      unlock();
      changed();
      return (list.size()-1);
    }
  
    virtual VarTypeId getType() const { return VARTYPE_ID_STRINGENUM; } ;
  
    virtual vector<VarType *> getChildren() const
    {
      lock();
      vector<VarType *> l = list;
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
      //TODO add connect...
      (void)delegate;
      (void)parent;
      (void)option;
      QComboBox * w = new QComboBox(parent);
      connect((const QObject *)w,SIGNAL(currentIndexChanged( int )),(const QObject *)delegate,SLOT(editorChangeEvent()));
      return w;
    }
    
    virtual void setEditorData(const VarItemDelegate * delegate, QWidget *editor) const {
      (void)delegate;
      QComboBox * combo =(QComboBox *)editor;
      int n = getCount();
      QString tmp;
      combo->clear();
      for (int i=0;i<n;i++) {
        tmp=QString::fromStdString(getLabel(i));
        combo->insertItem(combo->count(), tmp);
        if (tmp==QString::fromStdString(getString())) {
          combo->setCurrentIndex(i);
        }
      }
    }
  
    virtual void setModelData(const VarItemDelegate * delegate, QWidget *editor) {
      (void)delegate;
      QComboBox * combo=(QComboBox *) editor;
      if (select(combo->currentText().toStdString())) mvcEditCompleted();
    };
  
  };
};
#endif /*DATAGROUP_H_*/
