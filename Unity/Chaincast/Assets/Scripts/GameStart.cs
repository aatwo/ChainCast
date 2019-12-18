using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class GameStart : MonoBehaviour
{
    [SerializeField]
    Tilemap floorTilemap;
    [SerializeField]
    Tile floorTile;

    [SerializeField]
    Tilemap environmentTilemap;
    [SerializeField]
    Tile wallTile;

    [SerializeField]
    Transform playerOne;
    Vector2Int playerOneSpawn = new Vector2Int(1, 1);

    private void Start()
    {
        GenerateLevel();
        SpawnPlayers();
    }

    void GenerateLevel()
    {
        int wallChancePercent = 30;
        for( int x = 0; x < Common.gameWidth; x++ )
        {
            bool isHorizontalEdge = (x == 0 || x == Common.gameWidth - 1);
            for( int y = 0; y < Common.gameWidth; y++ )
            {
                bool isVerticalEdge = (y == 0 || y == Common.gameHeight - 1);

                bool isPlayerSpawn = (x == playerOneSpawn.x && y == playerOneSpawn.y);

                // Place floor tile
                Vector3Int p = new Vector3Int( x, y, 0 );
                floorTilemap.SetTile( p, floorTile );

                if( isPlayerSpawn )
                    continue;

                // Place environment tile
                if( isHorizontalEdge || isVerticalEdge || Random.Range( 0, 100 ) < wallChancePercent )
                    environmentTilemap.SetTile( p, wallTile );
            }
        }
    }

    void SpawnPlayers()
    {
        // TODO: support multiple players
        Transform player = Instantiate(playerOne, GetTileCenterPos(1, 1), Quaternion.identity);        
    }

    Vector3 GetTileCenterPos(int x, int y)
    {
        Vector3Int cellPos = new Vector3Int(x, y, 0);
        return environmentTilemap.GetCellCenterWorld( cellPos );
    }
}
