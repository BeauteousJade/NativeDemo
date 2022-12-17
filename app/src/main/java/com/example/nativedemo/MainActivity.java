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

    private static String TAG = "pby13_MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        int[] array = {1, 2, 3, 4, 5};
        updateArray(array);
        Log.i(TAG, Arrays.toString(array));
    }

    /**
     * A native method that is implemented by the 'nativedemo' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();


    public native void updateArray(int[] array);
}