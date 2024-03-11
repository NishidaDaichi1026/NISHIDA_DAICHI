#include    "utility.h"

namespace fs = std::filesystem;

// 指定時間後にスレッドを起動
void Invoke(std::function<void()> Function, int Time)
{
	std::thread([=]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(Time));
			Function();
		}).detach();
}

// ファイルパスからファイル名だけを取得する
std::string GetFileName(std::string filepath) {

    // u8エンコードでｐａｔｈをコンストラクト
    fs::path p = fs::u8path(filepath.c_str());

    // パスから、ファイル名を除いたディレクトリ、ファイル名、
    // 拡張子を除いたファイル名、拡張子を抽出
//    std::cout << "ディレクトリ名 : " << p.parent_path() << std::endl;
//    std::cout << "ファイル名  : " << p.filename() << std::endl;
//    std::cout << "ファイル名（拡張子なし） : " << p.stem() << std::endl;
//    std::cout << "拡張子 : " << p.extension() << std::endl;

    return p.filename().string();
}

// ファイルパスからファイル名だけを取得する
std::wstring GetFileNameWide(std::string filepath) {

    // u8エンコードでｐａｔｈをコンストラクト
//    fs::path p = fs::u8path(filepath.c_str());
    fs::path p = fs::path(filepath.c_str());

    // システムＷＩＤＥ文字で取得
    const std::wstring wsfilename = p.filename().wstring();

    // ロケールを日本にする
    setlocale(LC_ALL, "Japanese");
    std::wcout << wsfilename << std::endl;

    return wsfilename;
}

// ファイルパスからファイル拡張子を取得する
std::string GetFileExt(std::string filepath) {

    // UTF-8
//    fs::path p = fs::u8path(filepath.c_str());
    fs::path p = fs::path(filepath.c_str());

    return p.extension().string();
}

// ファイルパス(UTF16)からファイル拡張子を取得する
std::wstring GetFileExtWide(std::string filepath) {

    //u8エンコードでｐａｔｈをコンストラクト
    fs::path p = fs::u8path(filepath.c_str());

    return p.extension().wstring();
}

// ファイルパスから親ディレクトリを取得する
std::wstring GetParentDirectoryWide(std::string filepath) {

    // u8エンコードでｐａｔｈをコンストラクト
    fs::path p = fs::u8path(filepath.c_str());

    // システムＷＩＤＥ文字(UTF-16)で取得
    std::wstring wsparentdirectory;

    // 親ディレクトリを持っているか？
    if (p.has_parent_path()) {
        wsparentdirectory = p.parent_path().wstring() + L"\\";
    }
    else {
        wsparentdirectory = L"\\";
    }

    return wsparentdirectory;
}