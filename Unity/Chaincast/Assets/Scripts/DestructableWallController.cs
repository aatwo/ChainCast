using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestructableWallController : MonoBehaviour, ProjectileTarget
{

    public void HandleProjectileCollision( Transform t, Projectile p )
    {
        Destroy( this.gameObject );
    }
}
