#include "Audio.h"
#include <Audio.h>
#include <memory>
#include <unordered_map>

namespace {
	std::unique_ptr<DirectX::AudioEngine> audioEngine_;  //オーディオのシステム自体
	std::unordered_map<std::string, std::unique_ptr<DirectX::SoundEffect>> sounds_; //サウンドエフェクトのマップ
}

bool Audio::Initialize()
{
	//オーディオエンジンの作成(プログラム全体で1つあればいい)
	audioEngine_ = std::make_unique<DirectX::AudioEngine>();
	return true;
}

void Audio::Update()
{
	if (audioEngine_ != nullptr)
	{
		audioEngine_->Update();
	}
}

void Audio::Release()
{
	sounds_.clear(); // サウンドエフェクトの解放
	//audioEngine_.reset(); // オーディオエンジンの解放
}

bool Audio::Load(const std::string& name, const file_path& path)
{
	if (audioEngine_ == nullptr)
	{
		return false;
	}
	sounds_[name] = std::make_unique<DirectX::SoundEffect>(audioEngine_.get(), path.c_str());
	return true;
}

void Audio::Play(const std::string& name)
{
	auto it = sounds_.find(name);
	if (it != sounds_.end())
	{
		it->second->Play();
	}
}
