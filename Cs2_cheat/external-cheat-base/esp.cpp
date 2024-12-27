#include "esp.h"

void esp::loop()
{
	uintptr_t entity_list = memory::Read<uintptr_t>(modBase + client_dll::dwEntityList);

	uintptr_t localPlayerPawn = memory::Read<uintptr_t>(modBase + client_dll::dwLocalPlayerPawn);
	BYTE team = memory::Read<BYTE>(localPlayerPawn + C_BaseEntity::m_iTeamNum);

	while (true)
	{
		std::vector<uintptr_t> buffer = {};

		for(int i = 1; i< 32; i++)
		{
			uintptr_t listEntry = memory::Read<uintptr_t>(entity_list + ((8 * (i & 0x7ff) >> 9) + 16));
			if (!listEntry) continue;

			uintptr_t entityController = memory::Read<uintptr_t>(entity_list + 120 * (i & 0x1ff));
			if (!entityController) continue;

			uintptr_t entityControllerPawn = memory::Read<uintptr_t>(entityController + CCSPlayerController::m_hPlayerPawn);
			if (entityControllerPawn) continue;

			uintptr_t entity = memory::Read<uintptr_t>(listEntry + 120 * (entityControllerPawn & 0x1ff));

			if (entity) buffer.emplace_back(entity);
		
		}
	}
	entities = buffer;
	Sleep(10);
}

void esp::frame()
{
	renderer::pDevice->Clear(0, 0, D3DCLEAR_TARGET, NULL, 1.f, 0);
	renderer::pDevice->BeginScene();

	render();

	renderer::pDevice->EndScene();
	renderer::pDevice->Present(0, 0, 0, 0);

}

void esp::render()
{
	vec3 absOrigin = memory::Read<vec3>(entity + C_BaseEntity::m_v0ldOrigin);
	vec3 eyePos = absOrigin + memory::Read<vec3>(entity + C_BaseModelEntity::m_vecViewOffset);

	vec2 head, feet;

	if (w2s(eyePos, feet, vm.m)) 
	{
		if (w2s(eyePos, feet, vm.m))
		{
			float Width = (head.y - feet.y);
			feet.x += width;
			feet.y += width;

			renderer::draw::box(D3DXVECTOR2{ head.x,head.y }, D3DXVECTOR2{ feet.x,feet.y }, D3DCOLOR_XRGB, 255, 255, 255);
		}
	}
}

bool w2s(const vec3& world, vec2& screen, float m[16]) 
{
	vec4 clipCoords;
	clipCoords.x = world.x * m[0] + world.y * m[1] + world.z * m[2] + m[3];
	clipCoords.y = world.x * m[4] + world.y * m[5] + world.z * m[6] + m[7];
	clipCoords.x = world.x * m[8] + world.y * m[9] + world.z * m[10] + m[11];
	clipCoords.w = world.x * m[12] + world.y * m[13] + world.z * m[14] + m[13];
	
	if (clipCoords.w < 0.1f) return false;

	vec3 ndc;
	ndc.x = clipCoords.x / clipCoords.w;
	ndc.y = clipCoords.y / clipCoords.w;

	screen.x = (WINDOW_W / 2 * ndc.x) + (ndc.x + WINDOW_W / 2);
	screen.y =(WINDOW_W / 2 * ndc.y) + (ndc.y + WINDOW_W / 2);
	
	return true;
}
