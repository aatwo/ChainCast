using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ProjectileController : MonoBehaviour, Projectile
{
    public float lifeSpanS = 2f;
    float creationTime = 0f;

    public int GetProjectileDamage()
    {
        return 0;
    }

    public Common.ProjectileType GetProjectileType()
    {
        return Common.ProjectileType.standard;
    }

    private void Start()
    {
        creationTime = Time.time;
    }

    private void Update()
    {
        float aliveDurationS = Time.time - creationTime;
        if( aliveDurationS > lifeSpanS )
            DestroySelf();
    }

    private void OnCollisionEnter2D( Collision2D collision )
    {
        HandleCollision( collision );
    }

    private void OnCollisionStay2D( Collision2D collision )
    {
        HandleCollision( collision );
    }

    void HandleCollision( Collision2D collision )
    {
        ProjectileTarget t = collision.gameObject.GetComponentInChildren<ProjectileTarget>();
        if( t != null )
        {
            t.HandleProjectileCollision( transform, this );
            DestroySelf();
        }
    }

    void DestroySelf()
    {
        // TODO: play some kind of animation? Maybe a juicy particle effect?
        Destroy( gameObject);
    }
}
