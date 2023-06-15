#include"GuiCommands.h"
#include"ui/GuiManager.hpp"

void SwapGuiTexture::Excute() {
	m_manager->SwapTexture(m_tx);
}