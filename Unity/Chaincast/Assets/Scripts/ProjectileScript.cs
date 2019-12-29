using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ProjectileScript : MonoBehaviour
{
    static int value = 0;

    private void OnCollisionEnter2D( Collision2D collision )
    {
        HandleCollision( collision );
    }

    private void OnCollisionStay2D( Collision2D collision )
    {
        HandleCollision( collision );
    }

    void HandleCollision(Collision2D collision)
    {
        // TODO: handle
        if( collision.gameObject.tag == Common.destructableWallTag )
        {
            Destroy( collision.gameObject );
        }

        DestroySelf();
    }

    void DestroySelf()
    {
        // TODO: play some kind of animation? Maybe a juicy particle effect?
        Destroy( gameObject );
    }
}
