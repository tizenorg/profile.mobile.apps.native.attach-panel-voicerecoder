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

#include "utils/config.h"
#include "utils/audio-recorder-utils.h"
#include "utils/logger.h"

#include <app.h>
#include <app_preference.h>
#include <assert.h>
#include <stdlib.h>

typedef struct
{
    recorder_audio_codec_e *codec_list;
    int len;
} supported_encoder_data;

typedef struct
{
    int samplerate;
    int channel;
    int bitrate;
    recorder_audio_codec_e codec;
    audio_recording_quality_e quality;
} audio_quality_table;

static const audio_quality_table audio_quality[] =
{
    {8000,  1, 256000,  RECORDER_AUDIO_CODEC_PCM, AQ_LOW},
    {22050, 2, 706000,  RECORDER_AUDIO_CODEC_PCM, AQ_MEDIUM},
    {44100, 2, 1411000, RECORDER_AUDIO_CODEC_PCM, AQ_HIGH},
    {8000,  1, 4750,    RECORDER_AUDIO_CODEC_AMR, AQ_LOW},
    {8000,  1, 7950,    RECORDER_AUDIO_CODEC_AMR, AQ_MEDIUM},
    {8000,  1, 12200,   RECORDER_AUDIO_CODEC_AMR, AQ_HIGH},
    {22050, 2, 120000,  RECORDER_AUDIO_CODEC_AAC, AQ_LOW},
    {44100, 2, 220000,  RECORDER_AUDIO_CODEC_AAC, AQ_MEDIUM},
    {44100, 2, 320000,  RECORDER_AUDIO_CODEC_AAC, AQ_HIGH}
};

static bool _recorder_supported_audio_encoder_cb(recorder_audio_codec_e codec, void *user_data);

void audio_recorder_save_preprocessing(bool preprocessing)
{
    //preference_set_boolean(VCONF_PREPROC_KEY, preprocessing);
}

bool audio_recorder_read_preprocessing()
{
    bool val = false;
    //_get_boolean(VCONF_PREPROC_KEY, &val);
    return val;
}

void audio_recorder_set_quality(recorder_h recorder, audio_recording_quality_e quality)
{
    RETM_IF(!recorder, "recorder is null");

    int size = sizeof(audio_quality) / sizeof(audio_quality_table);

    recorder_audio_codec_e codec = RECORDER_AUDIO_CODEC_DISABLE;
    recorder_get_audio_encoder(recorder, &codec);

    if (codec != RECORDER_AUDIO_CODEC_DISABLE) {
        int i;
        for (i = 0; i < size; ++i) {
            if (audio_quality[i].codec == codec && audio_quality[i].quality == quality) {
                recorder_attr_set_audio_samplerate(recorder, audio_quality[i].samplerate);
                recorder_attr_set_audio_channel(recorder, audio_quality[i].channel);
                recorder_attr_set_audio_encoder_bitrate(recorder, audio_quality[i].bitrate);
                break;
            }
        }
    }
}


void audio_recorder_save_quality(audio_recording_quality_e quality)
{
   // preference_set_int(VCONF_QUALITY_KEY, quality);
}

audio_recording_quality_e audio_recorder_read_quality()
{
    int val = AQ_LOW;
   // preference_get_int(VCONF_QUALITY_KEY, &val);
    return val;
}

recorder_audio_codec_e *audio_recorder_get_supported_encoder(recorder_h recorder, int *list_length)
{
    supported_encoder_data data = {0};
    data.codec_list = NULL;
    data.len = 0;

    int res = recorder_foreach_supported_audio_encoder(recorder, _recorder_supported_audio_encoder_cb, &data);

    if (res && list_length) {
        *list_length = data.len;
    }

    return data.codec_list;
}

recorder_file_format_e audio_recorder_get_file_format_by_codec(recorder_h recorder, recorder_audio_codec_e codec)
{
    switch (codec){
        case RECORDER_AUDIO_CODEC_AMR:
            return RECORDER_FILE_FORMAT_AMR;
            break;

        case RECORDER_AUDIO_CODEC_AAC:
            return RECORDER_FILE_FORMAT_MP4;
            break;

        case RECORDER_AUDIO_CODEC_PCM:
            return RECORDER_FILE_FORMAT_WAV;
            break;

        case RECORDER_AUDIO_CODEC_VORBIS:
            return RECORDER_FILE_FORMAT_OGG;
            break;

        default:
            break;
    }

    assert(false);
    return RECORDER_FILE_FORMAT_WAV;
}


const char *audio_recorder_get_preproc_file_name_by_codec(recorder_audio_codec_e codec)
{
    switch (codec) {
        case RECORDER_AUDIO_CODEC_AMR:
            return get_resource_path(PREPROCAMR);
            break;

        case RECORDER_AUDIO_CODEC_AAC:
            return get_resource_path(PREPROCAAC);
            break;

        case RECORDER_AUDIO_CODEC_PCM:
            return get_resource_path(PREPROCPCM);
            break;

        default:
            break;
    }

    return "";
}

static bool _recorder_supported_audio_encoder_cb(recorder_audio_codec_e codec, void *user_data)
{
    RETVM_IF(!user_data, false, "data is NULL");
    bool result = false;
    supported_encoder_data *data = user_data;

    if (codec != RECORDER_AUDIO_CODEC_DISABLE) {
        data->codec_list = realloc(data->codec_list, sizeof(recorder_audio_codec_e) * (data->len + 1));
	if (!data->codec_list) {
		return false;
	}
        data->codec_list[data->len] = codec;
        ++(data->len);
        result = true;
    }

    return result;
}

