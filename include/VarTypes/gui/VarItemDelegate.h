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
  \file    VarItemDelegate.h
  \brief   C++ Interface: VarItemDelegate
  \author  Stefan Zickler, (C) 2008
*/

#ifndef GRAPHITEMDELEGATE_H_
#define GRAPHITEMDELEGATE_H_
#include <QItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QComboBox>
#include "VarItem.h"
#include "VarTypes.h"
#include "VarTreeModel.h"
#include <QSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QDoubleSpinBox>

namespace VarTypes {
  /*!
    \class  VarItemDelegate
    \brief  The 'item-delegate' inheriting QItemDelegate for displaying VarTypes in the QT4 Item-Model
    \author Stefan Zickler, (C) 2008
    \see    VarTypes.h
  
    This file is part of the QT4 Item-Model for VarTypes.
    It is used when you want to edit/visualize VarTypes using
    QT4's model/view system.
  
    If you don't know what VarTypes are, please see \c VarTypes.h 
  */
  class VarItemDelegate : public QItemDelegate
  {
  Q_OBJECT
  public:
    VarItemDelegate(QObject *parent = 0);
    virtual ~VarItemDelegate();
  public slots:
    void editorChangeEvent();
  protected:
    virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual void drawBar ( VarType * dt, QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
  
    virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
  
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
  
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
  
    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
  
  
  };
};
#endif /*GRAPHITEMDELEGATE_H_*/
