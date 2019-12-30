using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public interface Projectile
{
    int GetProjectileDamage();
    Common.ProjectileType GetProjectileType();
}

public interface ProjectileTarget
{
    void HandleProjectileCollision( Transform t, Projectile p );
}

public class Common : MonoBehaviour
{
    //static public int gameWidth = 33;
    //static public int gameHeight = 33;

    static public string destructableWallTag = "DestructableWall";
    public enum ProjectileType
    {
        standard
    };

    public static float AngleBetweenTwoPoints( Vector2 vec1, Vector2 vec2 )
    {
        Vector2 vec1Rotated90 = new Vector2(-vec1.y, vec1.x);
        float sign = (Vector2.Dot(vec1Rotated90, vec2) < 0) ? -1.0f : 1.0f;
        return Vector2.Angle( vec1, vec2 ) * sign;
    }

    public static int ManhattanDistanceBetweenPoints( int x1, int y1, int x2, int y2 )
    {
        int deltaX = Mathf.Abs( x1 - x2 );
        int deltaY = Mathf.Abs( y1 - y2 );

        return deltaX + deltaY;
    }
}
