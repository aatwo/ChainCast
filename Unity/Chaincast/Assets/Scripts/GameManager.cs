using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class GameManager : MonoBehaviour
{
    public static int gameWidth = 33;
    public static int gameHeight = 33;

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
    Transform monster;
    MonsterController monsterController;

    PathFinder pathFinder;
    float pathFinderInterval = 1f;
    float lastPathFinderTime = 0f;

    bool foundPath = false;
    List<Vector2Int> path = new List<Vector2Int>();
    List<Vector3> worldPath = new List<Vector3>();

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
        CalculateMonsterPath();
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

                    else if( Random.Range( 0, 100 ) < wallChancePercent && /* TEMP */ x != 1 /* END TEMP*/ )
                    {
                        // Create the breakable wall
                        Transform t = Instantiate( breakableWallPrefab, GetTileCenterPos( x, y ), Quaternion.identity, transform );
                        wallGridPositions.Add( new Vector2Int( x, y ) );

                        // Attach to the walls destruction event
                        DestructableWallController wallScript = t.gameObject.GetComponent<DestructableWallController>();
                        if( wallScript == null )
                            Debug.LogError("No script found on destructable wall object");

                        wallScript.OnObstacleAboutToDie += HandleObstacleAboutToDie;
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
        monster = Instantiate( monsterPrefab, spawnPos, Quaternion.identity, transform ) as Transform;
        monsterController = monster.GetComponent<MonsterController>();
        if( monsterController == null )
            Debug.LogError("No monster controller found on monster");
    }

    Vector3Int GetTileCellFromWorldPos(Vector3 pos)
    {
        return environmentTilemap.layoutGrid.WorldToCell( pos );
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

    void CalculateMonsterPath()
    {
        float timeSinceLastPathFindAttempt = Time.time - lastPathFinderTime;
        if( timeSinceLastPathFindAttempt >= pathFinderInterval )
        {
            Vector3Int monsterGridPos = GetTileCellFromWorldPos(monster.transform.position);
            Vector3Int playerGridPos = GetTileCellFromWorldPos(playerList[0].transform.position);
            foundPath = pathFinder.FindPath( monsterGridPos.x, monsterGridPos.y, playerGridPos.x, playerGridPos.y );
            if( foundPath )
            {
                path = pathFinder.GetLastPath();

                // Remove the first worldPath item if this is the cell the monster currently inhabits
                if( path.Count > 0 && path[0].x == monsterGridPos.x && path[0].y == monsterGridPos.y )
                    path.RemoveAt( 0 );

                worldPath = new List<Vector3>( path.Count );
                foreach( Vector2Int gridPos in path )
                {
                    Vector3 worldPos = GetTileCenterPos(gridPos.x, gridPos.y);
                    worldPath.Add( worldPos );
                }
            }
            lastPathFinderTime = Time.time;
        }

        if( foundPath )
        {
            // Pass the path (in world coordinates) to the monster
            monsterController.SetPath( ref worldPath );

            // Render the current path
            for( int i = 0; i <= path.Count - 2; i++ )
            {
                Vector3 posA = GetTileCenterPos( path[i].x, path[i].y );
                Vector3 posB = GetTileCenterPos( path[i + 1].x, path[i + 1].y );

                Debug.DrawLine( posA, posB, Color.red );
            }
        }
        else
        {
            monsterController.ClearPath();
        }
    }

    void HandleObstacleAboutToDie(Transform obstacleTransform)
    {
        Vector3Int gridPosition = GetTileCellFromWorldPos(obstacleTransform.position);
        pathFinder.RemoveObstaclePosition( gridPosition.x, gridPosition.y );
    }
}
