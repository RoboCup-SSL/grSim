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
  \file    varnotifier.h
  \brief   C++ Interface: varnotifier
  \author  Stefan Zickler, 2009
*/
//========================================================================
#ifndef VARNOTIFIER_H
#define VARNOTIFIER_H
#include "primitives/VarType.h"
#include <QObject>
#include <QMutex>
#include <QHash>
#include <QQueue>

namespace VarTypes {
  /**
    @author Stefan Zickler
    @brief  A helper class which accumulates the occurence of VarType changes
  */
  class VarNotifier : public QObject {
  Q_OBJECT
  public:
    enum VarNotificationType {
      VarNotificationChanged,
      VarNotificationEdited,
    };
  protected:
    void internalNonMutexedAddItem(VarType * item, VarNotificationType notification_type=VarNotificationChanged);
    void internalNonMutexedRemoveItem(VarType * item);
  public:
    QHash<VarType *, VarNotificationType> senders;
    bool changed;
    QMutex mutex;
  public slots:
      void changeSlotOtherChange();
  protected slots:
      void changeSlot(VarType * item);
  signals:
      void changeOccured(VarType * item);
  public:
      VarNotifier();
  
      ~VarNotifier();
  
      void addItem(VarType * item, VarNotificationType notification_type=VarNotificationChanged);
  
      void addRecursive(VarType * item, VarNotificationType notification_type=VarNotificationChanged, bool include_root=true); 
  
      void removeItem(VarType *item);
  
      void removeRecursive(VarType * item, bool include_root=true);
  
      /// This will report true if any events have happened, but it will not reset the event counter
      /// It is encouraged to use hasChanged() instead, if possible.
      bool hasChangedNoReset();
  
      /// This will report true if any events have happened and reset the event counter.
      bool hasChanged();
  
      void setChanged(bool value);
  
      void clear();
  
  };
};
#endif
