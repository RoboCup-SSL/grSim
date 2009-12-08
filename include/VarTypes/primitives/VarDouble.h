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
  \file    VarDouble.h
  \brief   C++ Interface: VarDouble
  \author  Stefan Zickler, (C) 2008
*/

#ifndef VDOUBLE_H_
#define VDOUBLE_H_
#include "primitives/VarType.h"
#include "primitives/VarDoubleVal.h"
#include <QDoubleSpinBox>

namespace VarTypes {
  /*!
    \class  VarDouble
    \brief  A Vartype for storing double precision floating points
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  
  class VarDouble : public VarTypeTemplate<VarDoubleVal> 
  {
    Q_OBJECT
  protected:
    SafeVarVal<VarDoubleVal> _def;
    double _min;
    double _max;
  public:
    /// get the minimum accepted value if this data-type has a limited range
    /// \see setMin(...)
    /// \see hasMin()
    /// \see unsetMin()
    virtual double getMin() const { lock(); double v=_min; unlock(); return v; }
  
    /// get the maximum accepted value if this data-type has a limited range
    /// \see setMax(...)
    /// \see hasMax()
    /// \see unsetMax()
    virtual double getMax() const { lock(); double v=_max; unlock(); return v; }
  
    /// check whether this data type has a limited minimum value
    /// \see getMin(...)
    /// \see setMin(...)
    /// \see unsetMin()
    virtual bool hasMin() const { lock(); bool v= (_min != DBL_MIN); unlock(); return v; }
  
    /// check whether this data type has a limited maximum value
    /// \see getMax(...)
    /// \see setMax(...)
    /// \see unsetMax()
    virtual bool hasMax() const { lock(); bool v= (_max != DBL_MAX); unlock(); return v; }
  
    /// limit the minumum value of this parameter to a pre-set value
    /// \see getMin(...)
    /// \see hasMin()
    /// \see unsetMin()
    virtual void setMin(double minval) { lock(); _min=minval; unlock(); changed(); }
  
    /// limit the maximum value of this parameter to a pre-set value
    /// \see getMax(...)
    /// \see hasMax()
    /// \see unsetMax()
    virtual void setMax(double maxval) { lock(); _max=maxval; unlock(); changed(); }
  
    /// unset any previous limit of minimum value of this parameter
    /// \see getMin(...)
    /// \see setMin(...)
    /// \see hasMin()
    virtual void unsetMin() { lock(); _min=DBL_MIN; unlock(); changed(); }
  
    /// unset any previous limit of maximum value of this parameter
    /// \see getMax(...)
    /// \see setMax(...)
    /// \see hasMax()
    virtual void unsetMax() { lock(); _max=DBL_MAX; unlock(); changed(); }
  
  
    /// set the value of this node to val.
    virtual bool setDouble(double val)
    {
      double tmp;
      if (hasMin() && val < getMin()) {
        tmp=_min;
      } else if (hasMax() &&  val > getMax()) {
        tmp=_max;
      } else {
        tmp=val;
      }
      return VarDoubleVal::setDouble(tmp);
    };
  
    VarDouble(string name="", double default_val=0, double min_val=DBL_MIN, double max_val=DBL_MAX) : VarDoubleVal(default_val), VarTypeTemplate<VarDoubleVal>(name)
    {
      setMin(min_val);
      setMax(max_val);
      setDefault(default_val);
      changed();
    }
  
    virtual ~VarDouble() {}
  
    virtual void resetToDefault()
    {
      setDouble(_def.getDouble());
    }
  
    virtual void setDefault(double val)
    {
      _def.setDouble(val);
      changed();
    }
  
  #ifndef VDATA_NO_XML
  protected:
    virtual void updateAttributes(XMLNode & us) const
    {
      if (hasMin()) {
        us.updateAttribute(doubleToString(getMin()).c_str(),"minval","minval");
      } else {
        us.updateAttribute("","minval","minval");
      }
      if (hasMax()) {
        us.updateAttribute(doubleToString(getMax()).c_str(),"maxval","maxval");
      } else {
        us.updateAttribute("","maxval","maxval");
      }
    }
  
    virtual void readAttributes(XMLNode & us)
    {
      string s = fixString(us.getAttribute("minval"));
      if (s=="") {
        unsetMin();
      } else {
        double num=0; sscanf(s.c_str(),"%lf",&num);
        setMin(num);
      }
  
      s = fixString(us.getAttribute("maxval"));
      if (s=="") {
        unsetMax();
      } else {
        double num=0; sscanf(s.c_str(),"%lf",&num);
        setMax(num);
      }
    }
  
  #endif
  
  
  //Qt model/view gui stuff:
  public:
  virtual QWidget * createEditor(const VarItemDelegate * delegate, QWidget *parent, const QStyleOptionViewItem &option) {
    //TODO: hookup editor changes on press-enter and on spin:
    (void)delegate;
    (void)parent;
    (void)option;
    QDoubleSpinBox * w = new QDoubleSpinBox(parent);
    //uncomment the following line for instant updates:
    //connect((const QObject *)w,SIGNAL(valueChanged ( double )),(const QObject *)delegate,SLOT(editorChangeEvent()));
    return w;
  }
  virtual void setEditorData(const VarItemDelegate * delegate, QWidget *editor) const {
    (void)delegate;
    QDoubleSpinBox * spin=(QDoubleSpinBox *) editor;
    spin->setDecimals(10);
    spin->setRange(getMin(),getMax() );
    spin->setValue(getDouble());
  }
  virtual void setModelData(const VarItemDelegate * delegate, QWidget *editor) {
    (void)delegate;
    QDoubleSpinBox * spin=(QDoubleSpinBox *) editor;
    if (setDouble(spin->value())) mvcEditCompleted();
  }
  
  };
};
#endif /*VDOUBLE_H_*/
