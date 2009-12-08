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
  \file    VarTrigger.h
  \brief   C++ Interface: VarTrigger
  \author  Stefan Zickler, (C) 2008
*/


#ifndef VARTRIGGER_H_
#define VARTRIGGER_H_
#include "primitives/VarType.h"
#include <QPushButton>

namespace VarTypes {
  /*!
    \class  VarTrigger
    \brief  This is a Trigger-like VarType of the VarTypes system
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    This vartype will be represented as a single button in the
    GUI. If clicked, the trigger() slot will be called and an
    internal counter will be incremented by one.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarTrigger : public VarType
  {
    Q_OBJECT
  protected:
    int _counter;
    string label;
  signals:
    void signalTriggered();
  protected slots:
    void trigger() {
      lock();
      _counter++;
      emit(signalTriggered());
      unlock();
      changed();
      wasEdited((VarType*)this);
    }
  
  public:
  
    /// constructs a new VarTrigger
    /// \param _label The label of the button to be rendered in a GUI view.
    VarTrigger(string _name="", string _label="") : VarType(_name)
    {
      label=_label;
      _counter=0;
      _flags |= VARTYPE_FLAG_PERSISTENT;
      changed();
    }
  
    virtual ~VarTrigger() {
    }
  
    virtual void resetToDefault()
    {
    }
  
    /// get and reset the internal counter
    virtual int getAndResetCounter() {
      lock();
      int v=_counter;
      _counter=0;
      unlock();
      return v;
    }
  
    /// get the internal counter
    virtual int getCounter() {
      lock();
      int v=_counter;
      unlock();
      return v;
    }
  
    /// reset the internal counter back to zero.
    virtual void resetCounter() {
      lock();
      _counter=0;
      unlock();
    }
  
    virtual void printdebug() const
    {
    }
  
    /// get the label of the button
    virtual string getLabel() const {
      lock();
      string tmp=label;
      unlock();
      return tmp;
    }
  
    /// set the label of the button
    virtual void setLabel(string _label) {
      lock();
      label=_label;
      unlock();
      changed();
    }
  
    virtual VarTypeId getType() const { return VARTYPE_ID_TRIGGER; };
  
    //Qt model/view gui stuff:
    public:
    virtual QWidget * createEditor(const VarItemDelegate * delegate, QWidget *parent, const QStyleOptionViewItem &option) {
      (void)delegate;
      (void)option;
      QPushButton * tmp=new QPushButton(parent);
      tmp->setText(QString::fromStdString(label));
      connect(tmp,SIGNAL(clicked()),this,SLOT(trigger()));
      return tmp;
    }
    virtual void setEditorData(const VarItemDelegate * delegate, QWidget *editor) const {
      (void)delegate;
      (void)editor;
    }
    virtual void setModelData(const VarItemDelegate * delegate, QWidget *editor) {
      (void)delegate;
      (void)editor;
    }
  
  };
};
#endif
