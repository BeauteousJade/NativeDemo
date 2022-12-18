package com.example.nativedemo;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'nativedemo' library on application startup.
    static {
        System.loadLibrary("nativedemo");
    }

    private static final String TAG = "pby123_MainActivity";


    private String mTestString = "pby123";
    private static int mTestInt = 100;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        int[] array = {1, 2, 3, 4, 5};
//        updateArray(array);
//
//        printString("1234");

//        Log.i(TAG, Arrays.toString(array));

//        updateField();
//        Log.i(TAG, "update after mTestString:" + mTestString + ", mTestInt:" + mTestInt);

        Person person = newObject();
        Log.i(TAG, "person.age:" + person.mAge + " name:" + person.mName);
    }

    /**
     * A native method that is implemented by the 'nativedemo' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();


    public native void updateArray(int[] array);

    public native void printString(String string);

    public native void updateField();

    public native Person newObject();
}