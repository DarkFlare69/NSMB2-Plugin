#include "CTRPluginFramework.hpp"
#include "Unicode.h"
#include "csvc.h"
#include "3ds.h"
#include <math.h>
#include <vector>

namespace CTRPluginFramework
{
	using StringVector = std::vector<std::string>;
	u32 offset = 0;

	void freefly(MenuEntry *entry)
	{
		static float x = 0, y = 0;
		if (Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset))
			if (!Controller::IsKeysDown(L + R))
			{
				Process::ReadFloat(offset + 0x1D0, x);
				Process::ReadFloat(offset + 0x1D4, y);
			}
			else
			{
				Process::WriteFloat(offset + 0x1D0, x);
				Process::WriteFloat(offset + 0x1D4, y);
				if (Controller::IsKeysDown(L + R + DPadRight))
					Process::WriteFloat(offset + 0x1D0, x = (x + 3.5));
				if (Controller::IsKeysDown(L + R + DPadLeft))
					Process::WriteFloat(offset + 0x1D0, x = (x - 3.5));
				if (Controller::IsKeysDown(L + R + DPadUp))
					Process::WriteFloat(offset + 0x1D4, y = (y + 3.5));
				if (Controller::IsKeysDown(L + R + DPadDown))
					Process::WriteFloat(offset + 0x1D4, y = (y - 3.5));
			}
	}

	void moonjump(MenuEntry *entry)
	{
		if (Controller::IsKeyDown(R) && Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset))
			Process::WriteFloat(offset + 0x1E0, 3.5);
	}

	void touchTeleporter(MenuEntry *entry) // only works on horizontal levels
	{
		u32 doubleUsageTouch = 0;
		float touchX = 0, touchY = 0;
		if (entry->WasJustActivated())
			OSD::Notify("Touch Screen Teleporter: Read the note for compatibility details!", Color::Red, Color::Black);
		if (Process::Read32(0x6A355C, doubleUsageTouch) && doubleUsageTouch && Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset) && Process::Read32(0x101BCC, doubleUsageTouch) && Process::Read32(doubleUsageTouch + 0x30, doubleUsageTouch) && Process::Read32(doubleUsageTouch - 0x54, doubleUsageTouch) && Process::ReadFloat(doubleUsageTouch - 0x28, touchX) && Process::ReadFloat(doubleUsageTouch - 0x24, touchY) && touchX < 285 && touchX > 23 && touchY < 85 && touchY > 53)
		{
			Process::WriteFloat(offset + 0x1E0, 0.4);
			Process::WriteFloat(offset + 0x1D0, ((touchX - 23) * 17.65) + 16);
		}
	}

	void setSpeed(MenuEntry *entry)
	{
		struct SNumber
		{
			const char  *multiplier;
			const float    speed;
		};

		static const std::vector<SNumber> g_Multipliers =
		{
			{ "1x (Default)", 0.f },
			{ "2x", 1.f },
			{ "4x", 3.f },
			{ "8x", 7.f }
		};

		static StringVector multipliers;
		if (multipliers.empty())
			for (const SNumber &i : g_Multipliers)
				multipliers.push_back(i.multiplier);
		Keyboard keyboard("Set Speed Multiplier\n\nSelect which speed multiplier you'd like to have", multipliers);
		static int choice = 0;
		if (entry->WasJustActivated())
			choice = keyboard.Open();
		float ospeed = 0;
		if (choice > -1 && g_Multipliers[choice].speed > 0.f && Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset) && Process::ReadFloat(offset + 0x1B4, ospeed))
		{
			if (Controller::IsKeyDown(DPadLeft))
				Process::WriteFloat(offset + 0x1B4, ospeed - g_Multipliers[choice].speed);
			else if (Controller::IsKeyDown(DPadRight))
				Process::WriteFloat(offset + 0x1B4, ospeed + g_Multipliers[choice].speed);
		}
	}

	void invincible(MenuEntry *entry)
	{
		if (Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset))
			Process::Write32(offset + 0x15EC, 4);
	}

	void infiniteStar(MenuEntry *entry)
	{
		if (Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset))
			Process::Write32(offset + 0x17AC, 5);
	}

	void infiniteGoldHead(MenuEntry *entry) // requires you to enter a subworld or collect the gold block for it to work
	{
		if (entry->WasJustActivated())
			OSD::Notify("Infinite Gold Head: Read the note for compatibility details!", Color::Red, Color::Black);
		if (Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset) && Process::Read32(offset + 0x364, offset))
			Process::Write32(offset + 0x350, 0x255);
	}

	void alwaysFly(MenuEntry *entry)
	{
		if (Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset) && Process::Read32(offset + 0x364, offset))
		{
			Process::Write32(offset + 0x31C, 0x7E);
			Process::Write32(offset + 0x324, 0xFF);
		}
	}

	void infiniteFireballs(MenuEntry *entry)
	{
		if (Process::Read32(0x102CFC, offset) && Process::Read32(offset - 0x164, offset))
		{
			Process::Write32(offset + 0x6C, 0);
			Process::Write32(offset + 0x70, 0);
		}
	}

	void multiJump(MenuEntry *entry)
	{
		if (Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset))
			Process::Write32(offset + 0x13C0, 1);
	}

	void rapidFire(MenuEntry *entry)
	{
		static int counter = 0;
		if (!(counter % 4 == counter) && (Controller::IsKeyDown(X) || Controller::IsKeyDown(Y)))
		{
			Controller::InjectKey((Controller::IsKeyDown(Y) ? X : Y));
			counter = 0;
		}
		else if (!Controller::IsKeyDown(X) && !Controller::IsKeyDown(Y))
			counter = 0;
		else
			counter++;
	}

	void setPowerUp(MenuEntry *entry)
	{
		struct PowerUp
		{
			const char  *name;
			const u16    value;
		};

		static const std::vector<PowerUp> g_powerUps =
		{
			{ "No Power-Up", 0 },
			{ "Mushroom", 0x100 },
			{ "Fire Flower", 0x200 },
			{ "Mini Mushroom", 0x300 },
			{ "Tanooki Leaf", 0x400 },
			{ "Golden Flower", 0x600 },
			{ "Golden Leaf", 0x700 }
		};

		static StringVector powerUps;
		if (powerUps.empty())
			for (const PowerUp &i : g_powerUps)
				powerUps.push_back(i.name);
		Keyboard keyboard("Power-Up Selector\n\nSelect which Power-Up you'd like to have.", powerUps);
		int choice = keyboard.Open();
		if (choice > -1 && Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset))
			Process::Write32(offset + 0x4718, g_powerUps[choice].value);
	}

	void setBottomItem(MenuEntry *entry) // doesn't update item image, and if you don't have bottom screen item before, then nothing will happen
	{
		struct PowerUps
		{
			const char  *name;
			const u8    value;
		};

		static const std::vector<PowerUps> gpowerUps =
		{
			{ "No Power-Up", 9 },
			{ "Mushroom", 0 },
			{ "Fire Flower", 1 },
			{ "Star", 2 },
			{ "1-Up", 3 },
			{ "Mini Mushroom", 4 },
			{ "Tanooki Leaf", 6 },
			{ "Golden Flower", 7 },
			{ "Golden Tanooki", 8 }
		};

		static StringVector powerUps;
		if (powerUps.empty())
			for (const PowerUps &i : gpowerUps)
				powerUps.push_back(i.name);
		Keyboard keyboard("Bottom Screen Item Selector\n\nSelect which item you'd like to have stored in the bottom screen.", powerUps);
		int choice = keyboard.Open();
		if (entry->WasJustActivated())
			OSD::Notify("Set Bottom Screen Item: Read the note for compatibility details!", Color::Red, Color::Black);
		if (choice > -1 && Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset))
			Process::Write8(offset + 0xDF, gpowerUps[choice].value);
	}

	void setScore(MenuEntry *entry)
	{
		u32 score = 0;
		Keyboard keyboard("Enter your desired score, in hexadecimal.");
		if (keyboard.Open(score) != -1 && score >= 0 && Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset) && Process::Read32(offset - 0x40, offset))
			Process::Write32(offset + 0x3F0, score);
	}

	void setCoins(MenuEntry *entry)
	{
		u32 coins = 0;
		Keyboard keyboard("Enter your desired coin count, in hexadecimal.");
		if (keyboard.Open(coins) != -1 && coins >= 0 && Process::Read32(0x10230C, offset) && Process::Read32(offset + 0xB8, offset) && Process::Read32(offset - 0x40, offset))
			Process::Write32(offset + 0x4B4, coins);
	}

	void infiniteTimer(MenuEntry *entry)
	{
		if (Process::Read32(0x10230C, offset) && Process::Read32(offset - 0x1C, offset) && Process::Read32(offset - 0x48C, offset))
			Process::Write32(offset + 0x4D8, 0x1F3D7C);
	}

	void setWorldLives(MenuEntry *entry)
	{
		u32 lives = 0;
		Keyboard keyboard("Enter your desired life count, in hexadecimal.");
		if (keyboard.Open(lives) != -1 && lives >= 0 && Process::Read32(0x101520, offset) && Process::Read32(offset - 0x14, offset) && Process::Read32(offset - 0x10, offset))
			Process::Write32(offset + 0x44, lives);
	}

	void setStarCoins(MenuEntry *entry)
	{
		u32 starCoins = 0;
		Keyboard keyboard("Enter your desired star coin count, in hexadecimal.");
		if (keyboard.Open(starCoins) != -1 && starCoins >= 0 && Process::Read32(0x10230C, offset) && Process::Read32(offset + 0x15C, offset) && Process::Read32(offset - 0x10, offset))
		{
			Process::Write32(offset + 0x1E4, starCoins);
			Process::Write32(offset + 0x1E8, starCoins);
		}
	}

	void setOverworldPowerUp(MenuEntry *entry)
	{
		/*
		14072AEC
		16768264 [[[0x101520] - 0x14] - 0x10] + 0x48 // I chose to use this one since it was near the other codes
		167CBF78
		167DF750
		168E0FE8
		168F47C0
		*/
		struct PowerUp
		{
			const char  *name;
			const u8    value;
		};

		static const std::vector<PowerUp> g_powerUps =
		{
			{ "No Power-Up", 0 },
			{ "Mushroom", 1 },
			{ "Fire Flower", 2 },
			{ "Mini Mushroom", 3 },
			{ "Tanooki Leaf", 4 },
			{ "Golden Flower", 6 },
			{ "Golden Leaf", 7 }
		};

		static StringVector powerUps;
		if (powerUps.empty())
			for (const PowerUp &i : g_powerUps)
				powerUps.push_back(i.name);
		Keyboard keyboard("Power-Up Selector\n\nSelect which Power-Up you'd like to have in the overworld.", powerUps);
		int choice = keyboard.Open();
		if (choice > -1 && Process::Read32(0x101520, offset) && Process::Read32(offset - 0x14, offset) && Process::Read32(offset - 0x10, offset))
			Process::Write8(offset + 0x48, g_powerUps[choice].value);
	}

	void setOverworldCoins(MenuEntry *entry)
	{
		u32 overworldCoins = 0;
		Keyboard keyboard("Enter your desired total coin count, in hexadecimal.");
		if (keyboard.Open(overworldCoins) != -1 && overworldCoins >= 0 && Process::Read32(0x10230C, offset) && Process::Read32(offset - 0xDC, offset) && Process::Read32(offset - 0x40, offset))
			Process::Write32(offset + 0x4F4, overworldCoins);
	}
}