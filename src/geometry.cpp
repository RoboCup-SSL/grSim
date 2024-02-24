#include <geometry.h>
#include <algorithm>

/************************************************************************/
/*                        CVector                                       */
/************************************************************************/
CVector CVector::rotate(double angle) const
{
	double nowdir = dir() + angle;
	double nowx = mod() * cos(nowdir);
	double nowy = mod() * sin(nowdir);
	return CVector(nowx, nowy);
}

/************************************************************************/
/*                        CGeoLineLineIntersection                      */
/************************************************************************/
CGeoLineLineIntersection::CGeoLineLineIntersection(const CGeoLine& line_1, const CGeoLine& line_2)
{
	double d = line_1.a() * line_2.b() - line_1.b() * line_2.a();
	if (std::abs(d) < 0.0001) {
		_intersectant = false;
	} else {
		double px = (line_1.b() * line_2.c() - line_1.c() * line_2.b()) / d;
		double py = (line_1.c() * line_2.a() - line_1.a() * line_2.c()) / d;
		_point = CGeoPoint(px, py);
		_intersectant = true;
	}
}

/************************************************************************/
/*                        CGeoQuadrilateral                             */
/************************************************************************/
CGeoQuadrilateral::CGeoQuadrilateral(const CGeoPoint p1, const CGeoPoint p2, const CGeoPoint p3, const CGeoPoint p4)
	:_point{ p1,p2,p3,p4 }
{
	double px = 0, py = 0;
	// find a point p inside polygon: the average of the vertexes
	for (int i = 0; i < 4; i++) {
		px += _point[i].x();
		py += _point[i].y();
	}
	px /= 4.0;
	py /= 4.0;
	_center = CGeoPoint(px, py);
	std::sort(_point, _point + 4, [&](const CGeoPoint& p1, const CGeoPoint& p2)->bool {return (p1 - _center).dir() < (p2 - _center).dir(); });
}
bool CGeoQuadrilateral::HasPoint(const CGeoPoint& p) const
{
	CGeoSegment line_1(p, _center);
	for (int i = 0; i < 4; i++) {
		CGeoSegment line_2(_point[i], _point[(i + 1) % 4]);
		CGeoLineLineIntersection inter(line_1, line_2);
		if (inter.Intersectant()
			&& line_1.IsPointOnLineOnSegment(inter.IntersectPoint())
			&& line_2.IsPointOnLineOnSegment(inter.IntersectPoint())) {
			return false;
		}
	}
	return true;
}

/************************************************************************/
/*                        CGeoCirlce                                    */
/************************************************************************/
bool CGeoCirlce::HasPoint(const CGeoPoint& p) const
{
	double d = (p - _center).mod();
	if (d < _radius) {
		return true;
	}
	return false;
}

/************************************************************************/
/*                        CGeoCircleTangent                             */
/************************************************************************/
CGeoCircleTangent::CGeoCircleTangent(const CGeoCirlce& circle, const CGeoPoint& p)
{
	double d = (p - circle.Center()).mod();
	if (fabs(d - circle.Radius()) < 1e-10) {
		tangent_size = 1;
		_point1 = p;
		_point2 = CGeoPoint();
	} else if (d < circle.Radius()) {
		tangent_size = 0;
		_point1 = CGeoPoint();
		_point2 = CGeoPoint();
	} else {
		tangent_size = 2;

		double angle = acos(circle.Radius() / d);
		double dir_p = (p - circle.Center()).dir();

		_point1 = circle.Center() + CVector(circle.Radius() * cos(dir_p + angle), circle.Radius() * sin(dir_p + angle));
		_point2 = circle.Center() + CVector(circle.Radius() * cos(dir_p - angle), circle.Radius() * sin(dir_p - angle));
	}
}