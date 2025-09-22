#pragma once
#include "ISystem.h"
#include "Transform.h"
#include "Matrix4x4.h"
#include "Handlers.h"
#include "Vector4.h"
#include "WorldToScreenData.h"
#include "WindowContext.h"
#include "Vector2F.h"
namespace mtgb
{
	class CameraSystem : public ISystem
	{
	public:
		CameraSystem();
		~CameraSystem();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// カメラとなる座標系を登録する
		/// </summary>
		/// <param name="pCameraTransform_">座標系のポインタ</param>
		/// <returns>カメラハンドル</returns>
		CameraHandleInScene RegisterDrawCamera(Transform* pCameraTransform_);
		/// <summary>
		/// 登録解除する
		/// </summary>
		/// <param name="pCameraTransform_">解除する座標系のポインタ</param>
		void UnregisterDrawCamera(const Transform* pCameraTransform_);
		/// <summary>
		/// すべての描画カメラをクリアする
		/// </summary>
		void ClearDrawCameraAll();

		/// <summary>
		/// FOVをセットする
		/// </summary>
		/// <param name="_angle">視野角度(Degree)</param>
		void SetFOV(const float _angle) { fov_ = _angle; }
		/// <summary>
		/// 描画対象となるカメラを指定する
		/// </summary>
		/// <param name="_hCamera">指定するカメラのハンドル</param>
		void SetDrawCamera(const CameraHandleInScene _hCamera) { hCurrentCamera_ = _hCamera; }
		/// <summary>
		/// 現在描画対象となっているカメラのハンドルを取得
		/// </summary>
		/// <returns>カメラのハンドル</returns>
		const CameraHandleInScene GetDrawCamera() const { return hCurrentCamera_; }

		mtgb::Vector3 GetWorldToScreenPos(Vector3 _pos,const WorldToScreenData& _data) const;
		mtgb::Vector3 GetWorldToScreenPos(Vector3 _pos,WindowContext _context);
		const Transform& GetTransform() const;
		const Transform& GetTransform(CameraHandleInScene _hCamera) const;
		const Transform& GetTransform(WindowContext _context) const;
		void GetViewMatrix(Matrix4x4* _pView) const;
		void GetViewMatrix(Matrix4x4* _pView, CameraHandleInScene _hCamera) const;
		void GetProjMatrix(Matrix4x4* _pProj) const;
		void GetPosition(Vector4* _pPosition) const;
		float GetNear() const;
		float GetFar() const;
		float GetFov() const;
		const WorldToScreenData& GetWorldToScreenData(WindowContext _context);
	private:
		void CalculateWorldToScreenData(WorldToScreenData* _data, WindowContext _context);
		std::vector<Transform*> pTransforms_;
		float fov_;
		float near_;
		float far_;
		CameraHandleInScene hCurrentCamera_;
		std::map<WindowContext, WorldToScreenData> worldToScreenDatas_;
		uint64_t currentFrameId_;
	};
}
