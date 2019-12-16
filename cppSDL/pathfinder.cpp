#include "pathfinder.h"
#include "utils.h"
#include "timer.h"
#include <algorithm>


PathFinder::PathFinder( int windowWidth, int windowHeight, int gridWidth, int gridHeight )
	:	mWindowWidth( windowWidth ),
		mWindowHeight( windowHeight ),
		mGridWidth( gridWidth ),
		mGridHeight( gridHeight ),
		mCellWidth( mWindowWidth / mGridWidth ),
		mCellHeight( mWindowHeight / mGridHeight )
{
	// Build the node list
	mNodes.resize( mGridWidth * mGridHeight );
	for( int i = 0; i < mNodes.size(); i++ )
	{
		mNodes[ i ] 		= new Node;
		mNodes[ i ]->index	= i;
		CoordinateFromIndex( i, mNodes[ i ]->x, mNodes[ i ]->y );
	}

	mOpenNodeList.reserve( mNodes.size() );
	mClosedNodeList.reserve( mNodes.size() );
}

PathFinder::~PathFinder()
{
	for( Node* node : mNodes )
		delete node;
}

void PathFinder::Render( SDL_Renderer* renderer )
{
	SDL_Rect rect;
	rect.w = mCellWidth;
	rect.h = mCellHeight;

	// Draw open nodes
	if( mFindingPath )
	{
		SDL_SetRenderDrawColor( renderer, 255, 191, 15, 150 );
		for( Node* node : mOpenNodeList )
		{
			rect.x = node->x * mCellWidth;
			rect.y = node->y * mCellHeight;
			SDL_RenderFillRect( renderer, &rect );
		}
	}

	SDL_SetRenderDrawColor( renderer, 185, 255, 112, 200 );
	
	// Draw current path
	for( Node* node : mPathToTarget )
	{
		rect.x = node->x * mCellWidth;
		rect.y = node->y * mCellHeight;
		SDL_RenderFillRect( renderer, &rect );
	}	

	// Draw start
	rect.x = mStartX * mCellWidth;
	rect.y = mStartY * mCellHeight;
	SDL_RenderFillRect( renderer, &rect );

	// Draw target
	rect.x = mTargetX * mCellWidth;
	rect.y = mTargetY * mCellHeight;
	SDL_RenderFillRect( renderer, &rect );

}

bool PathFinder::FindPath( int startX, int startY, int targetX, int targetY )
{
	mStartX 			= startX;
	mStartY 			= startY;
	mTargetX 			= targetX;
	mTargetY 			= targetY;
	mFindingPath 		= true;
	mFoundPath			= false;

	mPathToTarget.clear();
	mOpenNodeList.clear();
	mClosedNodeList.clear();

	// Add start square to open list
	mStartNode 				= GetNode( mStartX, mStartY );
	mStartNode->parent 		= nullptr;
	mStartNode->heuristic 	= utils::ManhattanDistanceBetweenPoints( mStartNode->x, mStartNode->y, mTargetX, mTargetY );
	mStartNode->pathCost 	= 0;
	mStartNode->pathScore	= mStartNode->pathCost + mStartNode->heuristic;
	mTargetNode 			= GetNode( mTargetX, mTargetY );
	mCurrentSearchNode 		= nullptr;

	// Add the startNode to the open node list to start things off
	mOpenNodeList.push_back( mStartNode );

	while( mFindingPath )
		Step();

	return mFoundPath;
}

int	PathFinder::GetPathDistance()
{
	if( mFindingPath || !mFoundPath )
		return -1;

	return mPathToTarget.size();
}

std::vector<SDL_Point> PathFinder::GetPath()
{
	std::vector<SDL_Point> ret;

	for( Node* node : mPathToTarget )
	{
		SDL_Point p;
		p.x = node->x;
		p.y = node->y;
		ret.push_back( p );
	}
	
	return ret;
}

void PathFinder::SetEntities( std::vector<Entity*>& entities )
{
	for( Node* node : mNodes )
		node->blocked = false;

	for( Entity* entity : entities )
	{
		if( entity->GetType() != Entity::Type::wall && entity->GetType() != Entity::Type::destructable_wall )
			continue;

		double x, y;
		entity->GetCenter( x, y );

		int row, column;
		utils::GridCoordinateFromScreenCoordinate( x, y, mCellWidth, mCellHeight, row, column );

		SetBlockedCell( row, column, true );
	}
}

void PathFinder::SetBlockedCell( int x, int y, bool blocked )
{
	int index = IndexFromCoordinate( x, y );
	mNodes[ index ]->blocked = blocked;
}

void PathFinder::Step()
{
	if( !mFindingPath )
		return;

	// Add start square to open list
	// Take lowest cost square from open list and move to closed list
	// Take all adjacent squares and for each
	//		if it is blocked or in the closed list ignore it
	//		Otherwise 
	//			if its not on the open list, add it to the open list if it's not already there and give it the current node as its parent
	//			else if it is on the open list, 

	// Take lowest cost node and add to closed list
	mCurrentSearchNode = TakeLowestCostNode( mOpenNodeList );

	if( mCurrentSearchNode == mTargetNode )
	{
		GenerateNodePath( mCurrentSearchNode, mPathToTarget );
		mFoundPath 		= true;
		mFindingPath 	= false;
		return;
	}

	else if( mCurrentSearchNode == nullptr )
	{
		mPathToTarget.clear();
		mFoundPath		= false;
		mFindingPath 	= false;
		return;
	}

	mClosedNodeList.push_back( mCurrentSearchNode );

	// Compute all adjacent nodes, set their parents and compute the path cost for them given their parent
	std::vector<Node*> adjacentNodes;
	GetAdjacentNodes( mCurrentSearchNode, adjacentNodes );
	for( Node* adjacentNode : adjacentNodes )
	{
		// Ignore blocked nodes or nodes that are in the closed list
		if( adjacentNode->blocked || ListContainsNode( adjacentNode, mClosedNodeList ) )
			continue;

		// If not on the open list we add it and compute its cost, heuristic and score
		if( !ListContainsNode( adjacentNode, mOpenNodeList ) )
		{
			// This is a new node so set it up and add it to the open list
			adjacentNode->parent 	= mCurrentSearchNode;
			adjacentNode->heuristic	= utils::ManhattanDistanceBetweenPoints( adjacentNode->x, adjacentNode->y, mTargetX, mTargetY );
			adjacentNode->pathCost 	= adjacentNode->parent->pathCost + mMovementCost;
			adjacentNode->pathScore	= adjacentNode->pathCost + adjacentNode->heuristic;
			mOpenNodeList.push_back( adjacentNode );
		}

		// If already in the open list we check to see if this new path cost is better than its existing path cost
		else
		{
			int newPathCost = mCurrentSearchNode->pathCost + 10;
			if( newPathCost < adjacentNode->pathCost )
			{
				// This path is better so recompute
				adjacentNode->parent 	= mCurrentSearchNode;
				adjacentNode->pathCost 	= newPathCost;
				adjacentNode->pathScore	= adjacentNode->pathCost + adjacentNode->heuristic;
			}
		}
	}

	GenerateNodePath( mCurrentSearchNode, mPathToTarget );
}

bool PathFinder::ListContainsNode( Node* node, std::vector<Node*>& nodeList )
{
	auto it = std::find( nodeList.begin(), nodeList.end(), node );
	if( it != nodeList.end() )
		return true;

	return false;
}

PathFinder::Node* PathFinder::TakeLowestCostNode( std::vector<Node*>& nodeList )
{
	// The lowest cost node is the node with the lowest F value (F = G + H)
	// 		G = the total cost to get from the start node to the node we are looking at
	//		H = the heuristic, which in our case is a rough estimate of the distance of the node we are looking at to the target

	Node* 	lowestCostNode 	= nullptr;
	int 	lowestCostIndex = -1;
	int		lowestCost		= -1;

	if( nodeList.size() )
	{
		lowestCostNode 	= nodeList.at( 0 );
		lowestCostIndex = 0;
		lowestCost		= lowestCostNode->pathScore;
	}

	for( int i = 1; i < nodeList.size(); i++ )
	{
		int nodeCost = nodeList[ i ]->pathScore;

		if( nodeCost >= lowestCost )
			continue;

		lowestCostNode 	= nodeList[ i ];
		lowestCostIndex	= i;
		lowestCost		= nodeCost;
		break;
	}

	if( lowestCostIndex != -1 )
		nodeList.erase( nodeList.begin() + lowestCostIndex );

	return lowestCostNode;
}

PathFinder::Node* PathFinder::GetNode( int x, int y )
{
	if( x < 0 || x >= mGridWidth )
		return nullptr;

	if( y < 0 || y >= mGridHeight )
		return nullptr;

	int index = IndexFromCoordinate( x, y );
	return mNodes[ index];
}

int PathFinder::IndexFromCoordinate( int x, int y )
{
	return ( y * mGridWidth ) + x;
}

void PathFinder::CoordinateFromIndex( int index, int& x_OUT, int& y_OUT )
{
	x_OUT = index % mGridWidth;
	y_OUT = ( index - x_OUT ) / mGridWidth;
}

void PathFinder::GetAdjacentNodes( Node* node, std::vector<Node*>& nodes_OUT )
{
	// This returns horizontally and vertically adjacent nodes only

	// Node to the left
	if( node->x > 0 )
	{
		int index = IndexFromCoordinate( ( node->x - 1 ), node->y );
		nodes_OUT.push_back( mNodes[ index ] );
	}

	// Node to the right
	if( node->x < ( mGridWidth - 1 ) )
	{
		int index = IndexFromCoordinate( ( node->x + 1 ), node->y );
		nodes_OUT.push_back( mNodes[ index ] );
	}

	// Node to the top
	if( node->y > 0 )
	{
		int index = IndexFromCoordinate( node->x, ( node->y - 1 ) );
		nodes_OUT.push_back( mNodes[ index ] );
	}

	// Node to the right
	if( node->y < ( mGridHeight - 1 ) )
	{
		int index = IndexFromCoordinate( node->x, ( node->y + 1 ) );
		nodes_OUT.push_back( mNodes[ index ] );
	}
}

void PathFinder::GenerateNodePath( Node* finalNode, std::vector<Node*>& nodeList_OUT )
{
	nodeList_OUT.clear();
	Node* node = finalNode;
	while( node )
	{
		nodeList_OUT.push_back( node );
		node = node->parent;
	}
}
