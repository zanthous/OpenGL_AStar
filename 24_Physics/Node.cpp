#include "Node.h"
namespace Simplex
{
	float Node::heuristic( int i0, int j0, int i1, int j1 )
	{
		return std::abs( i0 - i1 ) + std::abs( j0 - j1 );
	}
}