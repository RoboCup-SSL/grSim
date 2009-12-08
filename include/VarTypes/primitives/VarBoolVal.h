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
  \file    VarBoolVal.h
  \brief   C++ Interface: VarBoolVal
  \author  Stefan Zickler, (C) 2008
*/

#ifndef VARBOOLVAL_H_
#define VARBOOLVAL_H_
#include "primitives/VarVal.h"

namespace VarTypes {
  /*!
    \class  VarBoolVal
    \brief  A Vartype for storing booleans
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  
  class VarBoolVal : public virtual VarVal
  {

  protected:
    bool _val;

  public:
  
    VarBoolVal(bool default_val=false) : VarVal()
    {
      lock();
      _val=default_val;
      unlock();
      changed();
    }
  
    virtual ~VarBoolVal() {}
  
    virtual void printdebug() const
    {
      lock();
      printf("%s\n",(_val ? "true" : "false"));
      unlock();
    }
  
    virtual VarVal * clone() const {
      VarBoolVal * tmp = new VarBoolVal();
      tmp->setBool(getBool());
      return tmp;
    }
  
    virtual VarTypeId getType() const { return VARTYPE_ID_BOOL; };
  
    /// will return the string "true" if true, or "false" if false.
    virtual string getString() const { return (getBool() ? "true" : "false"); };
    /// will return 1.0 if true, 0.0 if false.
    virtual double getDouble()const { return (getBool() ? 1.0 : 0.0); };
    /// will return 1 if true, 0 if false
    virtual int    getInt()   const { return (getBool() ? 1 : 0); };
    /// return the boolean value
    virtual bool   getBool() const  { lock(); bool v=_val; unlock(); return v; };
  
    /// will set this to true if the string is "true" or false otherwise
    virtual bool setString(const string & val) { return setBool(val=="true"); };
    /// will set this to true if the value is 1.0 or false otherwise
    virtual bool setDouble(double val) { return setBool(val==1.0); };
    /// will set this to true if the value is 1 or false otherwise
    virtual bool setInt(int val)       { return setBool(val==1); };
    /// set this to a particular boolean value
    virtual bool setBool(bool val)     { lock(); if (val!=_val) { _val=val; unlock(); changed(); return true;} else { unlock(); return false; }; };
  
    //plotting functions:
    virtual bool hasValue() const { return true; }
    virtual bool hasMaxValue() const { return true; }
    virtual bool hasMinValue() const { return true; }
    virtual double getMinValue() const { return 0.0; }
    virtual double getMaxValue() const  { return 1.0; }
    virtual double getValue() const { return getDouble(); }

  };
};
#endif /*VBOOL_H_*/
