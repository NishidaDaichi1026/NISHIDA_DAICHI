#include	"State.h"

//基本ステータス	初期化したいときはここから持ってくる
ST state[] ={
	//ID,名前,HP,ATTACK,DEFENCE,SPEED
	 {0,"プレイヤー", 2000, 500, 100, 1.0f},
	 {1,"スライム", 1500, 150, 64, 1.2f},
	 {2,"マグマ岩", 1000, 100, 250, 0.6f},
	 {3,"ミノタウロス", 5000, 256, 128, 0.8f},
	 {4,"カカシ",100000,0,123,0.0f}
};

//属性の倍率
float weak = 2.0f;		//弱点
float equal = 1.0f;		//等倍
float subtle = 0.5f;	//いまいち
float invalid = 0.0f;	//無効

//属性のステータス (ID, プレイヤー・敵の属性, 各属性に対する耐性)
ATTRIBUTE attr[] = {
	//ID,属性,無倍率,炎倍率,水倍率,氷倍率,雷倍率
	 {0,"無",equal, equal, equal, equal, equal},
	 {1,"炎",equal, invalid, weak, subtle, equal},
	 {2,"水",equal, subtle, invalid, subtle, weak},
	 {3,"氷",equal, weak, subtle, invalid, equal},
	 {4,"雷",equal, equal, weak, equal, invalid}
};