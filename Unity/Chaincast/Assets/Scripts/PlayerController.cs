using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour, ProjectileTarget
{
    public float maxSpeed = 5f;
    public float projectileSpeed = 10f;
    public float reloadDurationS = 2f;
    public Transform ProjectilePrefab;

    // Automatically located objects
    Rigidbody2D rb;
    Transform projectileSpawn;
    Transform playerDirectionPointer;
    Transform playerFrozenOverlay;

    int playerIndex = 0;
    float lastDirectionDegrees = 0.0f;
    Vector2 lastDirectionVector = new Vector2(1f, 0f);
    float lastFireTime = 0f;

    bool stunned = false;
    float stunDurationS = 2f;
    float lastStunTime = 0f;

    public void HandleProjectileCollision( Transform t, Projectile p )
    {
        SetStunned( true );
    }

    public void SetPlayerIndex(int n)
    {
        playerIndex = n;
    }

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
        if( rb == null )
            Debug.LogError( "no rigid body in player" );

        foreach( Transform t in transform )
        {
            if( t.name == "DirectionPointer" )
            {
                playerDirectionPointer = t;

                foreach( Transform dp_t in t )
                {
                    if( dp_t.name == "ProjectileSpawn" )
                        projectileSpawn = dp_t;
                }
            }

            else if( t.name == "FrozenOverlay" )
                playerFrozenOverlay = t;
        }

        if( playerDirectionPointer  == null )
            Debug.LogError( "no direction pointer found in player children" );

        if( projectileSpawn == null )
            Debug.LogError( "no projectile spawn found in player children" );

        if( playerFrozenOverlay == null )
            Debug.LogError( "no player frozen overlay found in player children" );
    }

    // Update is called once per frame
    void Update()
    {
        // TODO: allow support for players 2, 3 and 4
        if( playerIndex == 0 )
        {
            if( !stunned )
            {
                ProcessMovementInput();
                ProcessGunfireInput();
            }
        }

        CheckStunned();
    }

    void ProcessMovementInput()
    {
        Vector2 inputVector = ProcessKeyboardMovementInput();

        // If no keyboard movement then try gamepad
        if( inputVector.x == 0f && inputVector.y == 0f )
            ProcessGamepadMovementInput();
    }

    void ProcessGunfireInput()
    {
        float timeSinceLastShot = Time.time - lastFireTime;
        if( timeSinceLastShot >= reloadDurationS )
        {
            string name = "Fire" + playerIndex;
            bool fireButtonPressed = Input.GetButton( name );
            if(fireButtonPressed)
            {
                lastFireTime = Time.time;
                Transform projectile = Instantiate(ProjectilePrefab, projectileSpawn.position, Quaternion.identity, transform);
                Rigidbody2D rb = projectile.GetComponent<Rigidbody2D>();
                if( rb == null )
                    Debug.LogError( "Unable to find rigid body in player projectile" );

                rb.velocity = projectileSpeed * lastDirectionVector;
            }
        }
    }

    Vector2 ProcessKeyboardMovementInput()
    {
        string horizontalAxisName = "Horizontal" + playerIndex + "_key";
        string verticalAxisName = "Vertical" + playerIndex + "_key";

        float horizontalInput = Input.GetAxisRaw( horizontalAxisName );
        float verticalInput = Input.GetAxisRaw( verticalAxisName );

        return ProcessAxisInput( horizontalInput, verticalInput );
    }

    Vector2 ProcessGamepadMovementInput()
    {
        string horizontalAxisName = "Horizontal" + playerIndex + "_gamepad";
        string verticalAxisName = "Vertical" + playerIndex + "_gamepad";

        float horizontalInput = Input.GetAxisRaw( horizontalAxisName );
        float verticalInput = Input.GetAxisRaw( verticalAxisName );

        return ProcessAxisInput( horizontalInput, verticalInput );
    }

    Vector2 ProcessAxisInput( float horizontalInput, float verticalInput )
    {
        Vector2 normalisedInputVector = (new Vector2(horizontalInput, verticalInput)).normalized;
        float xVel = horizontalInput * maxSpeed * Mathf.Abs(normalisedInputVector.x);
        float yVel = verticalInput * maxSpeed * Mathf.Abs(normalisedInputVector.y);

        rb.velocity = new Vector2( xVel, yVel );

        // Only set the last direction if the input threshold is met (otherwise it snaps back to (0, 0))
        if( Mathf.Abs(horizontalInput) > 0.1f || Mathf.Abs(verticalInput) > 0.1f )
        {
            lastDirectionDegrees = CalculateDirectionAngle( rb.velocity );
            lastDirectionVector = normalisedInputVector;

            Quaternion rotation = playerDirectionPointer.rotation;
            rotation = Quaternion.Euler( 0f, 0f, -lastDirectionDegrees );
            playerDirectionPointer.rotation = rotation;
        }

        return new Vector2( horizontalInput, verticalInput );
    }

    float CalculateDirectionAngle( Vector2 vec )
    {
        Vector2 baseDirection = new Vector2(1f, 0f);
        return Common.AngleBetweenTwoPoints( vec, baseDirection );
    }

    void SetStunned( bool isStunned )
    {
        if( isStunned )
        {
            stunned = true;
            lastStunTime = Time.time;
            playerFrozenOverlay.gameObject.SetActive( true );
        }
        else
        {
            stunned = false;
            playerFrozenOverlay.gameObject.SetActive( false );
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
}
