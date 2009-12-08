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
  \file    VarTreeView.h
  \brief   C++ Interface: VarTreeView
  \author  Stefan Zickler, (C) 2008
*/
#ifndef VARTREEVIEW_H_
#define VARTREEVIEW_H_
#include <QObject>
#include <QWidget>
#include <QTreeView>
#include <QVBoxLayout>
#include <QLineEdit>
#include "VarTypes.h"
#include "gui/VarTreeModel.h"
#include "gui/VarItem.h"
#include "gui/VarItemDelegate.h"
namespace VarTypes {
  /*!
    \class  VarTreeView
    \brief  The 'view' using a QTreeView for displaying VarTypes in the QT4 Item-Model
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    This file is part of the QT4 Item-Model for VarTypes.
    It is used when you want to edit/visualize VarTypes using
    QT4's model/view system.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarTreeView : public QWidget
  {
    Q_OBJECT
  protected:
    QList<VarItem *> search_result;
    int result_idx;
    QLineEdit * search_edit;
    QTreeView * tw; //tree widget
    VarItemDelegate * delegate;
    VarTreeModel * model;
    QVBoxLayout * l;
  protected slots:
    void newItemChecksRows(const QModelIndex & parent, int start, int end);
    void checkDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
  public:
    void setModel(VarTreeModel * tmodel);
    VarTreeView(VarTreeModel * tmodel=0);
    virtual ~VarTreeView();
    void fitColumns(); //fit columns to data
    void expandAndFocus(VarType * search); //will focus on and expand an item, if it exists
  public slots:
    void search(const QString & text);
    void nextSearchResult();
  };
};
#endif /*VARTREEVIEW_H_*/
