/*
 *   Copyright (c) 2021 Project CHIP Authors
 *   All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
package com.tcl.chip.tvapp;

import android.util.Log;

public class MediaInputManagerStub implements MediaInputManager {

  private final String TAG = MediaInputManagerStub.class.getSimpleName();

  @Override
  public MediaInputInfo[] getInputList() {
    MediaInputInfo[] info = new MediaInputInfo[2];
    info[0] = new MediaInputInfo();
    info[0].name = "HDMI 1";
    info[0].description = "Living room Playstation";
    info[0].index = 0;
    info[0].type = MediaInputInfo.INPUT_TYPE_HDMI;

    info[1] = new MediaInputInfo();
    info[1].index = 1;
    info[1].type = MediaInputInfo.INPUT_TYPE_HDMI;

    return info;
  }

  @Override
  public int getCurrentInput() {
    Log.d(TAG, "getCurrentInput");
    return 1;
  }

  @Override
  public boolean selectInput(int index) {
    Log.d(TAG, "selectInput:" + index);
    return true;
  }

  @Override
  public boolean showInputStatus() {
    Log.d(TAG, "showInputStatus");
    return true;
  }

  @Override
  public boolean hideInputStatus() {
    Log.d(TAG, "hideInputStatus");
    return true;
  }

  @Override
  public boolean renameInput(int index, String name) {
    Log.d(TAG, "renameInput index:" + index + " name:" + name);
    return true;
  }
}
