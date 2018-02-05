#ifndef _PATH_FINDER_H_
#define _PATH_FINDER_H_


#include <SDL2/SDL.h>
#include <vector>
#include <utility>
#include "entity.h"


class PathFinder
{
	public:

												PathFinder( int windowWidth, int windowHeight, int gridWidth, int gridHeight );
												~PathFinder();


		void									Render( SDL_Renderer* renderer );
		bool									FindPath( int startX, int startY, int targetX, int targetY );
		int										GetPathDistance();
		std::vector<SDL_Point>					GetPath();
		void									SetEntities( std::vector<Entity*>& entities );
		void									SetBlockedCell( int x, int y, bool blocked );


	private:

		struct Node
		{
			int 	index		= -1;
			int 	x			= -1;
			int 	y 			= -1;
			int 	heuristic 	= 0;
			int		pathCost	= 0;
			int 	pathScore	= 0;
			bool 	blocked		= false;
			Node* 	parent		= nullptr;
		};


		void									Step();
		bool									ListContainsNode( Node* node, std::vector<Node*>& nodeList );
		Node*									TakeLowestCostNode( std::vector<Node*>& nodeList );
		Node*									GetNode( int x, int y );
		int 									IndexFromCoordinate( int x, int y );
		void									CoordinateFromIndex( int index, int& x_OUT, int& y_OUT );
		void									GetAdjacentNodes( Node* node, std::vector<Node*>& nodes_OUT );
		void									GenerateNodePath( Node* finalNode, std::vector<Node*>& nodeList_OUT );		


		std::vector<Node*>						mNodes;
		std::vector<Node*>						mPathToTarget;
		std::vector<Node*>						mOpenNodeList;
		std::vector<Node*>						mClosedNodeList;
		Node*									mStartNode			= nullptr;
		Node*									mTargetNode			= nullptr;
		Node*									mCurrentSearchNode	= nullptr;

		bool									mFoundPath			= false;
		bool									mFindingPath 		= false;
		int										mTargetX 			= -1;
		int										mTargetY 			= -1;
		int 									mStartX				= -1;
		int 									mStartY				= -1;

		const int 								mMovementCost 		= 10;
		const int								mWindowWidth;
		const int								mWindowHeight;
		const int								mGridWidth;
		const int								mGridHeight;
		const int								mCellWidth;
		const int								mCellHeight;


}; // PathFinder


#endif // _PATH_FINDER_H_
