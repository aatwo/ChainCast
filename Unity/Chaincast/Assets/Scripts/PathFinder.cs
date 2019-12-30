using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PathFinder : MonoBehaviour
{
    class Node
    {
        public int index = -1;
        public Vector2Int pos;
        public int heuristic = 0;
        public int pathCost = 0;
        public int pathScore = 0;
        public bool blocked = false;
        public int parent = -1;
    };


    public void Init( int gridWidth, int gridHeight )
    {
        mGridWidth = gridWidth;
        mGridHeight = gridHeight;
        mCellCount = mGridWidth * mGridHeight;

        mNodes = new Node[mCellCount];
        for( int i = 0; i < mNodes.Length; i++ )
        {
            mNodes[i] = new Node();
            mNodes[i].index = i;
            mNodes[i].pos = CoordinateFromIndex( i );
        }

        mOpenNodeList = new List<int>(mCellCount);
        mClosedNodeList = new List<int>(mCellCount);
        mLastPathToTarget = new List<int>( 0 );
    }

    public bool FindPath( int startX, int startY, int targetX, int targetY )
    {
        mStartX = startX;
        mStartY = startY;
        mTargetX = targetX;
        mTargetY = targetY;
        mFindingPath = true;
        mFoundPath = false;

        mLastPathToTarget.Clear();
        mOpenNodeList.Clear();
        mClosedNodeList.Clear();

        // Add start square to open list
        mStartNode = IndexFromCoordinate( mStartX, mStartY );
        mNodes[mStartNode].parent = -1;
        mNodes[mStartNode].heuristic = Common.ManhattanDistanceBetweenPoints( mNodes[mStartNode].pos.x, mNodes[mStartNode].pos.y, mTargetX, mTargetY );
        mNodes[mStartNode].pathCost = 0;
        mNodes[mStartNode].pathScore = mNodes[mStartNode].pathCost + mNodes[mStartNode].heuristic;
        mTargetNode = IndexFromCoordinate( mTargetX, mTargetY );
        mCurrentSearchNode = -1;

        // Add the startNode to the open node list to start things off
        mOpenNodeList.Add( mStartNode );

        while( mFindingPath )
            Step();

        return mFoundPath;
    }

    public int GetPathDistance()
    {
        if( mFindingPath || !mFoundPath )
            return -1;

        return mLastPathToTarget.Count;
    }

    public List<Vector2Int> GetPath()
    {
        List<Vector2Int> ret = new List<Vector2Int>(mLastPathToTarget.Count);

        foreach( int index in mLastPathToTarget )
        {
            ref Node n = ref mNodes[index];
            ret.Add( mNodes[index].pos );
        }

        return ret;
    }

    public void SetObstaclePositions( ref List<Vector2Int> positions )
    {
        for( int i = 0; i < mNodes.Length; i++ )
        {
            mNodes[i].blocked = false;
        }

        foreach( Vector2Int blockedPos in positions )
        {
            int nodeIndex = IndexFromCoordinate(blockedPos.x, blockedPos.y);
            mNodes[nodeIndex].blocked = true;
        }
    }

    public void AddObstaclePosition( int x, int y)
    {
        int index = IndexFromCoordinate( x, y );
        mNodes[index].blocked = true;
    }

    public void RemoveObstaclePosition( int x, int y)
    {
        int index = IndexFromCoordinate( x, y );
        mNodes[index].blocked = false;
    }

    void Step()
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
        mCurrentSearchNode = TakeLowestCostNode( ref mOpenNodeList );

        if( mCurrentSearchNode == mTargetNode )
        {
            GenerateNodePath( mCurrentSearchNode );
            mFoundPath = true;
            mFindingPath = false;
            return;
        }

        else if( mCurrentSearchNode == -1 )
        {
            mLastPathToTarget.Clear();
            mFoundPath = false;
            mFindingPath = false;
            return;
        }

        mClosedNodeList.Add( mCurrentSearchNode );

        // Compute all adjacent nodes, set their parents and compute the path cost for them given their parent
        List<int> adjacentNodes = GetAdjacentNodes( mNodes[mCurrentSearchNode].pos.x, mNodes[mCurrentSearchNode].pos.y );
        foreach( int adjacentNode in adjacentNodes )
        {
            // Ignore blocked nodes or nodes that are in the closed list
            ref Node n = ref mNodes[adjacentNode];
            if( mNodes[adjacentNode].blocked || ListContainsIndex( adjacentNode, ref mClosedNodeList ) )
                continue;
            
            // If not on the open list we add it and compute its cost, heuristic and score
            if( !ListContainsIndex( adjacentNode, ref mOpenNodeList ) )
            {
                // This is a new node so set it up and add it to the open list
                mNodes[adjacentNode].parent = mCurrentSearchNode;
                mNodes[adjacentNode].heuristic = Common.ManhattanDistanceBetweenPoints( mNodes[adjacentNode].pos.x, mNodes[adjacentNode].pos.y, mTargetX, mTargetY );
                mNodes[adjacentNode].pathCost = mNodes[mNodes[adjacentNode].parent].pathCost + mMovementCost;
                mNodes[adjacentNode].pathScore = mNodes[adjacentNode].pathCost + mNodes[adjacentNode].heuristic;
                mOpenNodeList.Add( adjacentNode );
            }

            // If already in the open list we check to see if this new path cost is better than its existing path cost
            else
            {
                int newPathCost = mNodes[mCurrentSearchNode].pathCost + 10;
                if( newPathCost < mNodes[adjacentNode].pathCost )
                {
                    // This path is better so recompute
                    mNodes[adjacentNode].parent = mCurrentSearchNode;
                    mNodes[adjacentNode].pathCost = newPathCost;
                    mNodes[adjacentNode].pathScore = mNodes[adjacentNode].pathCost + mNodes[adjacentNode].heuristic;
                }
            }
        }

        GenerateNodePath( mCurrentSearchNode );
    }

    bool ListContainsIndex( int index, ref List<int> indexList )
    {
        return indexList.Contains( index );
    }

    int TakeLowestCostNode( ref List<int> nodeList )
    {
        // The lowest cost node is the node with the lowest F value (F = G + H)
        // 		G = the total cost to get from the start node to the node we are looking at
        //		H = the heuristic, which in our case is a rough estimate of the distance of the node we are looking at to the target

        int lowestCostNodeIndex = -1;
        int lowestCostIndex = -1;
        int lowestCost = -1;

        if( nodeList.Count > 0 )
        {
            lowestCostNodeIndex = nodeList[0];
            lowestCostIndex = 0;
            lowestCost = mNodes[lowestCostIndex].pathScore;
        }

        for( int i = 1; i < nodeList.Count; i++ )
        {
            int nodeCost = mNodes[nodeList[ i ]].pathScore;
            if( nodeCost < lowestCost )
            {
                lowestCostNodeIndex = nodeList[i];
                lowestCostIndex = i;
                lowestCost = nodeCost;
            }
        }

        if( lowestCostIndex != -1 )
            nodeList.RemoveRange(0, lowestCostIndex  + 1);

        return lowestCostNodeIndex;
    }

    int IndexFromCoordinate( int x, int y )
    {
        return ( y * mGridWidth ) + x;
    }

    Vector2Int CoordinateFromIndex( int index )
    {
        int x = index % mGridWidth;
        int y = ( index - x ) / mGridWidth;
        return new Vector2Int( x, y );
    }

    List<int> GetAdjacentNodes( int x, int y )
    {
        // This returns horizontally and vertically adjacent nodes only
        List<int> ret = new List<int>();

        // Node to the left
        if( x > 0 )
        {
            int index = IndexFromCoordinate( ( x - 1 ), y );
            ret.Add( index );
        }

        // Node to the right
        if( x < ( mGridWidth - 1 ) )
        {
            int index = IndexFromCoordinate( ( x + 1 ), y );
            ret.Add( index );
        }

        // Node above
        if( y > 0 )
        {
            int index = IndexFromCoordinate( x, ( y - 1 ) );
            ret.Add( index );
        }

        // Node below
        if( y < ( mGridHeight - 1 ) )
        {
            int index = IndexFromCoordinate( x, ( y + 1 ) );
            ret.Add( index );
        }

        return ret;
    }

    void GenerateNodePath( int finalNodeIndex )
    {
        mLastPathToTarget.Clear();
        if( finalNodeIndex == -1 )
            return;

        Node node = mNodes[finalNodeIndex];
        while( node != null )
        {
            mLastPathToTarget.Add( node.index );

            if( node.parent == -1 )
                node = null;
            else
                node = mNodes[node.parent];
        }
    }

    Node[] mNodes;
    List<int> mLastPathToTarget;
    List<int> mOpenNodeList;
    List<int> mClosedNodeList;
    int mStartNode;
    int mTargetNode;
    int mCurrentSearchNode;

    bool mFoundPath = false;
    bool mFindingPath = false;
    int mTargetX = -1;
    int mTargetY = -1;
    int mStartX = -1;
    int mStartY = -1;

    int mMovementCost = 10;
    int mGridWidth;
    int mGridHeight;
    int mCellCount;
}
