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
  \file    VarString.h
  \brief   C++ Interface: VarString
  \author  Stefan Zickler, (C) 2008
*/

#ifndef VSTRING_H_
#define VSTRING_H_
#include "primitives/VarType.h"
#include "primitives/VarStringVal.h"

namespace VarTypes {
  /*!
    \class  VarString
    \brief  This is the string VarType of the VarTypes system
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarString : public VarTypeTemplate<VarStringVal> 
  {
    Q_OBJECT
  protected:
    SafeVarVal<VarStringVal> _def;
  public:
    VarString(string name="", string default_val="") : VarStringVal(default_val), VarTypeTemplate<VarStringVal>(name)
    {
      _def.setString(default_val);
      changed();
    }

    virtual ~VarString() {}

    virtual void resetToDefault()
    {
      setString(_def.getString());
    }

    virtual void setDefault(string val)
    {
      _def.setString(val);
      changed();
    }
  };
};
#endif /*VSTRING_H_*/
