#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#define _USE_MATH_DEFINES 
#include <iostream>
#include <cmath>
#include <algorithm>

/************************************************************************/
/*                        CVector                                       */
/************************************************************************/
class CVector {
public:
    CVector() : _x(0), _y(0) {}
    CVector(double x, double y) : _x(x), _y(y) {}
    CVector(const CVector& v) : _x(v.x()), _y(v.y()) {}
    bool setVector(double x, double y) {
        _x = x;
        _y = y;
        return true;
    }
    double mod() const {
        return std::sqrt(_x * _x + _y * _y);
    }
    double mod2() const {
        return (_x * _x + _y * _y);
    }
    double dir() const {
        return std::atan2(y(), x());
    }
    double theta(const CVector& v) {
        // the angle between this vector and vector v
        double _theta;
        _theta = std::atan2(_y, _x) - std::atan2(v.y(), v.x());
        if (_theta > M_PI) return _theta - 2 * M_PI;
        if (_theta < -M_PI) return _theta + 2 * M_PI;
        return _theta;
    }
    CVector rotate(double angle) const;
    CVector unit() const {
        CVector vector(_x, _y);
        if (vector.mod() < 1e-8) {
            return CVector(1, 0);
        }
        return CVector(vector.x() / vector.mod(),
            vector.y() / vector.mod());
    }
    double x() const {
        return _x;
    }
    double y() const {
        return _y;
    }
    double value(double angle) const {
        return mod() * std::cos(dir() - angle);
    }
    CVector operator +(const CVector& v) const {
        return CVector(_x + v.x(), _y + v.y());
    }
    CVector operator -(const CVector& v) const {
        return CVector(_x - v.x(), _y - v.y());
    }
    CVector operator *(double a) const {
        return CVector(_x * a, _y * a);
    }
    double operator *(CVector b) const {
        // dot product
        return double(_x * b.x() + _y * b.y());
    }
    double operator ^(CVector b) const {
        // length of cross product, signed
        return double(_x * b.y() - _y * b.x());
    }
    CVector operator ^(double b) const {
        // cross product with a verticle vector of length b
        return CVector(copysign(_y, b), -copysign(_x, b));
    }
    CVector operator /(double a) const {
        return CVector(_x / a, _y / a);
    }
    CVector operator -() const {
        return CVector(-1 * _x, -1 * _y);
    }
    friend std::ostream& operator <<(std::ostream& os, const CVector& v) {
        return os << "(" << v.x() << ":" << v.y() << ")";
    }

private:
    double _x, _y;
};

/************************************************************************/
/*                        CGeoPoint                                     */
/************************************************************************/
class CGeoPoint {
public:
    CGeoPoint() : _x(0), _y(0) {}
    ~CGeoPoint() {}
    CGeoPoint(double x, double y) : _x(x), _y(y) {}
    CGeoPoint(const CGeoPoint& p) : _x(p.x()), _y(p.y()) {}
    bool operator==(const CGeoPoint& rhs) {
        return ((this->x() == rhs.x()) && (this->y() == rhs.y()));
    }
    double x() const {
        return _x;
    }
    double y() const {
        return _y;
    }
    void setX(double x) {
        _x = x;
    }
    void setY(double y) {
        _y = y;
    }
    bool fill(double x, double y) {
        _x = x;
        _y = y;
        return true;
    }
    double dist(const CGeoPoint& p) const {
        return CVector(p - CGeoPoint(_x, _y)).mod();
    }
    double dist2(const CGeoPoint& p) const {
        return CVector(p - CGeoPoint(_x, _y)).mod2();
    }
    CGeoPoint operator+(const CVector& v) const {
        return CGeoPoint(_x + v.x(), _y + v.y());
    }
    CGeoPoint operator*(const double& a) const {
        return CGeoPoint(_x * a, _y * a);
    }
    CVector operator-(const CGeoPoint& p) const {
        return CVector(_x - p.x(), _y - p.y());
    }
    CGeoPoint midPoint(const CGeoPoint& p) const {
        return CGeoPoint((_x + p.x()) / 2, (_y + p.y()) / 2);
    }
    friend std::ostream& operator <<(std::ostream& os, const CGeoPoint& v) {
        return os << "(" << v.x() << ":" << v.y() << ")";
    }

private:
    double _x, _y;
};

/************************************************************************/
/*                        CGeoLine                                      */
/************************************************************************/
class CGeoLine {
public:
    CGeoLine() {}
    CGeoLine(const CGeoPoint& p1, const CGeoPoint& p2) : _p1(p1), _p2(p2) {
        calABC();
    }
    CGeoLine(const CGeoPoint& p, double angle) : _p1(p), _p2(p.x() + std::cos(angle), p.y() + std::sin(angle)) {
        calABC();
    }
    void calABC() {
        if (_p1.y() == _p2.y()) {
            _a = 0;
            _b = 1;
            _c = -1.0 * _p1.y();
        } else {
            _a = 1;
            _b = -1.0 * (_p1.x() - _p2.x()) / (_p1.y() - _p2.y());
            _c = (_p1.x() * _p2.y() - _p1.y() * _p2.x()) / (_p1.y() - _p2.y());
        }
    }

    CGeoPoint projection(const CGeoPoint& p) const {
        if (_p2.x() == _p1.x()) {
            return CGeoPoint(_p1.x(), p.y());
        } else {
            double k = (_p2.y() - _p1.y()) / (_p2.x() - _p1.x());
            // equation:                        y = k* ( x - pt1.x) + pt1.y
            // equation of perpendicular line:  y = (-1/k) * (x - p.x) + p.y
            // solve the two equations
            double x = (k * k * _p1.x() + k * (p.y() - _p1.y()) + p.x()) / (k * k + 1);
            double y = k * (x - _p1.x()) + _p1.y();
            return CGeoPoint(x, y);
        }
    }
    CGeoPoint point1() const {
        return _p1;
    }
    CGeoPoint point2() const {
        return _p2;
    }
    bool operator==(const CGeoLine& rhs) {
        return ((this->point1().x() == rhs.point1().x()) && (this->point1().y() == rhs.point1().y())
            && (this->point2().x() == rhs.point2().x()) && (this->point2().y() == rhs.point2().y()));
    }
    const double& a() const {
        return _a;
    }
    const double& b() const {
        return _b;
    }
    const double& c() const {
        return _c;
    }
private:
    CGeoPoint _p1;
    CGeoPoint _p2;

    // a*x+b*y+c=0, a>=0
    double _a;
    double _b;
    double _c;
};

class CGeoLineLineIntersection {
public:
    CGeoLineLineIntersection(const CGeoLine& line_1, const CGeoLine& line_2);
    bool Intersectant() const {
        return _intersectant;
    }
    const CGeoPoint& IntersectPoint() const {
        return _point;
    }
private:
    bool _intersectant;
    CGeoPoint _point;
};

/************************************************************************/
/*                        CGeoSegment                                   */
/************************************************************************/
class CGeoSegment : public CGeoLine {
public:
    CGeoSegment() {}
    CGeoSegment(const CGeoPoint& p1, const CGeoPoint& p2) : CGeoLine(p1, p2), _start(p1), _end(p2) {
        _compareX = std::abs(p1.x() - p2.x()) > std::abs(p1.y() - p2.y());
        _center = CGeoPoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);
    }
    bool IsPointOnLineOnSegment(const CGeoPoint& p) const {
        if (_compareX) {
            return p.x() > (std::min)(_start.x(), _end.x()) && p.x() < (std::max)(_start.x(), _end.x());
        }
        return p.y() > (std::min)(_start.y(), _end.y()) && p.y() < (std::max)(_start.y(), _end.y());
    }
    bool IsSegmentsIntersect(const CGeoSegment& p) const {
        CGeoLineLineIntersection tmpInter(*this, p);
        CGeoPoint interPoint = tmpInter.IntersectPoint();
        return (IsPointOnLineOnSegment(interPoint) && p.IsPointOnLineOnSegment(interPoint));
    }
    CGeoPoint segmentsIntersectPoint(const CGeoSegment& p) const {
        CGeoLineLineIntersection tmpInter(*this, p);
        CGeoPoint interPoint = tmpInter.IntersectPoint();
        if (IsPointOnLineOnSegment(interPoint) && p.IsPointOnLineOnSegment(interPoint))
            return interPoint;
        else return CGeoPoint(9999, 9999);
    }
    double dist2Point(const CGeoPoint& p) {
        CGeoPoint tmpProj = projection(p);
        if (IsPointOnLineOnSegment(tmpProj)) return p.dist(tmpProj);
        else return std::min(_start.dist(p), _end.dist(p));
    }
    double dist2Segment(const CGeoSegment& s) {
        if (IsSegmentsIntersect(s)) return 0;
        else return std::min(dist2Point(s.point1()), dist2Point(s.point2()));
    }
    const CGeoPoint& start() const {
        return _start;
    }
    const CGeoPoint& end() const {
        return _end;
    }
    const CGeoPoint& center() {
        return _center;
    }

private:
    CGeoPoint _start;
    CGeoPoint _end;
    CGeoPoint _center;
    bool _compareX;
};

/************************************************************************/
/*                        CGeoShape                                     */
/************************************************************************/
class CGeoShape {
public:
    virtual ~CGeoShape() { }
    virtual bool HasPoint(const CGeoPoint& p) const = 0;
};

/************************************************************************/
/*                        CGeoQuadrilateral                             */
/************************************************************************/
class CGeoQuadrilateral : public CGeoShape {
public:
    CGeoQuadrilateral() :CGeoQuadrilateral(CGeoPoint(), CGeoPoint(), CGeoPoint(), CGeoPoint()) {}
    CGeoQuadrilateral(const CGeoPoint p1, const CGeoPoint p2, const CGeoPoint p3, const CGeoPoint p4);
    virtual bool HasPoint(const CGeoPoint& p) const;
    CGeoPoint _point[4];
    CGeoPoint _center;
};

/************************************************************************/
/*                        CGeoCircle                                    */
/************************************************************************/
class CGeoCirlce : public CGeoShape {
public:
    CGeoCirlce() { }
    CGeoCirlce(const CGeoPoint& c, double r) : _radius(r), _center(c) { }
    virtual bool HasPoint(const CGeoPoint& p) const;
    CGeoPoint Center() const {
        return _center;
    }
    double Radius() const {
        return _radius;
    }
    double Radius2() const {
        return _radius * _radius;
    }
private:
    double _radius;
    CGeoPoint _center;
};
/************************************************************************/
/*                        CGeoCircleTangent                             */
/************************************************************************/
class CGeoCircleTangent {
public:
    CGeoCircleTangent(const CGeoCirlce& circle, const CGeoPoint& p);
    const CGeoPoint& point1() const {
        return _point1;
    }
    const CGeoPoint& point2() const {
        return _point2;
    }
    int size() {
        return tangent_size;
    }
private:
    int tangent_size;
    CGeoPoint _point1;
    CGeoPoint _point2;
};
#endif
