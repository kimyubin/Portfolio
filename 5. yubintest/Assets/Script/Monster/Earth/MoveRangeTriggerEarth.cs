using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveRangeTriggerEarth : MonoBehaviour
{
    bool inMoveTrigger; //true 직진, false 유턴(절벽)
    // Start is called before the first frame update
    void Start()
    {
        inMoveTrigger = true;                
    }

    // Update is called once per frame
    void Update()
    {        
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.gameObject.tag == "Wall" || other.gameObject.tag =="Platform")
            inMoveTrigger = true;
    }
    void OnTriggerExit2D(Collider2D other)
    {
        if(other.gameObject.tag == "Wall" || other.gameObject.tag =="Platform")
            inMoveTrigger = false; 
    }

    public bool GetinMoveTrigger()
    {
        return inMoveTrigger;
    }
}
