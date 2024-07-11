/**
 *	@file
 *	@brief	サウンド関連
 */
#include "Sound.h"
#if USE_SOUND
#include <xaudio2.h>
#include <vorbis/vorbisfile.h>

#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")


#if ENABLE_OPUS
#include <opusfile.h>
#pragma comment(lib, "opusfile.lib")
#pragma comment(lib, "opus.lib")
#endif


IXAudio2* g_xaudio;
IXAudio2MasteringVoice* g_masteringVoice;

struct SEInfo {
	char					filename[MAX_PATH];
	int						count;
	IXAudio2SourceVoice*	sourceVoice;
	XAUDIO2_BUFFER			audioBuffer;
	char*					pBuffer;
};

static SEInfo g_se[SE_MAX] = { 0 };
#endif

/**
 * サウンド初期化
 * 
 * @return true or false
 */
bool InitializeSound()
{
#if USE_SOUND
	HRESULT hr;

	if (FAILED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED))) {
		return false;
	}

	UINT32 flags = 0;
#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	if (FAILED(hr = XAudio2Create(&g_xaudio, flags))) {
#ifdef _DEBUG
		flags ^= XAUDIO2_DEBUG_ENGINE;
		if (FAILED(hr = XAudio2Create(&g_xaudio, flags)))
#endif
		{
			return false;
		}
	}

	if (FAILED(hr = g_xaudio->CreateMasteringVoice(&g_masteringVoice))) {
		return false;
	}
#endif
	return true;
}

/**
 * サウンド終了処理
 */
void FinalizeSound()
{
#if USE_SOUND
	if (g_masteringVoice) {
		g_masteringVoice->DestroyVoice();
		g_masteringVoice = nullptr;
	}
	if (g_xaudio) {
		g_xaudio->Release();
		g_xaudio = nullptr;
	}

	CoUninitialize();
#endif
}

static int getEmptySEID()
{
#if USE_SOUND
	for (int i = 0; i < SE_MAX; i++) {
		if (g_se[i].count <= 0) return i;
	}
#endif
	return -1;
}

#if USE_SOUND
static int findSEIDFromFilename(const char* filename)
{
	for (int i = 0; i < SE_MAX; i++) {
		if (g_se[i].count > 0 && strcmp(filename, g_se[i].filename) == 0) {
			return i;
		}
	}
	return -1;
}
#endif

/**
 * oggファイルからSE読み込み
 *
 * @param filename ファイル名
 * @return SEID
 */
int CreateSEOgg(const char* filename)
{
#if USE_SOUND
	//既に読み込まれてるかチェック
	int id = findSEIDFromFilename(filename);
	if (id >= 0) {
		++g_se[id].count;
		return id;
	}

	//SEID発行
	id = getEmptySEID();
	if (id < 0) {
		return -1;
	}

	HRESULT hr;

	OggVorbis_File vf;
	//ファイル開く
	int error = ov_fopen(filename, &vf);
	if (error != 0) {
		return -1;
	}

	//ファイル情報取得
	vorbis_info* vinfo = ov_info(&vf, -1);

	WAVEFORMATEX wfex;
	wfex.wFormatTag = WAVE_FORMAT_PCM;								//PCMなので固定
	wfex.nChannels = vinfo->channels;								//チャンネル数(モノラル:1 ステレオ:2)
	wfex.nSamplesPerSec = vinfo->rate;								//サンプリングレート(22050Hz, 44100Hzなど)
	wfex.wBitsPerSample = 16;										//サンプル当たりのビット数(8bit:8 16bit:16)
	wfex.nBlockAlign = wfex.nChannels * wfex.wBitsPerSample / 8;	//ブロックサイズ(16bit ステレオなら 2*2=4)
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;	//データ速度(秒間当たりのデータ量 channel*sample*bit/8)
	wfex.cbSize = 0;												//PCMなので固定

	//デコード後のファイルサイズ取得
	ogg_int64_t decodedSize = ov_pcm_total(&vf, 0) * wfex.nBlockAlign;

	//必要なメモリ確保して再度読み込み
	char* pDecodedBuf = (char*)malloc((size_t)decodedSize);
	if(pDecodedBuf == nullptr) MessageBox(nullptr, "CreateSEOgg\nメモリーが足りません。", "Unlimited", MB_OK);
	char* pBuf = pDecodedBuf;
	long readSize;
	int bitStream;
	while ((readSize = ov_read(&vf, pBuf, 8192, 0, 2, 1, &bitStream)) > 0) {
		pBuf += readSize;
	}

	//ファイル閉じる
	ov_clear(&vf);

	IXAudio2SourceVoice* sourceVoice = nullptr;
	if (FAILED(hr = g_xaudio->CreateSourceVoice(&sourceVoice, &wfex))) {
		free(pDecodedBuf);
		return -1;
	}

	XAUDIO2_BUFFER buf = { 0 };
	buf.pAudioData = (BYTE*)pDecodedBuf;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = (UINT32)decodedSize;

	SEInfo* pse = &g_se[id];
	pse->count = 1;
	strcpy_s(pse->filename, filename);
	pse->sourceVoice = sourceVoice;
	pse->audioBuffer = buf;
	pse->pBuffer = pDecodedBuf;

	return id;
#else
	return -1;
#endif
}

#if ENABLE_OPUS
/**
 * OpusファイルからSE読み込み
 *
 * @param filename ファイル名
 * @return SEID
 */
int CreateSEOpus(const char* filename)
{
	//既に読み込まれてるかチェック
	int id = findSEIDFromFilename(filename);
	if (id >= 0) {
		++g_se[id].count;
		return id;
	}

	//SEID発行
	id = getEmptySEID();
	if (id < 0) {
		return -1;
	}

	int ret;
	//ファイル開く
	OggOpusFile* of = op_open_file(filename, &ret);
	if (of == NULL) {
		return -1;
	}

	const OpusHead* pHead = op_head(of, -1);
	if (!pHead) {
		return false;
	}
	WAVEFORMATEX wfex{};
	//PCMなので固定
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	//チャンネル数(モノラル:1 ステレオ:2)
	wfex.nChannels = pHead->channel_count;
	//サンプリングレート(48000固定)
	wfex.nSamplesPerSec = 48000;
	//サンプル当たりのビット数(8bit:8 16bit:16)
	wfex.wBitsPerSample = 16;
	//ブロックサイズ(16bit ステレオなら 2*2=4)
	wfex.nBlockAlign = wfex.nChannels * wfex.wBitsPerSample / 8;
	//データ速度(秒間当たりのデータ量 channel*sample*bit/8)
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;


	// 合計サイズの取得
	ogg_int64_t decodedSize = op_pcm_total(of, -1) * wfex.nBlockAlign;
	//必要なメモリ確保
	char* pDecodedBuf = (char*)malloc(decodedSize);
	char* pBuf = pDecodedBuf;

	int readSize;
	while ((readSize = op_read(of, (opus_int16*)pBuf, 8096, NULL)) > 0) {
		pBuf += readSize * wfex.nBlockAlign;
	}
	op_free(of);

	IXAudio2SourceVoice* sourceVoice = nullptr;
	if (FAILED(g_xaudio->CreateSourceVoice(&sourceVoice, &wfex))) {
		free(pDecodedBuf);
		return -1;
	}
	

	XAUDIO2_BUFFER buf = { 0 };
	buf.pAudioData = (BYTE*)pDecodedBuf;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = decodedSize;

	SEInfo* pse = &g_se[id];
	pse->count = 1;
	strcpy(pse->filename, filename);
	pse->sourceVoice = sourceVoice;
	pse->audioBuffer = buf;
	pse->pBuffer = pDecodedBuf;

	return id;
}
#endif

/**
 * SE再生
 * @param seid SEID
 * @param isLoop trueで無限ループ再生
 * @param volume max 1.0 - 0.0 min
 */
void PlaySE(int seid, bool isLoop, float volume)
{
#if USE_SOUND
	SEInfo* pse = &g_se[seid];
	if (pse->count <= 0) return;

	pse->audioBuffer.LoopCount = isLoop ? XAUDIO2_LOOP_INFINITE : 0;

	pse->sourceVoice->Stop();
	pse->sourceVoice->FlushSourceBuffers();
	pse->sourceVoice->SubmitSourceBuffer(&pse->audioBuffer);
	pse->sourceVoice->SetVolume(volume);
	pse->sourceVoice->Start();
#endif
}

/**
 * SE停止
 * @param seid SEID
 */
void StopSE(int seid)
{
#if USE_SOUND
	SEInfo* pse = &g_se[seid];
	if (pse->count <= 0) return;

	pse->sourceVoice->Stop();
#endif
}

/**
 * SEのボリューム設定
 * @param seid SEID
 * @param volume max 1.0 - 0.0 min
 */
void SetVolumeSE(int seid, float volume)
{
#if USE_SOUND
	SEInfo* pse = &g_se[seid];
	if (pse->count <= 0) return;

	pse->sourceVoice->SetVolume(volume);
#endif
}

/**
 * SEの再生速度設定
 * @param seid SEID
 */
void SetFrequencySE(int seid, float ratio)
{
#if USE_SOUND
	SEInfo* pse = &g_se[seid];
	if (pse->count <= 0) return;

	pse->sourceVoice->SetFrequencyRatio(ratio);
#endif
}

/**
 * SE再生中か？
 * @param seid SEID
 * @return 再生中ならtrue
 */
bool IsPlaySE(int seid)
{
#if USE_SOUND
	SEInfo* pse = &g_se[seid];
	if (pse->count <= 0) return false;

	XAUDIO2_VOICE_STATE state;
	pse->sourceVoice->GetState(&state);
	return (state.BuffersQueued > 0);
#else
	return false;
#endif
}

/**
 * SE開放
 * @param seid SEID
 */
void ReleaseSE(int seid)
{
#if USE_SOUND
	SEInfo* pse = &g_se[seid];
	if (pse->count <= 0) return;

	if (--pse->count <= 0) {
		pse->sourceVoice->DestroyVoice();
		pse->sourceVoice = nullptr;
		free(pse->pBuffer);
	}
#endif
}

int GetSEUseCount(void)
{
	int count = 0;

#if USE_SOUND
	for(int i=0; i<SE_MAX; i++){
		if(g_se[i].count > 0) count++;
	}
#endif

	return count;
}
