#pragma once

//初期化
void InitAttTex();
//描画
void DrawAttTex();

//攻撃コマンド
void DrawAttackCom();
//スキルコマンド
void DrawSpecialCom();
//選択されている属性をわかりやすく表示
void DrawSelCom();

//各スキルが使用可能か--------------
void DrawAttTex_N();
void DrawAttTex_F();
void DrawAttTex_W();
void DrawAttTex_I();
void DrawAttTex_T();
//----------------------------------
//メモリ解放
void ExitAttTex();