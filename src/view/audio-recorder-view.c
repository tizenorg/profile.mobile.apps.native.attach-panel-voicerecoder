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
#include "view/audio-recorder-view.h"
#include <efl_extension.h>

#include <assert.h>
#include <app_control.h>
#include <media_content.h>

#define MAX_LABEL_LEN 256
#define MAX_TIME 1200
#define TIMER_UPD_PERIOD 1.0
#define PREPROC_FILE_BUF_SIZE 512

#define	VR_AUDIO_SOURCE_SAMPLERATE_LOW	     8000
#define VR_AUDIO_SOURCE_SAMPLERATE_HIGH      44100
#define VR_AUDIO_SOURCE_FORMAT		         MM_CAMCORDER_AUDIO_FORMAT_PCM_S16_LE
#define VR_AUDIO_SOURCE_CHANNEL		         1
#define VR_AUDIO_ENCODER_BITRATE_AMR	     12200
#define VR_AUDIO_ENCODER_BITRATE_AAC	     64000
#define VR_AUDIO_TIME_LIMIT		             (100 * 60 * 60)
#define STR_RECORDER_TITLE			"IDS_ST_BUTTON_TAP_AND_HOLD_TO_RECORD_ABB"
#define STR_RECORDING				"IDS_VR_BODY_RECORDING"
#define STR_NOTIFICATION_TEXT		"IDS_VR_TPOP_UNABLE_TO_SAVE_RECORDING_RECORDING_TOO_SHORT"
#define VR_STR_DOMAIN_SYS "sys_string"
#define VR_STR_DOMAIN_LOCAL "attach-panel-voicerecorder"

static audio_recorder_view *viewhandle = NULL;

#if 0
static void _codec_set(audio_recorder_view *view, recorder_audio_codec_e codec);
static void _get_supported_codec_list(audio_recorder_view *view);
static void _recorder_preproc_set(audio_recorder_view *view, bool preproc);
static void _preproc_set(audio_recorder_view *view, bool preproc);
static void _recorder_pause(audio_recorder_view *view);
static void _recorder_cancel(audio_recorder_view *view);
static void _on_recorder_audio_stream_cb(void* stream, int size, audio_sample_type_e format, int channel, unsigned int timestamp, void *user_data);
#endif

static void _recorder_create(audio_recorder_view *view);
static void _recorder_start(audio_recorder_view *view);
static void _recorder_stop(audio_recorder_view *view);
static void _recorder_apply_settings(audio_recorder_view *view);
static void _on_recording_status_cb(unsigned long long elapsed_time, unsigned long long file_size, void *user_data);
static void _on_recording_limit_reached_cb(recorder_recording_limit_type_e type, void *user_data);
static bool _main_file_register(const char *filename);
static void _recorder_destroy(audio_recorder_view *view);
/*static void _on_main_layout_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);*/

#define APP_CONTROL_DATA_FILE_PATH "http://tizen.org/appcontrol/data/file_path"

void _audio_recorder_view_set_data(void *data) {
    /*RETM_IF(!data, "data is NULL");*/
    viewhandle = (audio_recorder_view *)data;
}

void *_audio_recorder_view_get_data() {
    if (viewhandle) {
	return viewhandle;
    }
    return NULL;
}

void _audio_recorder_view_destroy() {
     LOGD("Enter _audio_recorder_view_destroy()");
     audio_recorder_view *view = (audio_recorder_view *)_audio_recorder_view_get_data();
     if (view) {
	_recorder_destroy(view);
     }
     /*free(viewhandle);*/
     _audio_recorder_view_set_data(NULL);
}

static void _on_start_btn_pressed_cb(void *data, Evas_Object *obj,
        const char *emission, const char *source)
{
    LOGD("Enter _on_start_btn_pressed_cb");
    RETM_IF(!data, "data is NULL");

    audio_recorder_view *view = (audio_recorder_view *)data;
    if (!view->is_recording) {
	view->is_recording = TRUE;
	_recorder_start(view);
    }

}

static void _on_start_btn_clicked_cb(void *data, Evas_Object *obj,
        const char *emission, const char *source)
{
    LOGD("Enter _on_start_btn_clicked_cb");
    RETM_IF(!data, "data is NULL");
}

static void _on_stop_btn_pressed_cb(void *data, Evas_Object *obj,
        const char *emission, const char *source)
{
    LOGD("Enter _on_stop_btn_pressed_cb");
    RETM_IF(!data, "data is NULL");

    audio_recorder_view *view = (audio_recorder_view *)data;

    if(view->is_recording) {
    	 view->is_recording = FALSE;
        _recorder_stop(view);
    }
}

static void _on_stop_btn_clicked_cb(void *data, Evas_Object *obj,
        const char *emission, const char *source)
{
    LOGD("Enter _on_stop_btn_clicked_cb");
    RETM_IF(!data, "data is NULL");

    audio_recorder_view *view = (audio_recorder_view *)data;

    if(view->is_recording) {
    	view->is_recording = FALSE;
    	_recorder_stop(view);
    } else {
    	char *domain = VR_STR_DOMAIN_LOCAL;
    	int ret = notification_status_message_post(dgettext(domain, STR_NOTIFICATION_TEXT));
    	if (ret != 0)
    		LOGD("notification_status_message_post()... [0x%x]", ret);
    }
}

#if 0
static void _on_pause_pressed_cb(void *data, Evas_Object *obj,
        const char *emission, const char *source)
{
    LOGD("Enter _on_pause_pressed_cb");
    RETM_IF(!data, "data is NULL");

    audio_recorder_view *view = (audio_recorder_view *)data;

    _recorder_pause(view);
}
#endif

#if 0
static void _on_cancel_btn_pressed_cb(void *data, Evas_Object *obj,
        const char *emission, const char *source)
{
    RETM_IF(!data, "data is NULL");
    audio_recorder_view *view = (audio_recorder_view *)data;

    _recorder_cancel(view);
}
#endif

static void _recorder_create(audio_recorder_view *view)
{
    LOGD("Enter _recorder_create");

    int ret = recorder_create_audiorecorder(&view->recorder);
    if (ret == RECORDER_ERROR_NONE) {
		recorder_set_recording_status_cb(view->recorder, _on_recording_status_cb, view);
		if (view->limitsize > 0) {
			recorder_attr_set_size_limit(view->recorder, (view->limitsize) / 1024);
		}
		recorder_set_recording_limit_reached_cb(view->recorder, _on_recording_limit_reached_cb, view);
    } else {
	LOGD("recorder_create_audiorecorder not successful error code: %d", ret);
    }
}

#if 0
static void _get_supported_codec_list(audio_recorder_view *view)
{
    RETM_IF(!view, "view is NULL");

    if (!view->codec_list) {
        view->codec_list = audio_recorder_get_supported_encoder(view->recorder, &view->codec_list_len);
    }
}
#endif

#if 0
static void _codec_set(audio_recorder_view *view, recorder_audio_codec_e codec)
{
    RETM_IF(!view, "view is NULL");

    view->codec = codec;
    view->file_format = audio_recorder_get_file_format_by_codec(view->recorder, codec);
}
#endif

#if 0
static void _preproc_set(audio_recorder_view *view, bool preproc)
{
    audio_recorder_save_preprocessing(preproc);
}
#endif

static void _recorder_apply_settings(audio_recorder_view *view)
{
    if (view->recorder) {
	recorder_attr_set_audio_channel(view->recorder, 2);
	recorder_attr_set_audio_device(view->recorder, RECORDER_AUDIO_DEVICE_MIC);
	recorder_attr_set_time_limit(view->recorder, MAX_TIME);

        recorder_set_filename(view->recorder, view->file_path);
        recorder_set_file_format(view->recorder, view->file_format);
	LOGD("file_format: %d", view->file_format);
        recorder_set_audio_encoder(view->recorder, view->codec);
	recorder_attr_set_audio_samplerate(view->recorder, view->sample_rate);
	recorder_attr_set_audio_encoder_bitrate(view->recorder, view->bitrate);

        recorder_prepare(view->recorder);

        char *domain = VR_STR_DOMAIN_LOCAL;
        elm_object_domain_translatable_part_text_set(view->layout, "recorder_title", domain, STR_RECORDING);

    }
}

#if 0
static void _recorder_preproc_set(audio_recorder_view *view, bool preproc)
{
    if (preproc) {
        recorder_audio_codec_e codec = RECORDER_AUDIO_CODEC_DISABLE;
        recorder_get_audio_encoder(view->recorder, &codec);

        if (codec != RECORDER_AUDIO_CODEC_DISABLE) {
            if (view->preproc_file) {
                fclose(view->preproc_file);
            }

            const char *file_name = audio_recorder_get_preproc_file_name_by_codec(codec);
            if (file_name) {
                view->preproc_file = fopen(file_name, "r");
                assert(view->preproc_file);
            }

            recorder_set_audio_stream_cb(view->recorder, _on_recorder_audio_stream_cb, view);
        }
    } else {
        recorder_unset_audio_stream_cb(view->recorder);
    }
}
#endif

static void _recorder_destroy(audio_recorder_view *view)
{
    LOGD("Enter _recorder_destroy");
    if (view->recorder) {
        recorder_cancel(view->recorder);
        recorder_unprepare(view->recorder);
        recorder_destroy(view->recorder);
        view->recorder = NULL;
    }
    if(view->gesture_long_tap) {
	evas_object_del(view->gesture_long_tap);
    }
    LOGD("End _recorder_destroy");
}

static void _recorder_start(audio_recorder_view *view)
{
    LOGD("Enter _recorder_start");
    if (!view->recorder) {
        LOGD("Error view->recorder is null");
        return;
    }

    recorder_state_e rec_state;
    recorder_get_state(view->recorder, &rec_state);

    if (rec_state == RECORDER_STATE_PAUSED) {
        recorder_start(view->recorder);
        return;
    }

    struct tm localtime = {0,};
    time_t rawtime = time(NULL);
    char filename[256] = {'\0',};
    char *data_path = "/opt/usr/media/Music/";

#if 0
    /*For MMS*/
    view->codec = RECORDER_AUDIO_CODEC_AMR;
    view->file_format = RECORDER_FILE_FORMAT_AMR;
    view->sample_rate = VR_AUDIO_SOURCE_SAMPLERATE_LOW;
    view->bitrate = VR_AUDIO_ENCODER_BITRATE_AMR;
    if (localtime_r(&rawtime, &localtime) != NULL) {
        snprintf(filename, sizeof(filename), "Voice-%04i-%02i-%02i_%02i:%02i:%02i.amr",
                            localtime.tm_year + 1900, localtime.tm_mon + 1, localtime.tm_mday,
                            localtime.tm_hour, localtime.tm_min, localtime.tm_sec);
    }
#else
    /*For High Quality*/
    view->codec = RECORDER_AUDIO_CODEC_AAC;
    view->file_format = RECORDER_FILE_FORMAT_MP4;
    view->sample_rate = VR_AUDIO_SOURCE_SAMPLERATE_HIGH;
    view->bitrate = VR_AUDIO_ENCODER_BITRATE_AAC;
    if (localtime_r(&rawtime, &localtime) != NULL) {
        snprintf(filename, sizeof(filename), "Voice-%04i-%02i-%02i_%02i:%02i:%02i.m4a",
                            localtime.tm_year + 1900, localtime.tm_mon + 1, localtime.tm_mday,
                            localtime.tm_hour, localtime.tm_min, localtime.tm_sec);
    }
#endif
    /*set file path*/
    snprintf(view->file_path, PATH_MAX, "%s%s", data_path, filename);
    _recorder_apply_settings(view);
    recorder_prepare(view->recorder);
    recorder_start(view->recorder);
}

static bool _main_file_register(const char *filename)
{
	LOGD("Enter _main_file_register");
	int err_code = 0;
	media_info_h info = NULL;

	char *register_file = strdup(filename);
	if (register_file == NULL) {
		LOGD("Failed to allocate memory");
		return FALSE;
	}

	err_code = media_info_insert_to_db(register_file, &info);
	if (err_code != MEDIA_CONTENT_ERROR_NONE) {
		LOGD("failed to media_file_register() : [%s], [%d]", register_file, err_code);
		media_info_destroy(info);
		free(register_file);
		return FALSE;
	}

	media_info_destroy(info);
	free(register_file);

	LOGD("End _main_file_register");
	return TRUE;
}

static void _recorder_stop(audio_recorder_view *view)
{
    LOGD("_recorder_stop");
    int ret = 0;
    int commitResult = RECORDER_ERROR_NONE;
    char **path_array = NULL;

    if (view->recorder) {
        commitResult = recorder_commit(view->recorder);
	if (commitResult != RECORDER_ERROR_NONE) {
		LOGD("recorder_commit failed Error [%d]", recorder_commit);
	}

	_main_file_register(view->file_path);
	elm_object_part_text_set(view->layout, "recorder_timer", "00 : 00");
	char *domain = VR_STR_DOMAIN_LOCAL;
	elm_object_domain_translatable_part_text_set(view->layout, "recorder_title", domain, STR_RECORDER_TITLE);

	/*Return the File To Caller.*/
	app_control_h app_control = NULL;
	ret = app_control_create(&app_control);
	if (ret == APP_CONTROL_ERROR_NONE) {
		LOGD("APP_CONTROL_ERROR_NONE");
		if (commitResult == RECORDER_ERROR_NONE) {
			path_array = (char**)calloc(1, sizeof(char *));
			if (path_array == NULL) {
				LOGD("Failed to allocate memory");
				app_control_destroy(app_control);
				return;
			}
			path_array[0] = strdup(view->file_path);
			ret = app_control_add_extra_data_array(app_control, APP_CONTROL_DATA_PATH, (const char **)path_array, 1);
			if (ret != APP_CONTROL_ERROR_NONE) {
				LOGD("Add selected path failed!");
			}
			ret = app_control_add_extra_data_array(app_control, APP_CONTROL_DATA_SELECTED , (const char **)path_array, 1);
			if (ret != APP_CONTROL_ERROR_NONE) {
				LOGD("Add selected path failed!");
			}
		}
		ret = ug_send_result(view->ug_handle, app_control);
		if (ret < 0) {
                    LOGD("ug_send_result failed");
		}
		app_control_destroy(app_control);
		if (path_array != NULL) {
			if (path_array[0] != NULL) {
				free(path_array[0]);
			}
			free(path_array);
		}
		/*ug_destroy_me(view->ug_handle);*/
	} else {
            LOGD("Failed to create app control\n");
	}
    }
}

#if 0
static void _recorder_pause(audio_recorder_view *view)
{
    LOGD(" Enter _recorder_pause");

    if (view->recorder) {
        int mmf_ret = RECORDER_ERROR_NONE;

        recorder_state_e state_now;
        mmf_ret = recorder_get_state(view->recorder, &state_now);

        if (mmf_ret != RECORDER_ERROR_NONE) {
            VR_DEBUG("recorder_get_state Error : %x\n", mmf_ret);

            LOGD("recorder_get_state failed, Recorder can't be paused");
            return; /* can not pause ! keep old state. */
        }

        if (state_now == RECORDER_STATE_PAUSED) {
            VR_DEBUG("\nrecorder already PAUSED\n");
            LOGD("Recorder can't be paused, already pasued");
            return; /* already pause ! keep old state. */
        }

        LOGD("Recorder is paused");
        recorder_pause(view->recorder);
    }
}
#endif

#if 0
static void _recorder_cancel(audio_recorder_view *view)
{
	recorder_state_e rec_state = RECORDER_STATE_NONE;
	recorder_get_state(view->recorder, &rec_state);

	if (rec_state == RECORDER_STATE_PAUSED || rec_state == RECORDER_STATE_RECORDING) {
	    recorder_cancel(view->recorder);
	    elm_object_part_text_set(view->layout,"recorder_timer","00 : 00");
	    char *domain = VR_STR_DOMAIN_LOCAL;
	    elm_object_domain_translatable_part_text_set(view->layout, "recorder_title", domain, STR_RECORDER_TITLE);
	}
}
#endif

/*static void _destroy(audio_recorder_view *view)
{
    app_reset_cbs(view->app);
    _recorder_destroy(view);

    if (view->preproc_file) {
        fclose(view->preproc_file);
    }

    free(view->codec_list);
    free(view);
}*/

void _main_layout_add(Evas_Object *layout, ui_gadget_h ug_handle, app_control_h service)
{
    audio_recorder_view *view = calloc(1, sizeof(audio_recorder_view));
    if (!view) {
        LOGD("Error Failed to create view");
        return;
    }

    _audio_recorder_view_set_data(view);

    view->layout = layout;
    view->ug_handle = ug_handle;
    view->is_recording = FALSE;
    view->service = service;
    view->limitsize = 0;
    int ret = 0;
    char *size_limit = NULL;
    ret = app_control_get_extra_data(service, "http://tizen.org/appcontrol/data/total_size", &(size_limit));
    if (ret != APP_CONTROL_ERROR_NONE) {
	LOGD("Failed to get total_size information!!");
    } else {
    	view->limitsize = atoi(size_limit);
    }

    LOGD("Size limit = %llu", view->limitsize);

    _recorder_create(view);

    // set layout file
    double scale = elm_config_scale_get();
    if ((scale - 1.8) < 0.0001) {
    	LOGD("WVGA");
    	elm_layout_file_set(layout, "/usr/ug/res/edje/attach-panel-voicerecorder/audio_recorder.edj", "audio_recorder_wvga");
    } else if ((scale - 2.6) < 0.0001) {
    	LOGD("HD");
    	elm_layout_file_set(layout, "/usr/ug/res/edje/attach-panel-voicerecorder/audio_recorder.edj", "audio_recorder_hd");
    } else {
    	elm_layout_file_set(layout, "/usr/ug/res/edje/attach-panel-voicerecorder/audio_recorder.edj", "audio_recorder_hd");
    }

    char *domain = VR_STR_DOMAIN_LOCAL;
    elm_object_domain_translatable_part_text_set(view->layout, "recorder_title", domain, STR_RECORDER_TITLE);
    Evas_Object *rec_btn = NULL;
    rec_btn = elm_button_add(view->layout);
    elm_button_autorepeat_set(rec_btn, EINA_TRUE);
    evas_object_size_hint_weight_set(rec_btn,  EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(rec_btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_color_set(rec_btn, 0, 0, 0, 0);
    elm_object_part_content_set(view->layout,"record_icon_swallow",rec_btn);
    evas_object_show(rec_btn);
    view->gesture_long_tap = elm_gesture_layer_add(rec_btn);

    elm_gesture_layer_cb_set(view->gesture_long_tap, ELM_GESTURE_N_LONG_TAPS, ELM_GESTURE_STATE_START, _on_start_btn_clicked_cb, view);
    elm_gesture_layer_cb_set(view->gesture_long_tap, ELM_GESTURE_N_LONG_TAPS, ELM_GESTURE_STATE_MOVE, _on_start_btn_pressed_cb, view);
    elm_gesture_layer_cb_set(view->gesture_long_tap, ELM_GESTURE_N_LONG_TAPS, ELM_GESTURE_STATE_END, _on_stop_btn_pressed_cb, view);
    elm_gesture_layer_cb_set(view->gesture_long_tap, ELM_GESTURE_N_LONG_TAPS, ELM_GESTURE_STATE_ABORT, _on_stop_btn_clicked_cb, view);
    elm_gesture_layer_attach(view->gesture_long_tap, rec_btn);

    evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
}

#if 0
static void _on_recorder_audio_stream_cb(void* stream, int size, audio_sample_type_e format, int channel, unsigned int timestamp, void *user_data)
{
    audio_recorder_view *view = user_data;

    if (view && view->preproc_file && !feof(view->preproc_file)) {
        char buf[PREPROC_FILE_BUF_SIZE] = {0,};
        int read_bytes = 0;

        while (read_bytes != size) {
            int request_bytes = MIN(PREPROC_FILE_BUF_SIZE, size - read_bytes);
            int read = fread(buf, sizeof(char), request_bytes, view->preproc_file);

            if (!read) {
                break;
            }

            memcpy(stream + read_bytes, buf, read);
            read_bytes += read;
        }
    }
}
#endif

static void _on_recording_status_cb(unsigned long long elapsed_time, unsigned long long file_size, void *user_data)
{
    audio_recorder_view *view =  (audio_recorder_view *)user_data;

    if (view) {
        recorder_state_e rec_state = RECORDER_STATE_NONE;
        recorder_get_state(view->recorder, &rec_state);
        if (rec_state == RECORDER_STATE_PAUSED || rec_state == RECORDER_STATE_READY) {
            return;
        }

        int min = 0;
        int sec = 0;

        if (elapsed_time >= 60000) {
            min = elapsed_time/60000;
            sec = elapsed_time % 60000;
            sec = sec/1000;
        } else {
            min = 0;
            sec = elapsed_time/1000;
        }

        //LOGD("elapsed time is %d",elapsed_time);
        char timer_string[MAX_LABEL_LEN] = { '\0' };

        if (elapsed_time > 0) {
            snprintf(timer_string, MAX_LABEL_LEN, "%02d : %02d", min,sec);
        }

        elm_object_part_text_set(view->layout,"recorder_timer",timer_string);
        evas_object_show(view->layout);
    }
}

static void _on_recording_limit_reached_cb(recorder_recording_limit_type_e type, void *user_data)
{
    audio_recorder_view *view = (audio_recorder_view *)user_data;
    if (view) {
        if (type == RECORDER_RECORDING_LIMIT_TIME) {
            _recorder_stop(view);
        } else if (type == RECORDER_RECORDING_LIMIT_SIZE) {
            _recorder_stop(view);
        }
    }
}

/*static void _on_main_layout_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    audio_recorder_view *view = data;
    if (view) {
        _destroy(view);
    }
}*/

void _audio_recorder_view_set_view_layout(AudioRecorderViewType audio_recorder_view_type)
{
	LOGD("_audio_recorder_view_set_view_layout - START");

	audio_recorder_view *view = (audio_recorder_view *)_audio_recorder_view_get_data();
	RETM_IF(!view, "view is NULL");

	if (audio_recorder_view_type == AUDIO_RECORDER_COMPACT_VIEW) {
		elm_object_signal_emit(view->layout, "compact_view", "prog");
		LOGD("Audio recorder view is set to COMPACT_VIEW");
	} else {
		elm_object_signal_emit(view->layout, "full_view", "prog");
		LOGD("Audio recorder view is set to FULL_VIEW");
	}

	LOGD("_audio_recorder_view_set_view_layout - END");
}
