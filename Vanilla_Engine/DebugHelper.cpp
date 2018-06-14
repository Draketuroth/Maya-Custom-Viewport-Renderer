
#include "DebugHelper.h"

void reportLiveObjects(ID3D11Device* gDevice) {

	ID3D11Debug* DebugDevice = nullptr;
	HRESULT result = gDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&DebugDevice));

	result = DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	SAFE_RELEASE(DebugDevice);

}

void setDebugName(ID3D11DeviceChild* child, const string &name) {

	if (child != nullptr) {

		child->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}
}