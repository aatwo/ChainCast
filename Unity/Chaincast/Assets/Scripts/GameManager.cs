using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class GameManager : MonoBehaviour
{
    [SerializeField] Transform mainCamera;

    [SerializeField] Tilemap floorTilemap;
    [SerializeField] Tile floorTile;

    [SerializeField] Tilemap environmentTilemap;
    [SerializeField] Tile wallTile;
    [SerializeField] Transform breakableWallPrefab;

    [SerializeField] Transform playerPrefab;
    [SerializeField] Transform monsterPrefab;

    Vector2Int[] playerSpawnLocations = new Vector2Int[4];
    Vector2Int monsterSpawnLocation;
    List<PlayerController> playerList = new List<PlayerController>();

    private void Start()
    {
        CalculateSpawnLocations();
        GenerateLevel();
        SpawnPlayers();
        SpawnMonster();
    }

    void CalculateSpawnLocations()
    {
        playerSpawnLocations[0] = new Vector2Int( 1, Common.gameHeight - 2 );
        playerSpawnLocations[1] = new Vector2Int( 1, 1 );
        playerSpawnLocations[2] = new Vector2Int( Common.gameWidth - 2, Common.gameHeight - 2 );
        playerSpawnLocations[3] = new Vector2Int( Common.gameWidth - 2, 1 );

        monsterSpawnLocation = new Vector2Int( Common.gameWidth / 2, Common.gameHeight / 2 );
    }

    void GenerateLevel()
    {
        int wallChancePercent = 30;

        bool isXColumn = true;
        for( int x = 0; x < Common.gameWidth; x++ )
        {
            bool isHorizontalEdge = (x == 0 || x == Common.gameWidth - 1);

            bool isYColumn = true;
            for( int y = 0; y < Common.gameHeight; y++ )
            {
                bool isVerticalEdge = (y == 0 || y == Common.gameHeight - 1);

                // Place floor tile
                Vector3Int p = new Vector3Int( x, y, 0 );
                floorTilemap.SetTile( p, floorTile );

                bool isSpawnLocation = IsTileSpawnLocation( x, y );
                if( !isSpawnLocation )
                {
                    bool isColumn = (isXColumn && isYColumn);
                    if( isHorizontalEdge || isVerticalEdge || isColumn )
                        environmentTilemap.SetTile( p, wallTile );

                    else if( Random.Range( 0, 100 ) < wallChancePercent )
                        Instantiate( breakableWallPrefab, GetTileCenterPos( x, y ), Quaternion.identity, transform );
                }

                isYColumn = !isYColumn;
            }

            isXColumn = !isXColumn;
        }
    }

    void SpawnPlayers()
    {
        // TODO: support multiple players

        for( int i = 0; i < 4; i++ )
        {
            Transform player = Instantiate(playerPrefab, GetPlayerSpawnPos(i), Quaternion.identity);
            PlayerController playerController = player.GetComponent<PlayerController>();
            if( playerController == null )
                Debug.LogError( "No player controller script found on player" );

            playerController.SetPlayerIndex( i );
            playerList.Add( playerController );
        }

        // Attach camera to player one
        mainCamera.parent = playerList[0].transform;
        mainCamera.transform.localPosition = new Vector3( 0f, 0f, mainCamera.transform.localPosition.z );

        // TODO: bind a controller to this player?
    }

    void SpawnMonster()
    {
        Vector3 spawnPos = GetTileCenterPos(monsterSpawnLocation.x, monsterSpawnLocation.y);
        Instantiate( monsterPrefab, spawnPos, Quaternion.identity, transform );
    }

    Vector3 GetTileCenterPos(int x, int y)
    {
        Vector3Int cellPos = new Vector3Int(x, y, 0);
        return environmentTilemap.GetCellCenterWorld( cellPos );
    }
    Vector3 GetPlayerSpawnPos( int index )
    {
        Vector3 pos = GetTileCenterPos(playerSpawnLocations[index].x, playerSpawnLocations[index].y);
        return new Vector3(pos.x, pos.y, 0f);
    }
    bool IsTileSpawnLocation(int x, int y)
    {
        for( int i = 0; i < playerSpawnLocations.Length; i++ )
        {
            if( playerSpawnLocations[i].x == x && playerSpawnLocations[i].y == y )
                return true;
        }

        if( monsterSpawnLocation.x == x && monsterSpawnLocation.y == y )
            return true;

        return false;
    }
}
