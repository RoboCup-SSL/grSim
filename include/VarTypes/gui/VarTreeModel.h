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
  \file    VarTreeModel.h
  \brief   C++ Interface: VarTreeModel
  \author  Stefan Zickler, (C) 2008
*/
#ifndef VARTREE_H_
#define VARTREE_H_
#include <QStandardItemModel>
#include <QObject>
#include "VarItem.h"
#include "VarTypes.h"

namespace VarTypes {
  
  /*!
    \class  VarTreeModel
    \brief  The 'item-model' inheriting QStandardItemModel for displaying VarTypes in the QT4 Item-Model
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    This file is part of the QT4 Item-Model for VarTypes.
    It is used when you want to edit/visualize VarTypes using
    QT4's model/view system.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarTreeModel : public QStandardItemModel
  {
    Q_OBJECT
  
  protected:
    VarTreeViewOptions opts;
  public:
    void resetViewOptions();
    const VarTreeViewOptions * getViewOptions() const;
    VarTreeModel();
    virtual ~VarTreeModel();
    void updateHeader();
    void setRootItems(vector<VarType *> items);
    void setRootItem(VarType * item);
    virtual const QStandardItem * itemPrototype () const;
    QList<VarItem *> findItems( const QString & text, bool case_sensitive=false ) const;
    QList<VarItem *> findItems( const VarType * item) const;
  
  };
};
#endif /*VDATATYPETREEWIDGETITEM_H_*/
