#include <iostream>
#include <vector>
struct Vertex { float x, y, z; };
struct Triangle
{
	Vertex a, b, c;
};


template <unsigned int Dim>
struct Point
{
	Point() { for (unsigned int i = 0; i < Dim; ++i) coords[i] = 0.0f; }
	float coords[Dim];
	constexpr unsigned int dimension() const { return Dim; }
};
template <unsigned int spaceDim, unsigned int vectDim>
struct NVector
{

};

template <unsigned int SpaceDimensions, unsigned int PrimitiveDimensions>
/*
	Examples:
		Triangle in 2D space: Primitive<2,3>
		Tetrahedron in 3D space: Primitive<3,4>
		Triangle in 3D space: Primitive<3,3>
*/
struct Primitive /* This is really a mesh primitive: e.g. you could have a tetrahedron in 3D, but you want it to be a 4D 'mesh-primitive' */
{
	Point<SpaceDimensions> points[PrimitiveDimensions];
	constexpr unsigned int coordinateLength() const { return SpaceDimensions; }
	constexpr unsigned int pointCount() const { return PrimitiveDimensions; }
};

//template <unsigned int Dim>
//struct Intersection
//{
//	/* Scalar Point in 1D, Line in 2D, Plane in 3D, Volume in 4D */
//	Primitive<Dim - 1,3> intersect(Primitive<Dim> prim)
//	{
//		static_assert(Dim > 1);
//
//
//		
//	}
//};

template <unsigned int Dim>
struct Mesh
{
	std::vector<Primitive<Dim,Dim>> elems;

	/*Mesh<Dim - 1> getMesh(Intersection<Dim-1> intersection)
	{
		Mesh<Dim - 1> result{};
		for (unsigned int i = 0; i < Dim; ++i)
		{

		}
	}*/


	Mesh<0> getMesh() { return Mesh<0>{}; }
};



/*

Description: {We don't actually care about 3D -> 2D but are trialling it to find the generic equation }

If I have a Primitive<3,3> triangle, whose points are in 3D space, and I have a plane, defined in 3D space that intersects it. I want a Primitive<2,2> line.
		-> What I want to find is the *bounded* line that is the part of the triangle that the plane interesects. { A Primitive<2,2> line or two 2D coordinates, oriented w.r.t the intersecting plane }
		There are two planes:
			The intersection plane (in 3D)
			The plane that the triangle is on (in 3D)
		This intersection produces a line, in 3D space (or a vector at the origin)
			I then want to use this 3D line (infinite) to find a Primitive<2,2> line by finding the point at which the line ends at each edge of the triangle.

		I want to generalise this process, such that I can find a set of Triangles (Primitive<3,3>) from a mesh of higher dimensional primitives and an intersection.

In General:
	-> I want to start with a mesh of Primitives<N,N>,
			-> And be able to produce a mesh of Primitives<3,3>, given an HPlane<N,3>.
			-> I need functions to find Primitives<N-1,N-1> given an Intersection<N,N-1>

From 11.13.2 in GACS:

	A line is formed from two points: line<3> = point1 ^ point2
	A plane is formed from 3 points: plane<3> = point1 ^ point2 ^ point3

	The pseudoscalar of the space is found with:
		mv I = unit_e(join(P1,P2));
	The intersection if found:
		mv = (P1 << inverse(I)) << P2;


		( Remember Meet = (B ¬ I_n^-1) ¬ a )
*/

//typedef Primitive<3> Triangle;
//typedef Primitive<4> Tetrahedron;


int main()
{
	Primitive<3,3> tri;
	tri.points[0].coords[0] = 1.0f;

	std::cout << "Triangle Coords:" << std::endl;

	for (unsigned i = 0; i < tri.coordinateLength(); ++i)
	{
		std::cout << "\tPoint " << i << std::endl;
		std::cout << "\t\tCoordinates: ";
		std::cout << "{";
		for (unsigned j = 0; j < tri.pointCount(); ++j)
		{
			std::cout << tri.points[i].coords[j];
			if (j != 2)
				std::cout << ", ";
		}
		std::cout << "}" << std::endl;
	}
}