//
//  Player.cpp
//  Albus-Box
//
//  Created by YotioSoft on 2021/08/07.
//

#include "Player.hpp"

Player::Player() {
	current_track = -1;
	status = PlayerStatus::Stop;
	
	loadSettings();
	loadAudioProfiles();
}

void Player::open(FilePath audio_filepath) {
	audio_files << new Audio(audio_filepath);
	audio_files.back()->setVolume(volume);
	audio_files.back()->setLoop(loop);
	audio_files_path << audio_filepath;
	
	stop();
	current_track = (int)audio_files.size()-1;
	play();
}

bool Player::play() {
	if (!isOpened()) {
		return false;
	}
	
	status = PlayerStatus::Play;
	audio_files[current_track]->play();
	
	return true;
}

bool Player::playFromBegin() {
	if (!isOpened()) {
		return false;
	}
	
	status = PlayerStatus::Play;
	audio_files[current_track]->setPosSample(0);
	audio_files[current_track]->play();
	
	return true;
}

bool Player::playing() {
	if (!isOpened()) {
		return false;
	}
	
	if (status == PlayerStatus::Play && !audio_files[current_track]->isPlaying()) {
		next();
	}
	
	return status == PlayerStatus::Play;
}

bool Player::pause() {
	if (!isOpened()) {
		return false;
	}
	
	status = PlayerStatus::Pause;
	audio_files[current_track]->pause();
	
	return true;
}

bool Player::stop() {
	if (!isOpened()) {
		return false;
	}
	
	status = PlayerStatus::Stop;
	audio_files[current_track]->stop();
	
	return true;
}

void Player::previous() {
	if (!isOpened()) {
		return;
	}
	
	// 現在再生中の曲の再生位置が1.0秒以上なら0.0秒に戻る
	if (audio_files[current_track]->posSec() >= 1.0) {
		audio_files[current_track]->setPosSec(0.0);
		return;
	}
	
	// そうでなければ（前の曲へボタンが2度押されたら）前の曲へ
	// 現在の曲をストップ
	stop();
	
	// 前の曲へ（現在の曲がリストの先頭ならリストの最後の曲へ）
	current_track --;
	if (current_track == -1) {
		current_track = (int)audio_files.size()-1;
	}
	
	// 再生
	playFromBegin();
}

void Player::next() {
	if (!isOpened()) {
		return;
	}
	
	if (current_track == 0 && audio_files.size() == 1 && loop) {	// 1曲しかリストに読み込まれていない場合
		audio_files[current_track]->setPosSec(0.0);					// 0.0秒に戻る
		play();
		return;
	}
	else if (current_track == 0 && audio_files.size() == 1 && !loop) {
		audio_files[current_track]->setPosSec(0.0);
		pause();
		return;
	}
	
	// 現在の曲をストップ
	stop();
	
	// 次の曲へ（現在の曲がリストの最後ならリストの先頭の曲へ）
	current_track ++;
	if (current_track == audio_files.size()) {
		current_track = 0;
	}
	
	// 再生
	playFromBegin();
}

bool Player::seekTo(double skip_pos) {
	if (!isOpened()) {
		return false;
	}
	
	// 0.0~1.0の範囲外ならエラー
	if (skip_pos < 0.0 || skip_pos > 1.0) {
		return false;
	}
	
	// 再生位置変更
	audio_files[current_track]->setPosSample(skip_pos * audio_files[current_track]->samples());
	
	return true;
}

double Player::getVolume() {
	return volume;
}

bool Player::changeVolumeTo(double volume_norm) {
	if (volume_norm < 0.0 || volume_norm > 1.0) {
		return false;
	}
	
	// ボリューム変更
	volume = volume_norm;
	
	// すべてのデータの適応
	for (auto af : audio_files) {
		af->setVolume(volume);
	}
	
	return true;
}

String Player::getTitle() {
	if (!isOpened()) {
		return U"";
	}
	
	if (audio_files_profile.count(getXXHash()) == 0) {
		// ファイル情報が存在しなければファイル名を返す
		return FileSystem::BaseName(audio_files_path[current_track]);
	}
	
	// ファイル情報が存在するなら設定されたタイトルを返す
	return audio_files_profile[getXXHash()].title;
}

void Player::editTitle(String new_title) {
	// ファイル情報をハッシュ値とともに格納
	audio_files_profile[getXXHash()].title = new_title;
	
	// 設定ファイルを上書き保存
	saveAudioProfiles();
}

int Player::getPlayPosSec() {
	if (!isOpened()) {
		return 0;
	}
	return audio_files[current_track]->posSec();
}

int Player::getPlayPosTimeMin() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track]->posSec() / 60;
}

int Player::getPlayPosTimeSec() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track]->posSec() % 60;
}

double Player::getPlayPosNorm() {
	if (!isOpened()) {
		return 0.0;
	}
	return (double)audio_files[current_track]->posSample() / audio_files[current_track]->samples();
}

int64 Player::getPlayPosSample() {
	if (!isOpened()) {
		return 0;
	}
	return audio_files[current_track]->posSample();
}

int Player::getTotalTimeMin() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track]->lengthSec() / 60;
}

int Player::getTotalTimeSec() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track]->lengthSec() % 60;
}

bool Player::isShowWaveEnabled() {
	return show_wave;
}

void Player::setShowWave(bool enable) {
	show_wave = enable;
}

bool Player::isLoopEnabled() {
	return loop;
}

void Player::setLoop(bool enable) {
	if (enable == loop)
		return;

	loop = enable;

	if (!isOpened())
		return;
	
	audio_files[current_track]->pause();

	int64 play_samples = audio_files[current_track]->posSample();

	for (auto af : audio_files) {
		af->setLoop(loop);
	}

	if (status == PlayerStatus::Play) {
		std::cout << "to play " << play_samples << std::endl;
		audio_files[current_track]->setPosSample(play_samples);
		audio_files[current_track]->play();
	}
}

void Player::fft(FFTResult& fft) {
	if (!isOpened())
		return;
	
	FFT::Analyze(fft, *audio_files[current_track]);
}

bool Player::isOpened() {
	if (current_track == -1) {
		return false;
	}
	return true;
}

void Player::loadSettings() {
	if (!FileSystem::Exists(specific::getSettingFilePath())) {
		volume = 1.0;
		show_wave = true;
		loop = false;
		
		return;
	}
	
	JSONReader json(specific::getSettingFilePath());
	
	volume = json[U"volume"].get<double>();
	show_wave = json[U"show_wave"].get<bool>();
	loop = json[U"loop"].get<bool>();
}

void Player::saveSettings() {
	JSONWriter json;
	
	json.startObject();
	{
		json.key(U"volume").write(volume);
		json.key(U"show_wave").write(show_wave);
		json.key(U"loop").write(loop);
	}
	json.endObject();
	
	json.save(specific::getSettingFilePath());
}

void Player::loadAudioProfiles() {
	if (!FileSystem::Exists(specific::getSettingFilePath())) {
		return;
	}
	
	JSONReader json(specific::getAudioProfilesFilePath());
	
	for (auto audio_profile : json[U"audio_profiles"].arrayView()) {
		uint64 hash = audio_profile[U"hash"].get<uint64>();
		audio_files_profile[hash].title = audio_profile[U"title"].getString();
		audio_files_profile[hash].artist_name = audio_profile[U"artist_name"].getString();
	}
}

void Player::saveAudioProfiles() {
	JSONWriter json;
	
	json.startObject();
	{
		json.key(U"audio_profiles").startArray();
		{
			for (auto audio_profile : audio_files_profile) {
				json.startObject();
				{
					json.key(U"hash").write(audio_profile.first);
					
					json.key(U"title").write(audio_profile.second.title);
					json.key(U"artist_name").write(audio_profile.second.artist_name);
				}
				json.endObject();
			}
		}
		json.endArray();
	}
	json.endObject();
	
	json.save(specific::getAudioProfilesFilePath());
}

void Player::free() {
	// 再生中の曲を停止
	stop();
	
	for (auto af : audio_files) {
		af->release();
		delete(af);
	}
}

uint64 Player::getXXHash() {
	return Hash::XXHashFromFile(audio_files_path[current_track]);
}
