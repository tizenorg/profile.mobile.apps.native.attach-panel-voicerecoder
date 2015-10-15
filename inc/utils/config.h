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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "logger.h"
#include <app.h>

#define MAX_LENGTH_PATH 1024

static inline const char *get_resource_path(const char * file_path);

#define EDJPATH "edje"
#define AUDIOPATH "sounds"
#define VIDEOPATH "video"
#define IMAGEPATH "images"

// Edj:
#define LAYOUTEDJ get_resource_path(EDJPATH"/layout.edj")
#define FLAGSEDJ get_resource_path(EDJPATH"/flags.edj")
#define TONEEDJ get_resource_path(EDJPATH"/tone.edj")
#define FRAMEEXTRACTOREDJ get_resource_path(EDJPATH"/frame-extractor.edj")
#define EQUALIZEREDJ get_resource_path(EDJPATH"/equalizer.edj")
#define MEDIAPLAYEREDJ get_resource_path(EDJPATH"/media_player.edj")
#define CAMERAPLAYEREDJ get_resource_path(EDJPATH"/camera_player.edj")
#define VIDEO_RECORDER_PLAYER_EDJ get_resource_path(EDJPATH"/video_recorder_player.edj")
#define AUDIORECORDEREDJ get_resource_path(EDJPATH"/audio_recorder.edj")
#define VIDEOPLAYEREDJ get_resource_path(EDJPATH"/video_player.edj")
#define CAMERACAPTUREEDJ get_resource_path(EDJPATH"/camera_capture.edj")
#define CAPTUREIMGVIEWEREEDJ get_resource_path(EDJPATH"/capture_image_viewer.edj")

// Audio:
#define SAMPLEMP3 AUDIOPATH"/SampleMP3.mp3"
#define SAMPLEAAC AUDIOPATH"/SampleAAC.aac"
#define SAMPLEWAV AUDIOPATH"/SampleWAV.wav"
#define SAMPLEAMR AUDIOPATH"/SampleAMR.amr"
#define PREPROCAAC AUDIOPATH"/PreProcessedAudioAAC.raw"
#define PREPROCAMR AUDIOPATH"/PreProcessedAudioAMR.raw"
#define PREPROCPCM AUDIOPATH"/PreProcessedAudioPCM.raw"

// Video:
#define SAMPLEH263 VIDEOPATH"/sampleH263.3gp"
#define SAMPLEH264 VIDEOPATH"/sampleH264.mp4"
#define SAMPLEMPEG4 VIDEOPATH"/sampleMPEG4.mp4"
#define RECORDERED_3GP_VIDEO "Recordered_Sample.3gp"

// Image:
#define AUDIOICON IMAGEPATH"/audio-icon.png"
#define MICICON IMAGEPATH"/mic.png"

static inline const char *get_resource_path(const char *file_path)
{
    static char absolute_path[MAX_LENGTH_PATH] = {'\0'};

    static char *res_path_buff = NULL;
    if (res_path_buff == NULL) {
        res_path_buff = app_get_resource_path();
    }

    snprintf(absolute_path, MAX_LENGTH_PATH, "%s%s", res_path_buff, file_path);

    return absolute_path;
}

#endif /* CONFIG_H_ */
