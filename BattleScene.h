#pragma once

#include	<directxmath.h>

struct BattleScene{

	int SceneChangeCnt = 0;		//�G��|���ƂP�Â�����@�G�̍ő吔�ȏ�ɂȂ�ƃV�[���J��

};

//�ϐ��������Ă�����悤��
BattleScene* GetBattleScene();
// �Q�[�����C���V�[������������
bool InitBattleScene();
// �Q�[�����C���V�[���X�V����
void UpdateBattleScene();
// �Q�[�����C���V�[���`�揈��
void DrawBattleScene();
// �Q�[�����C���V�[���I������
void ExitBattleScene();