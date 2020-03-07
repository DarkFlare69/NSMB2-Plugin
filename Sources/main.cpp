#include "3ds.h"
#include "cheats.cpp"
#include "csvc.h"
#include "CTRPluginFramework.hpp"
#include <vector>

namespace CTRPluginFramework
{
	void InitMenu(PluginMenu &menu);
	void displayStartMessage();
	int main(void)
	{
		std::string calculator = " You can use a calculator to convert decimal to hexadecimal.";
		PluginMenu *menu = new PluginMenu("New Super Mario Bros 2 Plugin", 1, 0, 0, "This is a plugin for NSMB2 created by DarkFlare and NintendoGamer28. We kept compatibility and creativity in mind when creating this plugin. All codes have notes containing usage instructions. For additional help, refer to the official GBAtemp page.");
		menu->SynchronizeWithFrame(true);
		InitMenu(*menu);
		MenuFolder *movementCodes = nullptr, *powerUpCodes = nullptr, *menuCodes = nullptr, *levelCodes = nullptr, *worldCodes = nullptr;

		movementCodes = new MenuFolder("Speed & Movement Codes", "These codes relate to the speed and movement of your character.");
		movementCodes->Append(new MenuEntry("Custom Speed Modifier", setSpeed, "Use the D-Pad Left/Right to move at the chosen multiplier."));
		movementCodes->Append(new MenuEntry("Freefly", freefly, "Use L+R along with the D-Pad to move in any direction across the map."));
		movementCodes->Append(new MenuEntry("Touch Screen Teleporter", touchTeleporter, "Touch anywhere along the level map to teleport there. This code only works properly on single-section horizontal stages! If there is sub-worlds or autoscrolling, this may not work. Compatibility with advanced stages is not guaranteed."));
		movementCodes->Append(new MenuEntry("Moonjump", moonjump, "Hold R to fly upwards."));
		movementCodes->Append(new MenuEntry("Can Always Jump (Always Ground Physics)", multiJump, "You will be able to jump in midair at any time."));
		movementCodes->Append(new MenuEntry("Can Always Fly (Always P Speed)", alwaysFly, "You will always be able to achieve flight as Tanooki Mario, and you will be able to fly forever."));
		menu->Append(movementCodes);
		
		powerUpCodes = new MenuFolder("Character & Power-Up Codes", "These codes involve your character, such as Power-Up status and defense.");
		powerUpCodes->Append(new MenuEntry("Set Power-Up", nullptr, setPowerUp, "You will be able to choose which Power-Up you'd like to have."));
		powerUpCodes->Append(new MenuEntry("Set Bottom Screen Item", nullptr, setBottomItem, "You will be able to choose which Power-Up you'd like to have stored on the bottom screen. This code only works when you enter a subworld or exit the level."));
		powerUpCodes->Append(new MenuEntry("Invincible", invincible, "You will not take damage from enemies or course hazards."));
		powerUpCodes->Append(new MenuEntry("Infinite Star Power", infiniteStar, "You will always have the star power invincibility. You will not visibly have star power."));
		powerUpCodes->Append(new MenuEntry("Everlasting Gold Block Head", infiniteGoldHead, "You will always have the gold block head after entering a subworld or obtaining it normally."));
		powerUpCodes->Append(new MenuEntry("Infinite Fireballs", infiniteFireballs, "You can have more than 2 fireballs on screen at once."));
		powerUpCodes->Append(new MenuEntry("Rapidfire Attack", rapidFire, "Hold X or Y to infinitely spam attack. This works great with fireballs and the Tanooki tail."));
		menu->Append(powerUpCodes);

		menuCodes = new MenuFolder("Menu, Interface & Overworld Codes", "These codes are relating to user interface elements (lives, score, timer, etc.) in the overworld and individual levels.");
		levelCodes = new MenuFolder("Level Codes", "These codes should only be used inside of a level.");
		levelCodes->Append(new MenuEntry("Set Coin Count", nullptr, setCoins, "You will be able to choose how many coins you'd like to have." + calculator));
		levelCodes->Append(new MenuEntry("Set Score", nullptr, setScore, "You will be able to set your score for the level you're playing on." + calculator));
		levelCodes->Append(new MenuEntry("Infinite Timer", infiniteTimer, "You will have unlimited time to complete any level."));
		menuCodes->Append(levelCodes);
		worldCodes = new MenuFolder("Overworld Codes", "These codes should only be used in the overworld, and they may only appear to take effect when changing world or entering a level.");
		worldCodes->Append(new MenuEntry("Set Lives", nullptr, setWorldLives, "You will be able to set the number of lives you have." + calculator));
		worldCodes->Append(new MenuEntry("Set Total Star Coin Count", nullptr, setStarCoins, "You will be able to set the total number of star coins you have." + calculator));
		worldCodes->Append(new MenuEntry("Set Total Coin Count", nullptr, setOverworldCoins, "You will be able to set the total number of coins you have collected." + calculator));
		worldCodes->Append(new MenuEntry("Set Overworld Power-Up", nullptr, setOverworldPowerUp, "You will be able to choose which Power-Up you'd like to have in the overworld."));
		menuCodes->Append(worldCodes);
		menu->Append(menuCodes);
		displayStartMessage();
		menu->Run();

		delete menu;
		return 0;
	}

    static void    ToggleTouchscreenForceOn(void)
    {
        static u32 original = 0;
        static u32 *patchAddress = nullptr;

        if (patchAddress && original)
        {
            *patchAddress = original;
            return;
        }

        static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
        };

        Result  res;
        Handle  processHandle;
        s64     textTotalSize = 0;
        s64     startAddress = 0;
        u32 *   found;

        if (R_FAILED(svcOpenProcess(&processHandle, 16)))
            return;

        svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
        svcGetProcessInfo(&startAddress, processHandle, 0x10005);
        if(R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
            goto exit;

        found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

        if (found != nullptr)
        {
            original = found[13];
            patchAddress = (u32 *)PA_FROM_VA((found + 13));
            found[13] = 0xE1A00000;
        }

        svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
        svcCloseHandle(processHandle);
    }

    void    PatchProcess(FwkSettings &settings)
    {
        ToggleTouchscreenForceOn();
    }

    void    OnProcessExit(void)
    {
        ToggleTouchscreenForceOn();
    }

    void    InitMenu(PluginMenu &menu)
	{
	
	}

	void	displayStartMessage()
	{
		if (!File().Exists("CTRPFData.bin"))
		{
			std::string welcome = "Welcome!";
			Sleep(Seconds(2));
			MessageBox(welcome, "Welcome to DarkFlare & NintendoGamer28's NSMB2 plugin! This is the only time you'll see this message.\n\nPress A to continue to the next screen, where you'll review some usage instructions.")();
			usage:
			MessageBox(welcome, "To open the plugin menu, press Select.\n\nThis button can be remapped in the settings at any time.")();
			if (MessageBox(welcome, "All codes and folders in this plugin will contain notes. These notes are very important because it will give you information about how to use any code. You can read a note by clicking on the blue (i) on the bottom screen while hovering over any code or folder in the menu.\n\nThat's all you need to know. Do you understand how to use the plugin?", DialogType::DialogYesNo).SetClear(ClearScreen::Both)())
				MessageBox("Great! You can review this information at any time on the official GBAtemp page.")();
			else
			{
				MessageBox("Let's try again. You can review this information as many times as you'd like.")();
				goto usage;
			}
		}
	}
}
