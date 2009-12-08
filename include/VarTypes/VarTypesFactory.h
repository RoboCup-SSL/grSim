//
// C++ Interface: VarTypeFactory
//
// Description: 
//
//
// Author: Stefan Zickler <szickler@cs.cmu.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VARTYPESFACTORY_H
#define VARTYPESFACTORY_H

  //include base-class
  #include "primitives/VarType.h"
  
  //include primitives
  #include "primitives/VarString.h"
  #include "primitives/VarInt.h"
  #include "primitives/VarBool.h"
  #include "primitives/VarDouble.h"

  #include "primitives/VarBlob.h"
  #include "primitives/VarList.h"
  #include "primitives/VarStringEnum.h"
  #include "primitives/VarSelection.h"
  #include "primitives/VarExternal.h"
  #include "primitives/VarQWidget.h"
  #include "primitives/VarTrigger.h"

  #ifdef DT_USE_TYPES_TIMEBASED
    #include "enhanced/TimeLine.h"
    #include "enhanced/TimeVar.h"
  #endif

namespace VarTypes {  
  /**
    @author Stefan Zickler <szickler@cs.cmu.edu>
  */
  class VarTypesFactory{
  public:
    VarTypesFactory();
    virtual ~VarTypesFactory();
  
  protected:
    virtual VarType * newUserVarType(VarTypeId t);
    virtual VarVal  * newUserVarVal(VarTypeId t);
    virtual VarTypeId stringToUserType(const string & s);
    virtual string    userTypeToString(VarTypeId t);
  
  public:
    VarType   * newVarType(VarTypeId t);
    VarVal    * newVarVal(VarTypeId t);
    VarTypeId   stringToType(const string & s);
    string      typeToString(VarTypeId t);
  
  };
};
#endif
