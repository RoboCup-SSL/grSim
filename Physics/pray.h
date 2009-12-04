#ifndef PRAY_H
#define PRAY_H

#include "pobject.h"

class PRay : public PObject
{
private:
    float _length;
public:
    PRay(float length);
    virtual void init();
    void setPose(float x,float y,float z,float dx,float dy,float dz);
};

#endif // PRAY_H
