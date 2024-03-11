#pragma once
#include	<iostream>
#include	<filesystem>
#include	<functional>

#include    <assimp\Importer.hpp>
#include    <assimp\scene.h>
#include    <assimp\postprocess.h>
#include    <assimp/cimport.h>
#include	<thread>

// 一定時間後に関数を起動する
void Invoke(std::function<void()> Function, int Time);

// ファイルパスからファイル名だけを取得する
std::string GetFileName(std::string filepath);
// ファイルパスからファイル名だけを取得する
std::wstring GetFileNameWide(std::string filepath);
// ファイルパスからファイル拡張子を取得する
std::string GetFileExt(std::string filepath);
// ファイルパスから親ディレクトリを取得する
std::wstring GetParentDirectoryWide(std::string filepath);
