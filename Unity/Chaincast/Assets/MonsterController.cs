using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MonsterController : MonoBehaviour, ProjectileTarget
{
    bool stunned = false;
    float stunDurationS = 2f;
    float lastStunTime = 0f;
    Transform frozenOverlay;

    public void HandleProjectileCollision( Transform t, Projectile p )
    {
        SetStunned( true );
    }

    // Start is called before the first frame update
    void Start()
    {
        foreach( Transform t in transform )
        {
            if( t.name == "FrozenOverlay" )
                frozenOverlay = t;
        }

        if( frozenOverlay == null )
            Debug.LogError( "no frozen overlay found in monster children" );
    }

    // Update is called once per frame
    void Update()
    {
        CheckStunned();
    }

    void SetStunned( bool isStunned )
    {
        if( isStunned )
        {
            stunned = true;
            lastStunTime = Time.time;
            frozenOverlay.gameObject.SetActive( true );
        }
        else
        {
            stunned = false;
            frozenOverlay.gameObject.SetActive( false );
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
