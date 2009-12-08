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
  \file    VarDoubleVal.h
  \brief   C++ Interface: VarDoubleVal
  \author  Stefan Zickler, (C) 2008
*/

#ifndef VDOUBLEVAL_H_
#define VDOUBLEVAL_H_
#include "primitives/VarVal.h"

namespace VarTypes {
  /*!
    \class  VarDouble
    \brief  A Vartype for storing double precision floating points
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  
  class VarDoubleVal : public virtual VarVal
  {

  protected:
    double _val;

  public:
    VarDoubleVal(double default_val=0) : VarVal()
    {
      lock();
      _val=default_val;
      unlock();
      changed();
    }

    /// set the value of this node to val.
    virtual bool setDouble(double val)
    {
      lock();
      if (val!=_val) {
        _val=val;
        unlock();
        changed();
        return true;
      } else {
        unlock();
        return false;
      }
    };
  
    virtual VarVal * clone() const {
      VarDoubleVal * tmp = new VarDoubleVal();
      tmp->setDouble(getDouble());
      return tmp;
    }

    virtual void printdebug() const
    {
      lock();
      printf("%f\n",_val);
      unlock();
    }
  
    virtual VarTypeId getType() const { return VARTYPE_ID_DOUBLE; };
  
    /// get a maximum precision string representation of the current value
    /// Internally this uses sprintf with the %lf argument
    virtual string getString() const
    {
      char result[255];
      result[0]=0;
      sprintf(result,"%lf",getDouble());
      return result;
    };
  
    /// get the value of this data-type
    virtual double getDouble() const { lock(); double v=_val; unlock(); return v; };
  
    /// will typecast the value to an int and return it.
    /// Note, that this will not perform any rounding, but rather a standard typecast.
    virtual int    getInt()    const { return (int)getDouble(); };
    /// will return true if the value is 1.0, or false otherwise
    virtual bool   getBool()   const { return (getDouble() == 1.0 ? true : false); };
  
    /// set this floating point value to some string value.
    /// internally, this uses sscanf with the %lf argument.
    virtual bool setString(const string & val) { double num=0; sscanf(val.c_str(),"%lf",&num); return setDouble(num); };
    virtual bool setInt(int val)       { return setDouble((double)val);  };
    virtual bool setBool(bool val)     { return setDouble(val ? 1.0 : 0.0);  };
  
    //plotting functions:
    virtual bool hasValue() const { return true; }
    virtual bool hasMaxValue() const { return false; }
    virtual bool hasMinValue() const { return false; }
    virtual double getMinValue() const { return 0.0; }
    virtual double getMaxValue() const  { return 1.0; }
    virtual double getValue() const { return getDouble(); }

  };
};
#endif /*VDOUBLE_H_*/
