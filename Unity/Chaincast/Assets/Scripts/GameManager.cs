using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class GameManager : MonoBehaviour
{
    public const int gameWidth = 33;
    public const int gameHeight = 33;

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

    PathFinder pathFinder;

    float pathFinderInterval = 1f;
    float lastPathFinderTime = 0f;
    bool foundPath = false;
    List<Vector2Int> path;

    private void Start()
    {
        pathFinder = gameObject.AddComponent<PathFinder>();
        pathFinder.Init( gameWidth, gameHeight );
        CalculateSpawnLocations();
        GenerateLevel();
        SpawnPlayers();
        SpawnMonster();
    }

    private void Update()
    {
        float timeSinceLastPathFindAttempt = Time.time - lastPathFinderTime;
        Vector2Int startPos = new Vector2Int(1, 1);

        if( timeSinceLastPathFindAttempt >= pathFinderInterval )
        {
            Vector3Int playerGridPos = environmentTilemap.layoutGrid.WorldToCell(playerList[0].transform.position);
            foundPath = pathFinder.FindPath( startPos.x, startPos.y, playerGridPos.x, playerGridPos.y );
            path = null;
            if( foundPath )
                path = pathFinder.GetPath();
            lastPathFinderTime = Time.time;
            Debug.Log( "FOUND PATH: " + foundPath );
        }

        //Debug.DrawLine( GetTileCenterPos(startPos.x, startPos.y), GetTileCenterPos(endPos.x, endPos.y), Color.blue);

        if( foundPath )
        {
            for( int i = 0; i <= path.Count - 2; i++ )
            {
                Vector3 posA = GetTileCenterPos( path[i].x, path[i].y );
                Vector3 posB = GetTileCenterPos( path[i + 1].x, path[i + 1].y );

                Debug.DrawLine( posA, posB, Color.red );
            }
        }
    }

    void CalculateSpawnLocations()
    {
        playerSpawnLocations[0] = new Vector2Int( 1, 1 );
        playerSpawnLocations[1] = new Vector2Int( 1, gameHeight - 2 );
        playerSpawnLocations[2] = new Vector2Int( gameWidth - 2, gameHeight - 2 );
        playerSpawnLocations[3] = new Vector2Int( gameWidth - 2, 1 );

        monsterSpawnLocation = new Vector2Int( gameWidth / 2, gameHeight / 2 );
    }

    void GenerateLevel()
    {
        List<Vector2Int> wallGridPositions = new List<Vector2Int>( gameWidth * gameHeight );
        int wallChancePercent = 30;

        bool isXColumn = true;
        for( int x = 0; x < gameWidth; x++ )
        {
            bool isHorizontalEdge = (x == 0 || x == gameWidth - 1);

            bool isYColumn = true;
            for( int y = 0; y < gameHeight; y++ )
            {
                bool isVerticalEdge = (y == 0 || y == gameHeight - 1);

                // Place floor tile
                Vector3Int p = new Vector3Int( x, y, 0 );
                floorTilemap.SetTile( p, floorTile );

                bool isSpawnLocation = IsTileSpawnLocation( x, y );
                if( !isSpawnLocation )
                {
                    bool isColumn = (isXColumn && isYColumn);

                    if( isHorizontalEdge || isVerticalEdge || isColumn )
                    {
                        environmentTilemap.SetTile( p, wallTile );
                        wallGridPositions.Add( new Vector2Int( x, y ) );
                    }

                    // TEMP
                    //else if( x == 2 )
                    //{
                    //    Instantiate( breakableWallPrefab, GetTileCenterPos( x, y ), Quaternion.identity, transform );
                    //    wallGridPositions.Add( new Vector2Int( x, y ) );
                    //}
                    // END TEMP

                    else if( Random.Range( 0, 100 ) < wallChancePercent && /* TEMP */ x != 1 /* END TEMP*/ )
                    {
                        Instantiate( breakableWallPrefab, GetTileCenterPos( x, y ), Quaternion.identity, transform );
                        wallGridPositions.Add( new Vector2Int( x, y ) );
                    }
                }

                isYColumn = !isYColumn;
            }

            isXColumn = !isXColumn;
        }

        pathFinder.SetObstaclePositions( ref wallGridPositions );
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
