using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TrackingRangeTriggerEarth : MonoBehaviour
{
    Vector3 playerPos;
    bool plyerFind;

    // Start is called before the first frame update
    void Start()
    {
        plyerFind = false;
    }
    void Update()
    {        
    }
    void OnTriggerStay2D(Collider2D other)
    {
        if(other.gameObject.tag == "Player")
        {
            playerPos = other.gameObject.transform.position;
            plyerFind = true;
        }
    }
    void OnTriggerExit2D(Collider2D other)
    {
        if(other.gameObject.tag == "Player")
            plyerFind = false;
    }

    public Vector3 GetPlayerPos()
    {
        return playerPos;
    }
    public bool GetPlayerFind()
    {
        return plyerFind;
    }
}
