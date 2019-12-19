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
        if( playerIndex == 0 )
        {
            float horizontalInput = Input.GetAxis( "Horizontal" );
            float verticalInput = Input.GetAxis( "Vertical" );

            Vector2 normalisedInputVector = (new Vector2(horizontalInput, verticalInput)).normalized;
            float xVel = horizontalInput * maxSpeed * Mathf.Abs(normalisedInputVector.x);
            float yVel = verticalInput * maxSpeed * Mathf.Abs(normalisedInputVector.y);

            rb.velocity = new Vector2( xVel, yVel );
        }
    }
}
