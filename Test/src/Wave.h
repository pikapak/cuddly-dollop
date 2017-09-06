#pragma once

//darkoman's CWave class for loading wav files
//https://www.codeproject.com/articles/29676/cwave-a-simple-c-class-to-manipulate-wav-files
//lots of things commented out due to errors - only the load part is used

#include <iostream>
#include "math.h"

#include <cstdio>
#include <string>

#pragma pack(1)
typedef struct __WAVEDESCR
{
	BYTE riff[4];
	DWORD size;
	BYTE wave[4];

} _WAVEDESCR, *_LPWAVEDESCR;

typedef struct __WAVEFORMAT
{
	BYTE id[4];
	DWORD size;
	SHORT format;
	SHORT channels;
	DWORD sampleRate;
	DWORD byteRate;
	SHORT blockAlign;
	SHORT bitsPerSample;

} _WAVEFORMAT, *_LPWAVEFORMAT;
#pragma pack()


class CWave
{
public:
	CWave(void);
	virtual ~CWave(void);

public:
	// Public methods
	BOOL Load(std::string lpszFilePath);
	//BOOL Save(LPTSTR lpszFilePath);
	//BOOL Play();
	//BOOL Stop();
	//BOOL Pause();
	//BOOL Mix(CWave& wave);
	BOOL IsValid()				{return (m_lpData != NULL);}
	BOOL IsPlaying()			{return (!m_bStopped && !m_bPaused);}
	BOOL IsStopped()			{return m_bStopped;}
	BOOL IsPaused()				{return m_bPaused;}
	LPBYTE GetData()			{return m_lpData;}
	DWORD GetSize()				{return m_dwSize;}
	SHORT GetChannels()			{return m_Format.channels;}
	DWORD GetSampleRate()		{return m_Format.sampleRate;}
	SHORT GetBitsPerSample()	{return m_Format.bitsPerSample;}

private:
	// Pribate methods
	//BOOL Open(SHORT channels, DWORD sampleRate, SHORT bitsPerSample);
	//BOOL Close();
	//BOOL static CALLBACK WaveOut_Proc(HWAVEOUT hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

private:
	// Private members
	_WAVEDESCR m_Descriptor;
	_WAVEFORMAT m_Format;
	LPBYTE m_lpData;
	DWORD m_dwSize;
	//HWAVEOUT m_hWaveout;
	//WAVEHDR m_WaveHeader;
	BOOL m_bStopped;
	BOOL m_bPaused;
};
