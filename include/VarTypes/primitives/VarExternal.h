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
  \file    VarExternal.h
  \brief   C++ Interface: VarExternal
  \author  Stefan Zickler, (C) 2008
*/


#ifndef DATAGROUPEXTERNAL_H_
#define DATAGROUPEXTERNAL_H_
#include "primitives/VarList.h"

using namespace std;
namespace VarTypes {
  /*!
    \class  VarExternal
    \brief  A list of VarTypes to be stored in an separate XML file.
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    This is the external list VarType of the VarTypes system.
    It is similar to the VarList type (it inherits it), except
    that VarExternal allows saving/loading from an external
    xml-file, which is useful if you don't want to store the
    entire data-tree in the same XML file, but rather put
    a certain subtree into its own file.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarExternal : public VarList
  {
    Q_OBJECT
  protected:
    string filename;
  
  public:
    /// Construct a VarExternal list from an existing VarList
    VarExternal(string _filename, VarList * vlist);
  
    /// Construct a new VarExternal list
    VarExternal(string _filename="", string _name="");
  
    virtual ~VarExternal();
    virtual VarTypeId getType() const { return VARTYPE_ID_EXTERNAL; } ;
  
  
  #ifndef VDATA_NO_XML
  protected:
    virtual void updateAttributes(XMLNode & us) const
    {
      us.updateAttribute(filename.c_str(),"filename","filename");
      VarList::updateAttributes(us);
    }
  
    virtual void readAttributes(XMLNode & us)
    {
      filename = fixString(us.getAttribute("filename"));
      VarList::readAttributes(us);
    }
  
    virtual void updateChildren(XMLNode & us) const
    {
      (void)us;
      XMLNode parent = XMLNode::openFileHelper(filename.c_str(),"VarXML");
      //and update it...
      VarList::updateChildren(parent);
      //save file to empty parent
      parent.writeToFile(filename.c_str());
    }
  
    virtual void readChildren(XMLNode & us);
  
    virtual void loadExternal();
  
  #endif
  };
};
#endif /*DATAGROUP_H_*/
