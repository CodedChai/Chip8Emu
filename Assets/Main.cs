using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Main : MonoBehaviour {
    public Material mat;
    public string fileLocation;
    Chip8 myChip8;
    int modifier = 10;

    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;

	// Use this for initialization
	void Start () {
        myChip8 = GetComponent<Chip8>();
        myChip8.LoadApplication(fileLocation);
        modifier = Screen.width / SCREEN_WIDTH;
	}
	
	// Update is called once per frame
	void Update () {
        KeyboardDown();
        KeyboardUp();
        myChip8.EmulateCycle();
        if (myChip8.drawFlag)
        {
            UpdateQuads();
            myChip8.drawFlag = false;
        }
    }

    void UpdateQuads()
    {
        Color col;
        for (int y = 0; y < 32; ++y)
            for (int x = 0; x < 64; ++x)
            {
                if (myChip8.gfx[(y * 64) + x] == 0)
                    col = Color.black;
                else
                    col = Color.white;

                DrawPixel(x, y, col);
            }
    }

    void DrawPixel(int x, int y, Color col)
    {
      
            GL.Begin(GL.QUADS);
            mat.SetPass(0);
            GL.Color(col);
            GL.Vertex3((x * modifier) + 0.0f, (y * modifier) + 0.0f, 0.0f);
            GL.Vertex3((x * modifier) + 0.0f, (y * modifier) + modifier, 0.0f);
            GL.Vertex3((x * modifier) + modifier, (y * modifier) + modifier, 0.0f);
            GL.Vertex3((x * modifier) + modifier, (y * modifier) + 0.0f, 0.0f);
            GL.End();
            print("Draw pixel");
        
      
    }

    void OnPostRender()
    {
        if (!mat)
        {
            Debug.LogError("Please Assign a material on the inspector");
            return;
        }
        GL.PushMatrix();
        mat.SetPass(0);
        GL.LoadOrtho();
        GL.Begin(GL.QUADS);
        GL.Color(Color.red);
        GL.Vertex3(0, 0.5F, 0);
        GL.Vertex3(0.5F, 1, 0);
        GL.Vertex3(1, 0.5F, 0);
        GL.Vertex3(0.5F, 0, 0);
        GL.Color(Color.cyan);
        GL.Vertex3(0, 0, 0);
        GL.Vertex3(0, 0.25F, 0);
        GL.Vertex3(0.25F, 0.25F, 0);
        GL.Vertex3(0.25F, 0, 0);
        GL.End();
        GL.PopMatrix();
    }

    void KeyboardDown()
    {
        
        if (Input.GetKeyDown(KeyCode.Escape))    // esc
            Application.Quit();

        if (Input.GetKeyDown("1")) myChip8.key[0x1] = 1;
        else if (Input.GetKeyDown("2")) myChip8.key[0x2] = 1;
        else if (Input.GetKeyDown("3")) myChip8.key[0x3] = 1;
        else if (Input.GetKeyDown("4")) myChip8.key[0xC] = 1;

        else if (Input.GetKeyDown(KeyCode.Q)) myChip8.key[0x4] = 1;
        else if (Input.GetKeyDown(KeyCode.W)) myChip8.key[0x5] = 1;
        else if (Input.GetKeyDown(KeyCode.E)) myChip8.key[0x6] = 1;
        else if (Input.GetKeyDown(KeyCode.R)) myChip8.key[0xD] = 1;

        else if (Input.GetKeyDown(KeyCode.A)) myChip8.key[0x7] = 1;
        else if (Input.GetKeyDown(KeyCode.S)) myChip8.key[0x8] = 1;
        else if (Input.GetKeyDown(KeyCode.D)) myChip8.key[0x9] = 1;
        else if (Input.GetKeyDown(KeyCode.F)) myChip8.key[0xE] = 1;

        else if (Input.GetKeyDown(KeyCode.Z)) myChip8.key[0xA] = 1;
        else if (Input.GetKeyDown(KeyCode.X)) myChip8.key[0x0] = 1;
        else if (Input.GetKeyDown(KeyCode.C)) myChip8.key[0xB] = 1;
        else if (Input.GetKeyDown(KeyCode.V)) myChip8.key[0xF] = 1;
    }

    void KeyboardUp()
    {
        if (Input.GetKeyUp("1")) myChip8.key[0x1] = 0;
        else if (Input.GetKeyUp("2")) myChip8.key[0x2] = 0;
        else if (Input.GetKeyUp("3")) myChip8.key[0x3] = 0;
        else if (Input.GetKeyUp("4")) myChip8.key[0xC] = 0;

        else if (Input.GetKeyUp(KeyCode.Q)) myChip8.key[0x4] = 0;
        else if (Input.GetKeyUp(KeyCode.W)) myChip8.key[0x5] = 0;
        else if (Input.GetKeyUp(KeyCode.E)) myChip8.key[0x6] = 0;
        else if (Input.GetKeyUp(KeyCode.R)) myChip8.key[0xD] = 0;

        else if (Input.GetKeyUp(KeyCode.A)) myChip8.key[0x7] = 0;
        else if (Input.GetKeyUp(KeyCode.S)) myChip8.key[0x8] = 0;
        else if (Input.GetKeyUp(KeyCode.D)) myChip8.key[0x9] = 0;
        else if (Input.GetKeyUp(KeyCode.F)) myChip8.key[0xE] = 0;

        else if (Input.GetKeyUp(KeyCode.Z)) myChip8.key[0xA] = 0;
        else if (Input.GetKeyUp(KeyCode.X)) myChip8.key[0x0] = 0;
        else if (Input.GetKeyUp(KeyCode.C)) myChip8.key[0xB] = 0;
        else if (Input.GetKeyUp(KeyCode.V)) myChip8.key[0xF] = 0;
    }
}
