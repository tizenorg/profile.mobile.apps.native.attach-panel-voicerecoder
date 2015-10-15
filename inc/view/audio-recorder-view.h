/*
* Copyright (c) 2000-2015 Samsung Electronics Co., Ltd All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#ifndef __AUDIO_RECORDER_VIEW_H__
#define __AUDIO_RECORDER_VIEW_H__

#include <Evas.h>
#include <ui-gadget-module.h>
#include <Elementary.h>
#include <Ecore.h>
#include <recorder.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef enum _audio_recorder_view_type{
	AUDIO_RECORDER_FULL_VIEW,
	AUDIO_RECORDER_COMPACT_VIEW,
}AudioRecorderViewType;

struct _audio_recorder_view
{
    Evas_Object *layout;
    Evas_Object *gesture_long_tap;
    Elm_Object_Item *settings_item;
    recorder_h recorder;
    recorder_audio_codec_e *codec_list;
    int codec_list_len;
    char file_path[PATH_MAX];
    char file_name[PATH_MAX];
    recorder_audio_codec_e codec;
    recorder_file_format_e file_format;
    FILE *preproc_file;
    ui_gadget_h ug_handle;
    app_control_h service;
    int bitrate;
    int sample_rate;
    bool is_recording;
    long long int limitsize;
};

typedef struct _audio_recorder_view audio_recorder_view;

/**
 * @brief Create audio recorder view
 * @param[in]   app   application instance
 * @param[in]   navi  owner naviframe
 * @return instance of audio_recorder_view
 */

void _main_layout_add(Evas_Object *parent, ui_gadget_h ug_handle, app_control_h service);

void _audio_recorder_view_set_data(void *data);
void *_audio_recorder_view_get_data();
void _audio_recorder_view_destroy();
void _audio_recorder_view_set_view_layout(AudioRecorderViewType audio_recorder_view_type);

#endif // __AUDIO_RECORDER_VIEW_H__

