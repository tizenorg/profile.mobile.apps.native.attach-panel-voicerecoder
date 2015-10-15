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

#ifndef __ATTACH_PANEL_VOICERECORDER_H__
#define __ATTACH_PANEL_VOICERECORDER_H__

#include <Elementary.h>

#define PKGNAME "attach-panel-voicerecorder"

#ifndef _EDJ
#define _EDJ(o)			elm_layout_edje_get(o)
#endif
#ifndef _
#define _(s)			dgettext(PKGNAME, s)
#endif
#ifndef dgettext_noop
#define dgettext_noop(s)	(s)
#endif
#ifndef N_
#define N_(s)			dgettext_noop(s)
#endif

#endif//__ATTACH_PANEL_VOICERECORDER_H__
