using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestructableWallController : MonoBehaviour, ProjectileTarget
{

    public void HandleProjectileCollision( Transform t, Projectile p )
    {
        Destroy( this.gameObject );
    }


    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
