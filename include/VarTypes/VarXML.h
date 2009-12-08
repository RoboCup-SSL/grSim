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
  \file    VarXML.h
  \brief   C++ Interface: VarXML
  \author  Stefan Zickler, (C) 2008
*/
//========================================================================
#ifndef VARXML_H_
#define VARXML_H_
#include "VarTypes.h"
#include "xml/xmlParser.h"

namespace VarTypes {
  /*!
    \class  VarXML
    \brief  XML helper functions of the VarType system.
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    This class provides static functions to parse or write an XML file
    representing a VarTypes tree.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarXML
  {
  public:
    VarXML();
    virtual ~VarXML();
  
    /// write a VarType node (and all of its children) to an xml file
    static void write(VarType * rootVar, string filename);
  
    /// write a vector of VarType nodes (and all of their children) to an xml file
    static void write(vector<VarType *> rootVars, string filename);
  
    /// read a VarType-tree from an xml file and return its root nodes as a vector.
    ///
    /// \param existing_nodes represents any pre-defined vartypes tree and should
    ///                       be an empty vector if no tree pre-exists.
    ///
    /// If a tree does pre-exist then the read function will update the data of
    /// any existing nodes, or create new nodes if they are missing in the existing
    /// tree.
    static vector<VarType *> read(vector<VarType *> existing_nodes, string filename);
  };
};
#endif /*VARXML_H_*/
