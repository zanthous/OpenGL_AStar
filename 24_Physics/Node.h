#pragma once
#include <queue>
#include <limits>
#include <cmath>
namespace Simplex
{
	class Node
	{
	public:
		int idx;
		float cost;

		Node( int i, float c ) : idx( i ), cost( c ) {};

		friend bool operator<( const Node &n1, const Node &n2 )
		{
			return n1.cost > n2.cost;
		}

		friend bool operator==( const Node &n1, const Node &n2 )
		{
			return n1.idx == n2.idx;
		}

		static float heuristic( int i0, int j0, int i1, int j1 );
	};
}

