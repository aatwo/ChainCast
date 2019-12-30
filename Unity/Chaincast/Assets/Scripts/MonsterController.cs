using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MonsterController : MonoBehaviour, ProjectileTarget
{
    float xSpeed = 4f;
    float ySpeed = 4f;
    float maxTimeSpentChasingTarget = 10f;

    bool stunned = false;
    float stunDurationS = 2f;
    float lastStunTime = 0f;
    Transform frozenOverlay;
    Rigidbody2D rb;

    float currentTargetStartTime = 0f;
    List<Vector3> currentPath;
    Vector3 currentTarget;
    bool gotTarget = false;

    public void ClearPath()
    {
        currentPath.Clear();
        gotTarget = false;
    }

    public void SetPath(ref List<Vector3> path )
    {
        // Remove the first path item since thats our current location
        currentPath = path;

        gotTarget = ( currentPath.Count > 0 );
        if( gotTarget )
            currentTarget = currentPath[0];
    }

    public void HandleProjectileCollision( Transform t, Projectile p )
    {
        SetStunned( true );
    }

    // Start is called before the first frame update
    void Start()
    {
        foreach( Transform t in transform )
        {
            if( t.name == "FrozenOverlay" )
                frozenOverlay = t;
        }

        if( frozenOverlay == null )
            Debug.LogError( "no frozen overlay found in monster children" );

        rb = GetComponent<Rigidbody2D>();
        if( rb == null )
            Debug.LogError( "no rigid body found on monster" );

        currentPath = new List<Vector3>();
        currentTarget = new Vector3();
    }

    // Update is called once per frame
    void Update()
    {
        CheckStunned();
        if( !stunned )
            MoveTowardsTarget();
    }

    void SetStunned( bool isStunned )
    {
        if( isStunned )
        {
            stunned = true;
            lastStunTime = Time.time;
            frozenOverlay.gameObject.SetActive( true );
            rb.velocity = new Vector2( 0f, 0f );
        }
        else
        {
            stunned = false;
            frozenOverlay.gameObject.SetActive( false );
        }
    }

    void CheckStunned()
    {
        float stunDurationS = 2f;
        if( stunned )
        {
            float timeStunned = Time.time - lastStunTime;
            if( timeStunned >= stunDurationS )
            {
                SetStunned( false );
            }
        }
    }

    void MoveTowardsTarget()
    {
        if( gotTarget )
        {
            float tolerance = 0.05f;

            float xVel = 0f;
            float yVel = 0f;

            float xDist = Mathf.Abs(transform.position.x - currentTarget.x);
            float yDist = Mathf.Abs(transform.position.y - currentTarget.y);

            if( xDist <= tolerance && yDist <= tolerance )
            {
                currentPath.RemoveAt( 0 );
                gotTarget = ( currentPath.Count > 0 );
                if( gotTarget )
                    currentTarget = currentPath[0];
            }
            else
            {
                // Move towards X
                if( xDist > tolerance )
                {
                    if( transform.position.x < currentTarget.x )
                        xVel = xSpeed;
                    else
                        xVel = -xSpeed;
                }

                // Move towards Y
                if( yDist > tolerance )
                {
                    if( transform.position.y < currentTarget.y )
                        yVel = ySpeed;
                    else
                        yVel = -ySpeed;
                }
            }

            rb.velocity = new Vector2( xVel, yVel );
        }
    }
}
