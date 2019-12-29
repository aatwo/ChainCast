﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Common : MonoBehaviour
{
    static public int gameWidth = 33;
    static public int gameHeight = 33;

    static public string destructableWallTag = "DestructableWall";

    public static float AngleBetweenTwoPoints( Vector2 vec1, Vector2 vec2 )
    {
        Vector2 vec1Rotated90 = new Vector2(-vec1.y, vec1.x);
        float sign = (Vector2.Dot(vec1Rotated90, vec2) < 0) ? -1.0f : 1.0f;
        return Vector2.Angle( vec1, vec2 ) * sign;
    }
}
