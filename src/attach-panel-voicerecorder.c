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

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

#include <Elementary.h>
#include <ui-gadget-module.h>
#include <dlog.h>

#include "view/audio-recorder-view.h"
#include "attach-panel-voicerecorder.h"
#define POPUP_TITLE_MAX (128)

struct ug_data *g_ugd;

#ifdef ENABLE_UG_CREATE_CB
extern int ug_create_cb(void (*create_cb)(char*,char*,char*,void*), void *user_data);
#endif

bool __attachPanelVoiceRecorder_get_extra_data_cb(app_control_h service, const char *key, void * user_data);

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "ATTACH_PANEL_VOICERECORDER"

struct ug_data {
	Evas_Object *base;
	Evas_Object *pu;
	ui_gadget_h ug;
	ui_gadget_h sub_ug;
	app_control_h service;
	bool is_caller_attach_panel;
};


#ifdef ENABLE_UG_CREATE_CB
static void create_cb(char *ug, char* mem, char* parent, void* user_data)
{
	LOGD("ug : %s, mem : %s, parent : %s, data : %p", ug, mem, parent, user_data);
}
#endif

static void __attachPanelVoiceRecorder_create_content(Evas_Object *parent, struct ug_data *ugd)
{
	LOGD("__attachPanelVoiceRecorder_create_content");
	_main_layout_add(parent, ugd->ug, ugd->service);
}

bool __attachPanelVoiceRecorder_get_extra_data_cb(app_control_h service, const char *key, void *user_data)
{
	char *value = NULL;
	int ret;

	ret = app_control_get_extra_data(service, key, &value);
	if (ret) {
		LOGE("__get_extra_data_cb: error get data(%d)\n", ret);
		return false;
	}

	LOGD("extra data : %s, %s\n", key, value);
	free(value);

	return true;
}

static Evas_Object *__attachPanelVoiceRecorder_create_fullview(Evas_Object *parent, struct ug_data *ugd)
{
	Evas_Object *base = NULL;

	base = elm_layout_add(parent);
	if (!base)
		return NULL;

	elm_layout_theme_set(base, "layout", "application", "default");
	elm_win_indicator_mode_set(parent, ELM_WIN_INDICATOR_SHOW);

	return base;
}

#if 0
static Evas_Object *__attachPanelVoiceRecorder_create_frameview(Evas_Object *parent, struct ug_data *ugd)
{
	Evas_Object *base = NULL;

	base = elm_layout_add(parent);
	if (!base)
		return NULL;

	elm_layout_theme_set(base, "standard", "window", "integration");
	edje_object_signal_emit(_EDJ(base), "elm,state,show,content", "elm");

	return base;
}
#endif

static void *__attachPanelVoiceRecorder_on_create(ui_gadget_h ug, enum ug_mode mode, app_control_h service,
		       void *priv)
{
	Evas_Object *parent = NULL;
	struct ug_data *ugd = NULL;
	char *operation = NULL;

	if (!ug || !priv)
		return NULL;

	LOGD("__attachPanelVoiceRecorder_on_create start");
	bindtextdomain("attach-panel-voicerecorder", "/usr/ug/res/locale");

	ugd = priv;
	ugd->ug = ug;
	ugd->service = service;
	g_ugd = ugd;

	app_control_get_operation(service, &operation);

	if (operation == NULL) {
		/* ug called by ug_create */
		LOGD("ug called by ug_create\n");
	} else {
		/* ug called by service request */
		LOGD("ug called by service request :%s\n", operation);
		free(operation);
	}

	app_control_foreach_extra_data(service, __attachPanelVoiceRecorder_get_extra_data_cb, NULL);

	parent = (Evas_Object *)ug_get_parent_layout(ug);
	if (!parent)
		return NULL;

	char *contact_id = NULL;
	app_control_get_extra_data(service, "__CALLER_PANEL__", &contact_id);
	if (contact_id && !strcmp(contact_id, "attach-panel")) {
		if (ugd) {
			ugd->is_caller_attach_panel = TRUE;
		}
	} else {
		ugd->is_caller_attach_panel = FALSE;
	}

	mode = ug_get_mode(ug);

	ugd->base = __attachPanelVoiceRecorder_create_fullview(parent, ugd);

	__attachPanelVoiceRecorder_create_content(ugd->base, ugd);
	return ugd->base;
}

static void __attachPanelVoiceRecorder_on_start(ui_gadget_h ug, app_control_h service, void *priv)
{
#if 0
	int i;
	pthread_t p_thread[10];
	int thr_id;
	int status;
	int a = 1;

	for (i=0; i<10; i++) {
		thr_id = pthread_create(&p_thread[i], NULL, __attachPanelVoiceRecorder_start_t_func, (void*)&a);
		if (thr_id < 0) {
			perror("thread create error: ");
			exit(0);
		}
		pthread_detach(p_thread[i]);
	}
#endif
}

static void __attachPanelVoiceRecorder_on_pause(ui_gadget_h ug, app_control_h service, void *priv)
{
}

static void __attachPanelVoiceRecorder_on_resume(ui_gadget_h ug, app_control_h service, void *priv)
{
	LOGD("%s : called\n", __func__);
}

static void __attachPanelVoiceRecorder_on_destroy(ui_gadget_h ug, app_control_h service, void *priv)
{
	struct ug_data *ugd;
	LOGD("%s : called\n", __func__);
	if (!ug || !priv)
		return;

	ugd = priv;
	evas_object_del(ugd->base);
	ugd->base = NULL;
}

static void __attachPanelVoiceRecorder_on_message(ui_gadget_h ug, app_control_h msg, app_control_h service,
		      void *priv)
{
	if (!ug || !priv) {
		LOGD("Invalid ug or priv");
		return;
	}

	struct ug_data *ugd = NULL;
	ugd = priv;
	char *display_mode = NULL;
	if (ugd->is_caller_attach_panel) {
		LOGD("called by attach panel ");
		app_control_get_extra_data(msg, APP_CONTROL_DATA_SELECTION_MODE, &display_mode);
		if (display_mode) {
			if(!strcmp(display_mode, "single")) {
				//change to compact view
				LOGD("compact view ");
				_audio_recorder_view_set_view_layout(AUDIO_RECORDER_COMPACT_VIEW);
			} else if (display_mode && !strcmp(display_mode, "multiple")) {
				//change to full view
				LOGD("full view");
				_audio_recorder_view_set_view_layout(AUDIO_RECORDER_FULL_VIEW);
			} else {
				LOGD("invalid mode: %s", display_mode);
			}
		}
	}
}

static void __attachPanelVoiceRecorder_on_event(ui_gadget_h ug, enum ug_event event, app_control_h service,
		    void *priv)
{
	/*int degree = -1;*/

	/*struct ug_data *ugd = priv;*/

	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	default:
		break;
	}
}

#if 0
static void __attachPanelVoiceRecorder_on_key_event(ui_gadget_h ug, enum ug_key_event event,
			 app_control_h service, void *priv)
{
	if (!ug)
		return;

	switch (event) {
	case UG_KEY_EVENT_END:
		ug_destroy_me(ug);
		break;
	default:
		break;
	}
}
#endif

static void __attachPanelVoiceRecorder_on_destroying(ui_gadget_h ug, app_control_h service, void *priv)
{
	LOGD("%s : called\n", __func__);
	_audio_recorder_view_destroy();
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	struct ug_data *ugd;

	if (!ops)
		return -1;

	ugd = calloc(1, sizeof(struct ug_data));
	if (!ugd)
		return -1;

	ops->create = __attachPanelVoiceRecorder_on_create;
	ops->start = __attachPanelVoiceRecorder_on_start;
	ops->pause = __attachPanelVoiceRecorder_on_pause;
	ops->resume = __attachPanelVoiceRecorder_on_resume;
	ops->destroy = __attachPanelVoiceRecorder_on_destroy;
	ops->message = __attachPanelVoiceRecorder_on_message;
	ops->event = __attachPanelVoiceRecorder_on_event;
	/*ops->key_event = __attachPanelVoiceRecorder_on_key_event;*/
	ops->destroying = __attachPanelVoiceRecorder_on_destroying;
	ops->priv = ugd;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	if (!ops)
		return;

	/*struct ug_data *ugd;
	ugd = ops->priv;
	if (ugd)
		free(ugd); */
}
