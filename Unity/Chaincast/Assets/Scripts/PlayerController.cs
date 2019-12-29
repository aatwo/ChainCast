using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public float maxSpeed = 5f;

    // Automatically located objects
    Rigidbody2D rb;
    Transform playerDirectionPointer;

    int playerIndex = 0;
    float lastDirectionDegrees = 0.0f;

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
                playerDirectionPointer = t;
        }

        if( playerDirectionPointer  == null )
            Debug.LogError( "no direction pointer found in player children" );
    }

    // Update is called once per frame
    void Update()
    {
        // TODO: allow support for players 3 and 4
        if( playerIndex == 0 || playerIndex == 1 )
        {
            Vector2 inputVector = ProcessKeyboardInput();

            // If no keyboard movement then try gamepad
            if( inputVector.x == 0f && inputVector.y == 0f)
                ProcessGamepadInput();
        }
    }

    Vector2 ProcessKeyboardInput()
    {
        string horizontalAxisName = "Horizontal" + playerIndex + "_key";
        string verticalAxisName = "Vertical" + playerIndex + "_key";
        return ProcessAxisInput( horizontalAxisName, verticalAxisName );
    }

    Vector2 ProcessGamepadInput()
    {
        string horizontalAxisName = "Horizontal" + playerIndex + "_gamepad";
        string verticalAxisName = "Vertical" + playerIndex + "_gamepad";
        return ProcessAxisInput( horizontalAxisName, verticalAxisName );
    }

    Vector2 ProcessAxisInput(string horizontalName, string verticalName)
    {
        float horizontalInput = Input.GetAxisRaw( horizontalName );
        float verticalInput = Input.GetAxisRaw( verticalName );

        Vector2 normalisedInputVector = (new Vector2(horizontalInput, verticalInput)).normalized;
        float xVel = horizontalInput * maxSpeed * Mathf.Abs(normalisedInputVector.x);
        float yVel = verticalInput * maxSpeed * Mathf.Abs(normalisedInputVector.y);

        rb.velocity = new Vector2( xVel, yVel );

        // Only set the last direction if the input threshold is met (otherwise it snaps back to (0, 0))
        if( Mathf.Abs(horizontalInput) > 0.1f || Mathf.Abs(verticalInput) > 0.1f )
        {
            lastDirectionDegrees = CalculateDirectionAngle( rb.velocity );

            Quaternion rotation = playerDirectionPointer.rotation;
            rotation = Quaternion.Euler( 0f, 0f, -lastDirectionDegrees );
            playerDirectionPointer.rotation = rotation;
        }

        return new Vector2( horizontalInput, verticalInput );
    }

    float AngleBetweenVector2( Vector2 vec1, Vector2 vec2 )
    {
        Vector2 vec1Rotated90 = new Vector2(-vec1.y, vec1.x);
        float sign = (Vector2.Dot(vec1Rotated90, vec2) < 0) ? -1.0f : 1.0f;
        return Vector2.Angle( vec1, vec2 ) * sign;
    }

    float CalculateDirectionAngle( Vector2 vec )
    {
        Vector2 baseDirection = new Vector2(1f, 0f);
        return AngleBetweenVector2( vec, baseDirection );
    }
}
