using UnityEngine;
using System;
using System.IO.Ports;
using UnityOSC;

public class FlyCamera : MonoBehaviour
{
    //localization serial information
    private SerialPort arduinoStream = new SerialPort("COM16", 115200);
    private Vector3 lastCoord = new Vector3(0, 0, 0);

    //IMU serial information
    //private SerialPort imuStream = new SerialPort("COM17", 115200);
    //private Vector3 lastAngles = new Vector3(0, 0, 0);

    

    void Start()
    {
        //initialize both serial streams
        arduinoStream.ReadTimeout = 1000;
        arduinoStream.Open();

        //imuStream.ReadTimeout = 1000;
        //imuStream.Open();
    }

    void Update()
    {
        //get IMU data
        //Vector3 nextAngles = ReadFromIMU(lastAngles);
        //transform.eulerAngles = nextAngles;
        //lastAngles = nextAngles;
        

        //Get localization data
        Vector3 nextCoord = ReadFromArduino(lastCoord);
        transform.position = nextCoord;
        lastCoord = nextCoord;
    }

    private Vector3 ReadFromArduino(Vector3 old)
    {
        string str = "blank";
        try {
            str =  arduinoStream.ReadLine();
            //split into coordinates
            string[] coords_string = str.Split(' ');
            float[] coords = new float[3] { 0, 0, 0 };
            //check for encoding that this is actually a coordinate (put in Arduino code)
            if(coords_string[0] == "~")
            {
                for (int i = 0; i < 3; i++)
                {
                    coords[i] = float.Parse(coords_string[i + 1]) * 10;
                }
                //System.Diagnostics.Debug.WriteLine(coords);

                //filter out out-of-bounds readings
                /*
                if (Math.Abs(coords[0] - old.x) > 1 || Math.Abs(coords[1] - old.z) > 1 || Math.Abs(coords[2] - old.y) > 1)
                {
                    return old;
                }
                */

                //z/y direction is currently fixed, since we are using 2D localization
                return new Vector3(coords[0], 18.288f, coords[1]);
            }
            else
            {
                System.Diagnostics.Debug.WriteLine("Arduino: not coords");
                return old;
            }
        }

        catch (TimeoutException) {
            System.Diagnostics.Debug.WriteLine("Arduino: Timeout");
            System.Diagnostics.Debug.WriteLine(str);

            return old;
        }
    }
}
