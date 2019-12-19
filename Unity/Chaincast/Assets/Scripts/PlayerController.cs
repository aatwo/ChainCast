using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public float maxSpeed = 5f;

    Rigidbody2D rb;
    int playerIndex = 0;

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
        float horizontalInput = Input.GetAxis( horizontalName );
        float verticalInput = Input.GetAxis( verticalName );

        Vector2 normalisedInputVector = (new Vector2(horizontalInput, verticalInput)).normalized;
        float xVel = horizontalInput * maxSpeed * Mathf.Abs(normalisedInputVector.x);
        float yVel = verticalInput * maxSpeed * Mathf.Abs(normalisedInputVector.y);

        rb.velocity = new Vector2( xVel, yVel );
        return new Vector2( horizontalInput, verticalInput );
    }
}
