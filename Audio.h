#pragma once
#include <filesystem>

using file_path = std::filesystem::path;

namespace Audio
{
	//Main.cppの方で、オーディオエンジンとサウンドエフェクトのインスタンスを作成している
	bool Initialize();
	void Update();
	void Release();

	//オーディオの読み込みと再生の関数
	bool Load(const std::string& name, const file_path& path);
	void Play(const std::string& name);
};

//使い方
//Audio::Load("bgm", "Assets\\Audio\\bgm.wav");
//Audio::Play("bgm");