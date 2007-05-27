// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_LINE_3D_H_INCLUDED__
#define __IRR_LINE_3D_H_INCLUDED__

#include "irrTypes.h"
#include "vector3d.h"

namespace irr
{
namespace core
{

//! 3D line between two points with intersection methods.
template <class T>
class line3d
{
	public:

		// Constructors

		line3d() : start(0,0,0), end(1,1,1) {};
		line3d(T xa, T ya, T za, T xb, T yb, T zb) : start(xa, ya, za), end(xb, yb, zb) {};
		line3d(const vector3d<T>& start, const vector3d<T>& end) : start(start), end(end) {};

		// operators

		line3d<T> operator+(const vector3d<T>& point) const { return line3d<T>(start + point, end + point); };
		line3d<T>& operator+=(const vector3d<T>& point) { start += point; end += point; return *this; };

		line3d<T> operator-(const vector3d<T>& point) const { return line3d<T>(start - point, end - point); };
		line3d<T>& operator-=(const vector3d<T>& point) { start -= point; end -= point; return *this; };

		bool operator==(const line3d<T>& other) const { return (start==other.start && end==other.end) || (end==other.start && start==other.end);};
		bool operator!=(const line3d<T>& other) const { return !(start==other.start && end==other.end) || (end==other.start && start==other.end);};

		// functions

		void setLine(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb) {start.set(xa, ya, za); end.set(xb, yb, zb);}
		void setLine(const vector3d<T>& nstart, const vector3d<T>& nend) {start.set(nstart); end.set(nend);}
		void setLine(const line3d<T>& line) {start.set(line.start); end.set(line.end);}

		//! Returns length of line
		//! \return Returns length of line.
		T getLength() const { return start.getDistanceFrom(end); };

		//! Returns sqared length of line
		//! \return Returns sqared length of line.
		T getLengthSQ() const { return start.getDistanceFromSQ(end); };

		//! Returns middle of line
		vector3d<T> getMiddle() const
		{
			return (start + end) * (T)0.5;
		}

		//! Returns vector of line
		vector3d<T> getVector() const
		{
			return end - start;
		}

		//! Returns if the given point is between start and end of the
		//! line. Assumes that the point is already somewhere on the line.
		bool isPointBetweenStartAndEnd(const vector3d<T>& point) const
		{
			return point.isBetweenPoints(start, end);
		}

		//! Returns the closest point on this line to a point
		vector3d<T> getClosestPoint(const vector3d<T>& point) const
		{
			vector3d<T> c = point - start;
			vector3d<T> v = end - start;
			T d = (T)v.getLength();
			v /= d;
			T t = v.dotProduct(c);

			if (t < (T)0.0)
				return start;
			if (t > d)
				return end;

			v *= t;
			return start + v;
		}

		//! Returns if the line intersects with a shpere
		//! \param sorigin: Origin of the shpere.
		//! \param sradius: Radius of the sphere.
		//! \param outdistance: The distance to the first intersection point.
		//! \return Returns true if there is an intersection.
		//! If there is one, the distance to the first intersection point
		//! is stored in outdistance.
		bool getIntersectionWithSphere(vector3d<T> sorigin, T sradius, f64& outdistance) const
		{
			const vector3d<T> q = sorigin - start;
			T c = q.getLength();
			T v = q.dotProduct(getVector().normalize());
			T d = sradius * sradius - (c*c - v*v);

			if (d < 0.0)
				return false;

			outdistance = v - sqrt(d);
			return true;
		}

		// member variables

		vector3d<T> start;
		vector3d<T> end;
};

	//! Typedef for an f32 line.
	typedef line3d<f32> line3df;
	//! Typedef for an integer line.
	typedef line3d<s32> line3di;

} // end namespace core
} // end namespace irr

#endif

