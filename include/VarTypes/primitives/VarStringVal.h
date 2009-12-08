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
  \file    VarStringVal.h
  \brief   C++ Interface: VarStringVal
  \author  Stefan Zickler, (C) 2008
*/

#ifndef VSTRINGVAL_H_
#define VSTRINGVAL_H_
#include "primitives/VarVal.h"


namespace VarTypes {
  /*!
    \class  VarStringVal
    \brief  This is the string VarTypeVal of the VarTypes system
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarStringVal : public virtual VarVal
  {
  protected:
  
    string _val;
  public:
  
    VarStringVal(const string & default_val="") : VarVal()
    {
      lock();
      _val=default_val;
      unlock();
      changed();
    }

    virtual ~VarStringVal() {}

    virtual void printdebug() const
    {
      lock();
      printf("%s\n",_val.c_str());
      unlock();
    }

    virtual VarTypeId getType() const { return VARTYPE_ID_STRING; };
    virtual string getString() const { lock(); string v=_val; unlock(); return v;  };
    virtual bool   hasValue()  const { return false; };
    virtual bool setString(const string & val) { lock(); if (_val!=val) {_val=val; unlock(); changed(); return true;} else { unlock(); return false;} };

    virtual VarVal * clone() const {
      VarStringVal * tmp = new VarStringVal();
      tmp->setString(getString());
      return tmp;
    }

  };

};
#endif /*VSTRING_H_*/
