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
  \file    VarBlob.h
  \brief   C++ Interface: VarBlob
  \author  Stefan Zickler, (C) 2008
*/

#ifndef VBLOB_H_
#define VBLOB_H_
#include "primitives/VarType.h"
#include "VarBase64.h"

namespace VarTypes {

  /*!
    \class  VarBlob
    \brief  A Vartype for storing binary data
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    The VarBlob Vartype allows for storage of any continuous binary string.
    It will automatically ensure that the data will be converted to/from
    base64 when storing/loading to/from XML.
  
    Note, that to use this VarType, you will first have to manually allocate a
    data-block and supply that data-pointer and a max-size as arguments to the
    VarBlob.
  
    Alternatively, you can use its \c allocate(size) function to construct the data-block
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  
  class VarBlob : public VarType
  {
    Q_OBJECT
  protected:
    long _dsize;
    uint8_t * _dpointer;
    bool _dsize_limited;
  public:
  
    VarBlob(uint8_t * external_pointer, int maxsize, string name="") : VarType(name)
    {
      if (external_pointer==0 || maxsize<=0) {
        _dpointer=external_pointer;
        _dsize=maxsize;
        _dsize_limited=false;
      } else {
        _dpointer=external_pointer;
        _dsize=maxsize;
        _dsize_limited=true;
      }
      changed();
    }
  
    VarBlob(int maxsize=0, string name="") : VarType(name)
    {
      if (maxsize > 0) {
        _dsize_limited=true;
      } else {
        _dsize_limited=false;
      }
      _dpointer=0;
      _dsize=0;
      allocate(maxsize);
    }
  
    virtual ~VarBlob() {}
  
    virtual uint8_t * getDataPointer() const { return _dpointer; };
    virtual long getDataSize() const { return _dsize; } ;
  
    virtual void allocate(int size) {
      if (_dpointer != 0) delete[] _dpointer;
      if (size<=0) {
        _dsize=0;
        _dpointer=0;
      } else {
        _dsize=size;
        _dpointer=new uint8_t[size];
      }
      changed();
    }
  
    virtual void clearData() {
      if (_dpointer != 0) delete _dpointer;
      _dsize=0;
      changed();
    }
  
    virtual void setDataPointer( uint8_t * pointer, int size) {
      _dpointer=pointer;
      _dsize=size;
      changed();
    }
  
    virtual void printdebug() const
    {
      printf("Data Blob pointer %p , size: %d\n",_dpointer,(int)_dsize);
    }
  
    virtual void getSerialString(string & val) const {
      VarBase64::getTool()->encode(getDataPointer(),getDataSize(), val,1);
    }
  
    virtual void setSerialString(const string & val) {
      VarBase64::getTool()->decode(val,getDataPointer(),getDataSize());
      changed();
    }

    virtual void getBinarySerialString(string & val) const {
      val.assign((const char*)getDataPointer(),getDataSize());
    }
  
    virtual void setBinarySerialString(const string & val) {
      long len = val.length();
      long maxlen = getDataSize();
      if (len > maxlen) len=maxlen;
      memcpy(getDataPointer(),val.data(), len);
      changed();
    }

  
    virtual VarTypeId getType() const { return VARTYPE_ID_BLOB; };
    virtual string getString() const { return "Pointer";  };
    virtual bool hasValue()  const { return false; };
    virtual bool setString(const string & val) { (void)val; return false; };
  
  //Qt model/view gui stuff:
  public:
    virtual QWidget * createEditor(const VarItemDelegate * delegate, QWidget *parent, const QStyleOptionViewItem &option) {
      (void)delegate;
      (void)option;
      (void)parent;
      return 0;
    }
  
  };
};
#endif /*VSTRING_H_*/
