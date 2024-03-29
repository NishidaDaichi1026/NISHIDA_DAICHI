#include	<DirectXMath.h>
#include	"CIndexBuffer.h"
#include	"CVertexBuffer.h"
#include	"CShader.h"
#include	"CDirectInput.h"
#include	"dx11mathutil.h"
#include	"camera.h"
#include	"CMaterial.h"
#include	"CStaticMesh.h"
#include	"CStaticMeshRenderer.h"

//初期化
void Init3DModel();
//メモリ解放
void Uninit3DModel();
//更新
void Update3DModel();
//描画
void Draw3DModel();
