using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System;
using System.Linq;

public class JsonSaveLoad : MonoBehaviour
{
    private static string key = "12jmkv9d2=4zx;ort";

    public static void CreateJsonFile(string str, string filename)
    {
        string path = pathForDocumentsFile(filename);
        string aesString = AESScript.Encrypt(str,key);

        FileStream file = new FileStream(path, FileMode.Create, FileAccess.Write);

        StreamWriter sw = new StreamWriter(file);
        sw.WriteLine(aesString);

        sw.Close();
        file.Close();
    }


    public static T LoadJsonFile<T>(string filename)//, int lineIndex )
    {
        string path = pathForDocumentsFile(filename);
        string aesString;
        FileStream file = new FileStream(path, FileMode.Open, FileAccess.Read);
        StreamReader sr = new StreamReader(file);

        string str = null;
        str = sr.ReadLine();

        sr.Close();
        file.Close();

         aesString= AESScript.Decrypt(str,key);
        
        return JsonUtility.FromJson<T>(aesString);

    }

    public static string pathForDocumentsFile(string filename)
    {
        if (Application.platform == RuntimePlatform.IPhonePlayer)
        {
            string path = Application.dataPath.Substring(0, Application.dataPath.Length - 5);
            path = path.Substring(0, path.LastIndexOf('/'));
            return Path.Combine(Path.Combine(path, "Documents"), filename);
        }

        else if (Application.platform == RuntimePlatform.Android)
        {
            string path = Application.persistentDataPath;
            path = path.Substring(0, path.LastIndexOf('/'));
            return Path.Combine(path, filename);
        }

        else
        {
            string path = Application.dataPath;
            path = path.Substring(0, path.LastIndexOf('/'));
            return string.Format("{0}/{1}.json", Application.dataPath, filename);
        }
    }
    public static void DataSave(string dataName)
    {
        string jsonData;
        
        if(dataName == "StateSaveData")
        {
            StateSaveData saveData = new StateSaveData(); //??????????????? ?????? ?????? ???????????? ???????????? ?????? ??? ?????????.
            jsonData = JsonUtility.ToJson(saveData); // object to json

        }
        else// if(dataName == "SettingData")
        {
            SettingData saveData = new SettingData();
            jsonData = JsonUtility.ToJson(saveData); // object to json

        }
        CreateJsonFile(jsonData, dataName);//"StateSaveData");
    }
    public static void DataLoad(string dataName)
    {
        if (dataName == "StateSaveData")
        {
            StateSaveData loadData = LoadJsonFile<StateSaveData>(dataName);//"StateSaveData");
            loadData.PlayerLoadData();
        }
        else// if(dataName == "SettingData")
        {
            SettingData loadData = LoadJsonFile<SettingData>(dataName);
            loadData.LoadSetting();
        }
    }


    public static void RecordSave(List<HeightRecord> h)
    {
        //List<HeightRecord> HR = h;
        //???????????? json?????? ???????????? array??? ??????.
        RecordOutterArray outterRecord = new RecordOutterArray();
        outterRecord.records = h.ToArray();
        string jsonData = JsonUtility.ToJson(outterRecord); // object to json

        CreateJsonFile(jsonData, "HeightRecord");
    }
    public static void RecordLoad()
    {
        RecordOutterArray recordData = LoadJsonFile<RecordOutterArray>("HeightRecord");
        List<HeightRecord> hRecord = recordData.records.ToList();
        PlayerData.SetHeightRecords(hRecord);
    }

    //????????? ?????? ?????? ?????? ??????
    public static bool SaveFileCheck(string dataName)
    {
        string path = pathForDocumentsFile(dataName);

        FileInfo info = new FileInfo(path);
        return info.Exists;
    }
}


//????????? ?????????
[System.Serializable]
public class HeightRecord
{
    public string saveDate;
    public float Height;
    public int score;
    public HeightRecord()
    {
        this.saveDate = DateTime.Now.ToString("yyyy.MM.dd");
        this.Height = PlayerData.HeightM;
        this.score = PlayerData.score;
    }

}
//List ????????????????????? ??? ????????? ????????? array??? ?????? ??????
[System.Serializable]
public class RecordOutterArray
{
    public HeightRecord[] records;
}

// ?????? ????????? ????????? ?????????
[System.Serializable]
public class StateSaveData
{
    public string saveTime; // ????????????
    public float stageHeghitM; // ?????? ?????? ???????????? ??????
    public int score;    //?????? ??????. ?????? ????????? ??????
    public int heart;
    public int stageNum;// ?????? ?????? ????????????

    //???????????? ?????? ????????????.
    //save
    public StateSaveData()
    {
        saveTime = DateTime.Now.ToString("yyyy.MM.dd");
        stageHeghitM = PlayerData.stageHeightM;
        score = PlayerData.score;
        heart = PlayerData.heart;
        stageNum = PlayerData.stageNum;
    }

    // ????????? ????????? ????????? to ???????????? ????????? ??????
    public void PlayerLoadData()
    {
        PlayerData.SetLoadData(stageHeghitM, score, heart, stageNum);
    }


}

[System.Serializable]
public class SettingData
{
    public bool mute;
    public SettingData()
    {
        mute = PlayerData.mute;
    }
    public void LoadSetting()
    {
        PlayerData.SetMute(mute);
    }
}



    

/* (???) ?????? ??????/?????? ?????? ??????
    static void CreateJsonFile(string SaveData, string dataName)
    {
        FileStream fileStream = new FileStream(string.Format("{0}/{1}.json", Application.dataPath, dataName), FileMode.Create);
        byte[] data = Encoding.UTF8.GetBytes(SaveData);
        fileStream.Write(data, 0, data.Length);
        fileStream.Close();
    }

    static T LoadJsonFile<T>(string dataName)
    {
        FileStream fileStream = new FileStream(string.Format("{0}/{1}.json", Application.dataPath, dataName), FileMode.Open);
        byte[] data = new byte[fileStream.Length];
        fileStream.Read(data, 0, data.Length);
        fileStream.Close();

        string jsondata = Encoding.UTF8.GetString(data);
        return JsonUtility.FromJson<T>(jsondata);
    }
*/