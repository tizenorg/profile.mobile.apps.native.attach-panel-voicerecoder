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

#ifndef __AUDIO_RECORDER_UTILS_H__
#define __AUDIO_RECORDER_UTILS_H__

#include <recorder.h>

/**
 * @brief Enumerations for audio recording quality
 */
typedef enum {
	AQ_LOW = 0,
	AQ_MEDIUM,
	AQ_HIGH
} audio_recording_quality_e;

/**
 * @brief Save preprocessing value to DB
 * @param[in] preprocessing  enable or disable preprocessing
 */
void audio_recorder_save_preprocessing(bool preprocessing);

/**
 * @brief Read preprocessing value from DB
 * @return preprocessing true - enable, false - disable
 */
bool audio_recorder_read_preprocessing();

/**
 * @brief Set quality
 * @param[in] recorder  Instance of recorder_h
 * @param[in] quality   Recording quality
 */
void audio_recorder_set_quality(recorder_h recorder, audio_recording_quality_e quality);

/**
 * @brief Save quality value to DB
 * @param[in] quality   Recording quality
 */
void audio_recorder_save_quality(audio_recording_quality_e quality);

/**
 * @brief Read quality value from DB
 * @return Recording quality
 */
audio_recording_quality_e audio_recorder_read_quality();

/**
 * @brief Get supported audio encoder
 * @param[in] recorder      Instance of recorder_h
 * @param[out] list_length Length of codec list
 * @return List supported codec list (must be freed)
 */
recorder_audio_codec_e *audio_recorder_get_supported_encoder(recorder_h recorder, int *list_length);

/**
 * @brief Get file format by codec
 * @param[in] recorder  Onstance of recorder_h
 * @param[in] codec     Audio codec
 * @return File format
 */
recorder_file_format_e audio_recorder_get_file_format_by_codec(recorder_h recorder, recorder_audio_codec_e codec);

/**
 * @brief Get preprocessing file name by codec
 * @param[in] codec     Audio codec
 * @return File format
 */
const char *audio_recorder_get_preproc_file_name_by_codec(recorder_audio_codec_e codec);

/**
 * @brief Convert codec(int) to string
 * @param[in] codec  Audio codec
 * @return Codec as C string
 */
const char *audio_recorder_codec_to_str(recorder_audio_codec_e codec);

/**
 * @brief Convert file format(int) to string
 * @param[in] format  Audio format
 * @return File format as C string
 */
const char *audio_recorder_file_format_to_str(recorder_file_format_e format);

#endif // __AUDIO_RECORDER_UTILS_H__

