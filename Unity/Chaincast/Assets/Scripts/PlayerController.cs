using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public float maxSpeed = 5f;

    Rigidbody2D rb;


    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
        if( rb == null )
            Debug.LogError( "no rigid body in player" );
    }

    private void FixedUpdate()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        float horizontalInput = Input.GetAxis( "Horizontal" );
        float verticalInput = Input.GetAxis( "Vertical" );

        Vector2 inputVector = new Vector2(horizontalInput, verticalInput);
        Vector2 speedVector = inputVector * maxSpeed;

        Vector2 normalisedInputVector = inputVector.normalized;
        Vector2 fixedSpeedVector = new Vector2(speedVector.x * Mathf.Abs(normalisedInputVector.x), speedVector.y * Mathf.Abs(normalisedInputVector.y));

        rb.velocity = fixedSpeedVector;
    }
}
