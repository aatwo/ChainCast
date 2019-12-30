using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestructableWallController : MonoBehaviour, ProjectileTarget
{
    public delegate void ObstacleAboutToDie(Transform t);
    public event ObstacleAboutToDie OnObstacleAboutToDie;

    public void HandleProjectileCollision( Transform t, Projectile p )
    {
        OnObstacleAboutToDie(transform);
        Destroy( this.gameObject );
    }
}
