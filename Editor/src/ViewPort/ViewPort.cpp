#include "ViewPort.h"

Umi::ViewPort::ViewPort()
{

}

Umi::ViewPort::~ViewPort()
{
	if (g_MapTexture) g_MapTexture->Release();
	if (g_MapRTV) g_MapRTV->Release();
	if (g_MapSRV) g_MapSRV->Release();
	if (g_MapDepthTexture) g_MapDepthTexture->Release();
	if (g_MapDSV) g_MapDSV->Release();
}
