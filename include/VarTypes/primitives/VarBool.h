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
  \file    VarBool.h
  \brief   C++ Interface: VarBool
  \author  Stefan Zickler, (C) 2008
*/

#ifndef VARBOOL_H_
#define VARBOOL_H_

#include "primitives/VarType.h"
#include "primitives/VarBoolVal.h"
#include <QCheckBox>
namespace VarTypes {
  /*!
    \class  VarBool
    \brief  A Vartype for storing booleans
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  
  class VarBool : public VarTypeTemplate<VarBoolVal>
  {
    Q_OBJECT
  protected:
    SafeVarVal<VarBoolVal> _def;
  public:
    VarBool(string name="", bool default_val=false) : VarBoolVal(default_val), VarTypeTemplate<VarBoolVal>(name)
    {
      _def.setBool(default_val);
      lock();
      _flags |= VARTYPE_FLAG_PERSISTENT;
      unlock();
      changed();
    }

    virtual ~VarBool() {}
  
    virtual void resetToDefault()
    {
      setBool(_def.getBool());
    }
  
    virtual void setDefault(bool val)
    {
      _def.setBool(val);
      changed();
    }
  
    //Qt model/view gui stuff:
    virtual QWidget * createEditor(const VarItemDelegate * delegate, QWidget *parent, const QStyleOptionViewItem &option) {
      (void)option;
      QCheckBox * checker=new QCheckBox(parent);
      connect((const QObject *)checker,SIGNAL(stateChanged(int)),(const QObject *)delegate,SLOT(editorChangeEvent()));
      if (getBool()) {
        checker->setText("True"); 
      } else {
        checker->setText("False");
      }
      return checker;
  
    }
    virtual void setEditorData(const VarItemDelegate * delegate, QWidget *editor) const {
      (void)delegate;
      QCheckBox * checker=(QCheckBox *) editor;
      checker->setChecked(getBool());
  
    }
    virtual void setModelData(const VarItemDelegate * delegate, QWidget *editor) {
      (void)delegate;
      QCheckBox * checker=(QCheckBox *) editor;
      if (checker->isChecked()) {
        checker->setText("True"); 
      } else {
        checker->setText("False");
      }
      if (setBool(checker->isChecked())) mvcEditCompleted();
  
    }
  
  };
};
#endif /*VBOOL_H_*/
