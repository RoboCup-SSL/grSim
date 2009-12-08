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
  \file    VarType.h
  \brief   C++ Interface: VarType
  \author  Stefan Zickler, (C) 2008
*/
#ifndef VDATATYPE_H_
#define VDATATYPE_H_
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <limits.h>
#include <float.h>
#include <set>
#include <queue>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QLineEdit>
#include "primitives/VarVal.h"
#include <QObject>
  
  #ifndef VDATA_NO_XML
    #include "xml/xmlParser.h"
  #endif
  
  #ifndef VDATA_NO_THREAD_SAFETY
    #include <QMutex>
  #endif

namespace VarTypes {
  class VarItemDelegate;
  
  /*!
    \class  VarType
    \brief  The base class of the VarTypes system.
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    This is is the base-class of the VarType system.
    All other VarTypes must publicly inherit this class.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  
  using namespace std;
  /*!
    \enum vDataFlags
    \brief bitflags for setting meta-properties of a VarType
  */
  enum VarTypeFlagEnum {
    VARTYPE_FLAG_NONE = 0x00,  /// no special properties
    VARTYPE_FLAG_READONLY= 0x01 << 0, /// this vartype is not editable in the GUI
    VARTYPE_FLAG_HIDDEN  = 0x01 << 1, /// this vartype will be hidden in the GUI
    VARTYPE_FLAG_AUTO_EXPAND = 0x01 << 2, /// whether to expand this node by default
    VARTYPE_FLAG_AUTO_EXPAND_TREE = 0x01 << 3, /// whether to expand this node and all of its sub-nodes by default
    VARTYPE_FLAG_NOSAVE = 0x01 << 4, /// don't store this subtree to XML
    VARTYPE_FLAG_NOLOAD = 0x01 << 5, /// don't load this subtree from XML
    VARTYPE_FLAG_PERSISTENT = 0x01 << 6, /// make the item editor be persistent (always open, not just when clicked on it)
    VARTYPE_FLAG_HIDE_CHILDREN = 0x01 << 7, /// hide the entire subtree (but not node itself)
    VARTYPE_FLAG_NOLOAD_ENUM_CHILDREN = 0x01 << 8, /// don't load this subtree from XML  
    VARTYPE_FLAG_ENUM_COUNT = 0x01 << 9,
    VARTYPE_FLAG_NOSTORE = VARTYPE_FLAG_NOSAVE | VARTYPE_FLAG_NOLOAD
  };
  
  typedef int VarTypeFlag;
  
  
  //if using QT, trigger the hasChanged() signal
  #define VARTYPE_MACRO_CHANGED emit(hasChanged(this));
  
  #ifndef VDATA_NO_THREAD_SAFETY
    #define VARTYPE_MACRO_LOCK _mutex->lock();
    #define VARTYPE_MACRO_UNLOCK _mutex->unlock();
  #else
    #define VARTYPE_MACRO_LOCK
    #define VARTYPE_MACRO_UNLOCK
  #endif
  
  
  //if using QT, inherit QObject as a base
  class VarType : public QObject, public virtual VarVal
  {
  

    //if using QT, enable signals by employing the q_object macro
    Q_OBJECT
  signals:
    /// This signal is triggered when any data or flag of this VarType has changed.
    /// This includes changes that were done programmatically, as well through a GUI.
    /// It also includes changes to render-flags or other meta-data.
    void hasChanged(VarType *);
  
    /// This signal is triggered when data of this VarType has been edited by a user through an MVC viewer.
    /// Unlike /c hasChanged(), this signal is not necessarily triggered if this data was internally changed.
    /// So, if you would like to catch all events, you should use /c hasChanged() instead.
    void wasEdited(VarType *); //this signal is only triggered when a value was edited by a user through a MVC view-instance.
  
    /// This signal is triggered if data was written from an xml node
    void XMLwasRead(VarType *);
  
    /// This signal is triggered if data was written to an xml node
    void XMLwasWritten(VarType *);
  
  public slots:
    /// A slot to receive signals from a model-view system that editing of this item was just completed.
    void mvcEditCompleted() {
      wasEdited(this);
    }

  protected:
    virtual inline void lock() const {
      VARTYPE_MACRO_LOCK;
    }
    virtual inline void unlock() const {
      VARTYPE_MACRO_UNLOCK;
    }
    virtual inline void changed() {
      VARTYPE_MACRO_CHANGED;
    }
  
    #ifndef VDATA_NO_THREAD_SAFETY
      QMutex * _mutex;
    #endif
    VarTypeFlag _flags;
    string _name;
    static string fixString(const char * cst);
    static string intToString(int val);
    static string doubleToString(double val);
  
  public:
  
    /// The constructor of a VarType type. If inheriting this class, you should forward the name argument
    /// to this constructor.
    ///
    /// \param name The string label of this VarType node
    VarType(string name="");
  
    virtual ~VarType();
  
    /// Return a list of children nodes of this VarType type.
    /// This can be an empty vector if the node does not have any children.
    virtual vector<VarType *> getChildren() const;
  
    /// removes all children, by actually deleting them in memory
    /// note that this is recursive.
    /// use it to un-allocate entire hierarchies from memory
    virtual void deleteAllChildren() {
    }

    /// Set the string label of this node.
    virtual void setName(string name);
  
    /// Get the string label of this node.
    virtual string getName() const;
  
    /// Reset the values of this node to default.
    virtual void resetToDefault();
  
    /// Return the current render option bitflags
    /// \see VarTypeFlag
    virtual VarTypeFlag getFlags() const;
  
    /// Set the current render option bitflags
    /// \see VarTypeFlag
    virtual void setFlags(VarTypeFlag f);
  
    /// Add (bitwise-or) some renderflags f to the currently set renderflags.
    /// \see VarTypeFlag
    virtual void addFlags(VarTypeFlag f);
  
    /// Remove some renderflags f from the currently set renderflags.
    /// \see VarTypeFlag
    virtual void removeFlags(VarTypeFlag f);
  
    /// Check whether all the flags specified in f are currently set.
    /// \see VarTypeFlag  
    virtual bool areFlagsSet(VarTypeFlag f) const;
  
  
    /// Finds a child based on its label
    /// Returns 0 if not found
    VarType * findChild(string label) const;
  
    /// TODO: implement this function. It might be useful for some purposes.
    /// VarType * merge(VarType * structure, VarType * data);
  
  
  #ifndef VDATA_NO_XML
    //XML LOAD/STORE functions
  protected:
  
    //The following 6 functions are the ones to be reimplemented by subclasses to perform the correct
    //XML store/load actions
    virtual void updateAttributes(XMLNode & us) const;
    virtual void updateText(XMLNode & us) const;
    virtual void updateChildren(XMLNode & us) const;
  
    virtual void readAttributes(XMLNode & us);
    virtual void readText(XMLNode & us);
    virtual void readChildren(XMLNode & us);
  
    virtual void loadExternal();
  
    static XMLNode findOrAppendChild(XMLNode & parent, string key, string val);
  public:
    /// Clear an XMLNode's list of children.
    static void deleteAllVarChildren(XMLNode & node);
  
    /// A helper function to read a list of children from XML and convert it to a vector of VarType nodes.
    static vector<VarType *> readChildrenHelper(XMLNode & parent , vector<VarType *> existing_children, bool only_update_existing, bool blind_append);
  
    /// Write the contents of this VarType node to an XMLNode.
    void writeXML(XMLNode & parent, bool blind_append=true);
  
    /// Let this VarType node load the contents of an XMLNode.
    void readXML(XMLNode & us);
  #endif
  
  //-------------MODEL VIEW STUFF BELOW-------------------
  
  
  virtual QWidget * createEditor(const VarItemDelegate * delegate, QWidget *parent, const QStyleOptionViewItem &option);
  virtual void setEditorData(const VarItemDelegate * delegate, QWidget *editor) const;
  virtual void setModelData(const VarItemDelegate * delegate, QWidget *editor);
  virtual QSize sizeHint(const VarItemDelegate * delegate, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  virtual void paint (const VarItemDelegate * delegate, QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
  };
  
  template <class CLASS_VARVAL_TYPE>
  class VarTypeTemplate : public VarType, virtual public CLASS_VARVAL_TYPE {
    public:
    VarTypeTemplate(string name="") : VarType(name) {
    };
  };

  template <class CLASS_VARVAL_TYPE>
  class SafeVarVal : public virtual CLASS_VARVAL_TYPE {
    protected:
    #ifndef VDATA_NO_THREAD_SAFETY
      QMutex * _mutex;
    #endif
    public:
    SafeVarVal() {
      #ifndef VDATA_NO_THREAD_SAFETY
      _mutex=new QMutex();
      #endif
    }
    virtual ~SafeVarVal()
    {
      #ifndef VDATA_NO_THREAD_SAFETY
      delete _mutex;
      #endif
    }
    protected:
    virtual inline void lock() const {
      VARTYPE_MACRO_LOCK;
    }
    virtual inline void unlock() const {
      VARTYPE_MACRO_UNLOCK;
    }
  };

};


#endif /*VDATATYPE_H_*/
