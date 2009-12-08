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
  \file    VarBase64.h
  \brief   C++ Interface: VarBase64
  \author  Stefan Zickler, (C) 2008
*/
//========================================================================


#ifndef VARBASE64_H_
#define VARBASE64_H_
#include "xml/xmlParser.h"
namespace VarTypes {
  
  /*!
    \class  VarBase64
    \brief  A singleton wrapper to XMLParserBase64Tool
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    This is a singleton wrapper to the XMLParserBase64Tool
    It is used for the en/decoding of binary data to/from xml using the
    Base64 ascii format.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  //singleton instanciation of XML base64 encoder
  class VarBase64
    {
    public:
        static VarTypes::XMLParserBase64Tool* getTool();
    protected:
        VarBase64();
        VarBase64(const VarBase64&);
        VarBase64& operator= (const VarBase64&);
    private:
        static VarBase64* pinstance;
        VarTypes::XMLParserBase64Tool * tool;
  };
};
#endif /*VARBASE64_H_*/
